/**
 * zero-address code generator -- conditionals and loops
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 10-july-2022
 * @license see 'LICENSE' file
 */

#include "asm.h"


// ----------------------------------------------------------------------------
// conditionals
// ----------------------------------------------------------------------------
t_astret ZeroACAsm::visit(const ASTCond* ast)
{
	// condition
	ast->GetCond()->accept(this);

	t_vm_addr skipEndCond = 0;         // how many bytes to skip to jump to end of the if block?
	t_vm_addr skipEndIf = 0;           // how many bytes to skip to jump to end of the entire if statement?
	std::streampos skip_addr = 0;      // stream position with the condition jump label
	std::streampos skip_else_addr = 0; // stream position with the if block jump label

	// if the condition is not fulfilled...
	m_ostr->put(static_cast<t_vm_byte>(OpCode::NOT));

	// ...skip to the end of the if block
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));      // push jump address
	m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_IP));
	skip_addr = m_ostr->tellp();
	m_ostr->write(reinterpret_cast<const char*>(&skipEndCond),
		vm_type_size<VMType::ADDR_IP, false>);
	m_ostr->put(static_cast<t_vm_byte>(OpCode::JMPCND));

	// if block
	std::streampos before_if_block = m_ostr->tellp();
	ast->GetIf()->accept(this);
	if(ast->HasElse())
	{
		// skip to end of if statement if there's an else block
		m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));  // push jump address
		m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_IP));
		skip_else_addr = m_ostr->tellp();
		m_ostr->write(reinterpret_cast<const char*>(&skipEndIf),
			vm_type_size<VMType::ADDR_IP, false>);
		m_ostr->put(static_cast<t_vm_byte>(OpCode::JMP));
	}

	std::streampos after_if_block = m_ostr->tellp();

	// go back and fill in missing number of bytes to skip
	skipEndCond = after_if_block - before_if_block;
	m_ostr->seekp(skip_addr);
	m_ostr->write(reinterpret_cast<const char*>(&skipEndCond),
		vm_type_size<VMType::ADDR_IP, false>);
	m_ostr->seekp(0, std::ios_base::end);

	// else block
	if(ast->HasElse())
	{
		std::streampos before_else_block = m_ostr->tellp();
		ast->GetElse()->accept(this);
		std::streampos after_else_block = m_ostr->tellp();

		// go back and fill in missing number of bytes to skip
		skipEndIf = after_else_block - before_else_block;
		m_ostr->seekp(skip_else_addr);
		m_ostr->write(reinterpret_cast<const char*>(&skipEndIf),
			vm_type_size<VMType::ADDR_IP, false>);
	}

	// go to end of stream
	m_ostr->seekp(0, std::ios_base::end);

	return nullptr;
}


t_astret ZeroACAsm::visit(const ASTCases* ast)
{
	std::vector<std::pair<std::streampos, std::streampos>> jump_addrs;
	jump_addrs.reserve(ast->GetCases().size());

	for(auto& [ cond, stmts ] : ast->GetCases())
	{
		// condition for the case: expr == case_cond?
		ast->GetExpr()->accept(this);
		cond->accept(this);
		m_ostr->put(static_cast<t_vm_byte>(OpCode::EQU));

		// if the condition is not fulfilled...
		m_ostr->put(static_cast<t_vm_byte>(OpCode::NOT));

		// ...skip to the end of the case block
		m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));      // push jump address
		m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_IP));
		std::streampos skip_case_addr = m_ostr->tellp();        // stream position with the condition jump label
		t_vm_addr skipEndCond = 0;                              // how many bytes to skip to jump to end of the case block?
		m_ostr->write(reinterpret_cast<const char*>(&skipEndCond),
			vm_type_size<VMType::ADDR_IP, false>);
		m_ostr->put(static_cast<t_vm_byte>(OpCode::JMPCND));

		// run case statements block
		std::streampos before_case_block = m_ostr->tellp();
		stmts->accept(this);

		// skip to the end of all cases
		m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));      // push jump address
		m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_IP));
		std::streampos skip_after_case_addr = m_ostr->tellp();  // stream position with the condition jump label
		t_vm_addr skipEndAllDummy = 0;                          // how many bytes to skip to jump to end of all case blocks?
		m_ostr->write(reinterpret_cast<const char*>(&skipEndAllDummy),
			vm_type_size<VMType::ADDR_IP, false>);
		m_ostr->put(static_cast<t_vm_byte>(OpCode::JMP));
		jump_addrs.emplace_back(std::make_pair(skip_after_case_addr, m_ostr->tellp()));

		// go back and fill in missing number of bytes to skip
		skipEndCond = m_ostr->tellp() - before_case_block;
		m_ostr->seekp(skip_case_addr);
		m_ostr->write(reinterpret_cast<const char*>(&skipEndCond),
			vm_type_size<VMType::ADDR_IP, false>);
		m_ostr->seekp(0, std::ios_base::end);
	}

	// run default case
	if(ast->GetDefaultCase())
		ast->GetDefaultCase()->accept(this);

	// patch-in remaining jump addresses
	std::streampos after_all_cases = m_ostr->tellp();
	for(const auto& [label_addr, jump_addr] : jump_addrs)
	{
		// go back and fill in missing number of bytes to skip
		t_vm_addr to_skip = after_all_cases - jump_addr;
		m_ostr->seekp(label_addr);
		m_ostr->write(reinterpret_cast<const char*>(&to_skip),
			vm_type_size<VMType::ADDR_IP, false>);
	}

	// go to end of stream
	m_ostr->seekp(0, std::ios_base::end);

	return nullptr;
}


t_astret ZeroACAsm::visit(const ASTLoop* ast)
{
	std::size_t loop_ident = ++m_loop_ident;
	m_cur_loop.push_back(loop_ident);

	std::streampos loop_begin = m_ostr->tellp();

	// loop condition
	ast->GetCond()->accept(this);

	// how many bytes to skip to jump to end of the block?
	t_vm_addr skip = 0;
	std::streampos skip_addr = 0;

	// negate loop condition
	m_ostr->put(static_cast<t_vm_byte>(OpCode::NOT));

	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));      // push jump address
	m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_IP));
	skip_addr = m_ostr->tellp();
	m_ostr->write(reinterpret_cast<const char*>(&skip),
		vm_type_size<VMType::ADDR_IP, false>);
	m_ostr->put(static_cast<t_vm_byte>(OpCode::JMPCND));

	// loop statement block
	std::streampos before_block = m_ostr->tellp();
	// loop statements
	ast->GetLoopStmt()->accept(this);

	// loop back
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));      // push jump address
	m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_IP));
	std::streampos after_block = m_ostr->tellp();
	skip = after_block - before_block;
	t_vm_addr skip_back = loop_begin - after_block;
	skip_back -= vm_type_size<VMType::ADDR_IP, true>;
	m_ostr->write(reinterpret_cast<const char*>(&skip_back),
		vm_type_size<VMType::ADDR_IP, false>);
	m_ostr->put(static_cast<t_vm_byte>(OpCode::JMP));

	// go back and fill in missing number of bytes to skip
	after_block = m_ostr->tellp();
	skip = after_block - before_block;
	m_ostr->seekp(skip_addr);
	m_ostr->write(reinterpret_cast<const char*>(&skip),
		vm_type_size<VMType::ADDR_IP, false>);

	// fill in any saved, unset start-of-loop jump addresses (continues)
	while(true)
	{
		auto iter = m_loop_begin_comefroms.find(loop_ident);
		if(iter == m_loop_begin_comefroms.end())
			break;

		std::streampos pos = iter->second;
		m_loop_begin_comefroms.erase(iter);

		t_vm_addr to_skip = loop_begin - pos;
		// already skipped over address and jmp instruction
		to_skip -= vm_type_size<VMType::ADDR_IP, true>;
		m_ostr->seekp(pos);
		m_ostr->write(reinterpret_cast<const char*>(&to_skip),
			vm_type_size<VMType::ADDR_IP, false>);
	}

	// fill in any saved, unset end-of-loop jump addresses (breaks)
	while(true)
	{
		auto iter = m_loop_end_comefroms.find(loop_ident);
		if(iter == m_loop_end_comefroms.end())
			break;

		std::streampos pos = iter->second;
		m_loop_end_comefroms.erase(iter);

		t_vm_addr to_skip = after_block - pos;
		// already skipped over address and jmp instruction
		to_skip -= vm_type_size<VMType::ADDR_IP, true>;
		m_ostr->seekp(pos);
		m_ostr->write(reinterpret_cast<const char*>(&to_skip),
			vm_type_size<VMType::ADDR_IP, false>);
	}

	// go to end of stream
	m_ostr->seekp(0, std::ios_base::end);
	m_cur_loop.pop_back();

	return nullptr;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// loops
// ----------------------------------------------------------------------------
t_astret ZeroACAsm::visit(const ASTRangedLoop* ast)
{
	// --------------------------------------------------------------------
	// assign initial counter variable
	const t_str& ctrvar_ident = ast->GetRange()->GetIdent();

	// expression for the counter's initial value
	ast->GetRange()->GetBegin()->accept(this);

	t_astret ctr_sym = GetSym(ctrvar_ident);
	if(!ctr_sym)
		throw std::runtime_error("ASTRangedLoop: Counter variable \"" + ctrvar_ident + "\" is not in symbol table.");
	if(!ctr_sym->addr)
		throw std::runtime_error("ASTRangedLoop: Counter variable \"" + ctrvar_ident + "\" has not been declared.");

	CastTo(ctr_sym, std::nullopt, true);
	AssignVar(ctr_sym);
	// --------------------------------------------------------------------

	// start loop
	std::size_t loop_ident = ++m_loop_ident;
	m_cur_loop.push_back(loop_ident);

	std::streampos loop_begin = m_ostr->tellp();

	// --------------------------------------------------------------------
	// loop condition: check if the counter is smaller than the end value
	// push counter variable address
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	m_ostr->put(static_cast<t_vm_byte>(
		ctr_sym->is_global ? VMType::ADDR_GBP : VMType::ADDR_BP));
	t_vm_addr addr = static_cast<t_vm_addr>(*ctr_sym->addr);
	m_ostr->write(reinterpret_cast<const char*>(&addr),
		vm_type_size<VMType::ADDR_BP, false>);
	// dereference counter variable
	m_ostr->put(static_cast<t_vm_byte>(OpCode::RDMEM));

	// end value
	ast->GetRange()->GetEnd()->accept(this);

	// ctr <= end ?
	m_ostr->put(static_cast<t_vm_byte>(OpCode::LEQU));
	// --------------------------------------------------------------------

	// how many bytes to skip to jump to end of the block?
	t_vm_addr skip = 0;
	std::streampos skip_addr = 0;

	// negate loop condition
	m_ostr->put(static_cast<t_vm_byte>(OpCode::NOT));

	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));      // push jump address
	m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_IP));
	skip_addr = m_ostr->tellp();
	m_ostr->write(reinterpret_cast<const char*>(&skip),
		vm_type_size<VMType::ADDR_IP, false>);
	m_ostr->put(static_cast<t_vm_byte>(OpCode::JMPCND));

	// --------------------------------------------------------------------
	// loop statement block
	std::streampos before_block = m_ostr->tellp();
	// loop statements
	ast->GetLoopStmt()->accept(this);
	// --------------------------------------------------------------------

	// --------------------------------------------------------------------
	// increment counter
	if(ast->GetRange()->GetInc())
		ast->GetRange()->GetInc()->accept(this);
	// increment by 1 if nothing is given
	else
		PushIntConst(1);

	// push counter variable address
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	m_ostr->put(static_cast<t_vm_byte>(
		ctr_sym->is_global ? VMType::ADDR_GBP : VMType::ADDR_BP));
	m_ostr->write(reinterpret_cast<const char*>(&addr),
		vm_type_size<VMType::ADDR_BP, false>);
	// dereference counter variable
	m_ostr->put(static_cast<t_vm_byte>(OpCode::RDMEM));

	// add counter and increment and re-assign to counter
	// TODO: casts
	m_ostr->put(static_cast<t_vm_byte>(OpCode::ADD));
	AssignVar(ctr_sym);
	// --------------------------------------------------------------------

	// loop back
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));      // push jump address
	m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_IP));
	std::streampos after_block = m_ostr->tellp();
	skip = after_block - before_block;
	t_vm_addr skip_back = loop_begin - after_block;
	skip_back -= vm_type_size<VMType::ADDR_IP, true>;
	m_ostr->write(reinterpret_cast<const char*>(&skip_back),
		vm_type_size<VMType::ADDR_IP, false>);
	m_ostr->put(static_cast<t_vm_byte>(OpCode::JMP));

	// go back and fill in missing number of bytes to skip
	after_block = m_ostr->tellp();
	skip = after_block - before_block;
	m_ostr->seekp(skip_addr);
	m_ostr->write(reinterpret_cast<const char*>(&skip),
		vm_type_size<VMType::ADDR_IP, false>);

	// fill in any saved, unset start-of-loop jump addresses (continues)
	while(true)
	{
		auto iter = m_loop_begin_comefroms.find(loop_ident);
		if(iter == m_loop_begin_comefroms.end())
			break;

		std::streampos pos = iter->second;
		m_loop_begin_comefroms.erase(iter);

		t_vm_addr to_skip = loop_begin - pos;
		// already skipped over address and jmp instruction
		to_skip -= vm_type_size<VMType::ADDR_IP, true>;
		m_ostr->seekp(pos);
		m_ostr->write(reinterpret_cast<const char*>(&to_skip),
			vm_type_size<VMType::ADDR_IP, false>);
	}

	// fill in any saved, unset end-of-loop jump addresses (breaks)
	while(true)
	{
		auto iter = m_loop_end_comefroms.find(loop_ident);
		if(iter == m_loop_end_comefroms.end())
			break;

		std::streampos pos = iter->second;
		m_loop_end_comefroms.erase(iter);

		t_vm_addr to_skip = after_block - pos;
		// already skipped over address and jmp instruction
		to_skip -= vm_type_size<VMType::ADDR_IP, true>;
		m_ostr->seekp(pos);
		m_ostr->write(reinterpret_cast<const char*>(&to_skip),
			vm_type_size<VMType::ADDR_IP, false>);
	}

	// go to end of stream
	m_ostr->seekp(0, std::ios_base::end);
	m_cur_loop.pop_back();

	return nullptr;
}


t_astret ZeroACAsm::visit(const ASTLoopBreak* ast)
{
	//if(!m_curscope.size())
	//	throw std::runtime_error("ASTLoopBreak: Not in a function.");
	if(!m_cur_loop.size())
		throw std::runtime_error("ASTLoopBreak: Not in a loop.");

	t_int loop_depth = ast->GetNumLoops();

	// reduce to maximum loop depth
	if(static_cast<std::size_t>(loop_depth) >= m_cur_loop.size() || loop_depth < 0)
		loop_depth = static_cast<t_int>(m_cur_loop.size()-1);

	// jump to the end of the loop
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));  // push jump address
	m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_IP));
	m_loop_end_comefroms.insert(std::make_pair(
		m_cur_loop[m_cur_loop.size()-loop_depth-1], m_ostr->tellp()));
	t_vm_addr dummy_addr = 0;
	m_ostr->write(reinterpret_cast<const char*>(&dummy_addr),
		vm_type_size<VMType::ADDR_IP, false>);
	m_ostr->put(static_cast<t_vm_byte>(OpCode::JMP));

	return nullptr;
}


t_astret ZeroACAsm::visit(const ASTLoopNext* ast)
{
	//if(!m_curscope.size())
	//	throw std::runtime_error("ASTLoopNext: Not in a function.");
	if(!m_cur_loop.size())
		throw std::runtime_error("ASTLoopNext: Not in a loop.");

	t_int loop_depth = ast->GetNumLoops();

	// reduce to maximum loop depth
	if(static_cast<std::size_t>(loop_depth) >= m_cur_loop.size() || loop_depth < 0)
		loop_depth = static_cast<t_int>(m_cur_loop.size()-1);

	// jump to the beginning of the loop
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));  // push jump address
	m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_IP));
	m_loop_begin_comefroms.insert(std::make_pair(
		m_cur_loop[m_cur_loop.size()-loop_depth-1], m_ostr->tellp()));
	t_vm_addr dummy_addr = 0;
	m_ostr->write(reinterpret_cast<const char*>(&dummy_addr),
		vm_type_size<VMType::ADDR_IP, false>);
	m_ostr->put(static_cast<t_vm_byte>(OpCode::JMP));

	return nullptr;
}


t_astret ZeroACAsm::visit(const ASTLabel* ast)
{
	// save current stream position
	std::streampos addr = m_ostr->tellp();
	m_labels.emplace(std::make_pair(ast->GetIdent(), addr));

	return nullptr;
}


t_astret ZeroACAsm::visit(const ASTJump* ast)
{
	if(ast->IsComefrom())
		throw std::runtime_error("Comefrom is not (yet) implemented...");

	// jump to the label
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));  // push jump address
	m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_IP));
	m_goto_comefroms.emplace_back(std::make_pair(ast->GetLabel(), m_ostr->tellp()));
	t_vm_addr dummy_addr = 0;
	m_ostr->write(reinterpret_cast<const char*>(&dummy_addr),
		vm_type_size<VMType::ADDR_IP, false>);
	m_ostr->put(static_cast<t_vm_byte>(OpCode::JMP));

	return nullptr;
}
// ----------------------------------------------------------------------------
