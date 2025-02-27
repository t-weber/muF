/**
 * zero-address code generator
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 10-july-2022
 * @license see 'LICENSE' file
 */

#include "asm.h"


/**
 * find the symbol with a specific name in the symbol table
 */
t_astret ZeroACAsm::GetSym(const t_str& name) const
{
	t_str scoped_name;
	for(const t_str& scope : m_curscope)
		scoped_name += scope + Symbol::get_scopenameseparator();
	scoped_name += name;

	Symbol* sym = nullptr;
	if(m_syms)
	{
		sym = m_syms->FindSymbol(scoped_name);

		// try global scope instead
		if(!sym)
			sym = m_syms->FindSymbol(name);
	}

	if(!sym)
	{
		throw std::runtime_error("GetSym: \"" + scoped_name +
			"\" does not have an associated symbol.");
	}

	return sym;
}


Symbol* ZeroACAsm::GetTypeConst(SymbolType ty) const
{
	switch(ty)
	{
		case SymbolType::REAL:
			return m_real_const;
		case SymbolType::INT:
			return m_int_const;
		case SymbolType::CPLX:
			return m_cplx_const;
		case SymbolType::REAL_ARRAY:
			return m_real_array_const;
		case SymbolType::INT_ARRAY:
			return m_int_array_const;
		case SymbolType::CPLX_ARRAY:
			return m_cplx_array_const;
		case SymbolType::BOOL:
			return m_bool_const;
		case SymbolType::STRING:
			return m_str_const;
		default:
			return nullptr;
	}
	return nullptr;
}


/**
 * finds the size of the symbol for the stack frame
 */
std::size_t ZeroACAsm::GetSymSize(const Symbol* sym) const
{
	if(sym->ty == SymbolType::REAL)
		return vm_type_size<VMType::REAL, true>;
	else if(sym->ty == SymbolType::INT)
		return vm_type_size<VMType::INT, true>;
	else if(sym->ty == SymbolType::CPLX)
		return vm_type_size<VMType::CPLX, true>;
	else if(sym->ty == SymbolType::REAL_ARRAY)
		return get_vm_vec_real_size(sym->dims[0], true, true);
	else if(sym->ty == SymbolType::INT_ARRAY)
		return get_vm_vec_int_size(sym->dims[0], true, true);
	else if(sym->ty == SymbolType::CPLX_ARRAY)
		return get_vm_vec_cplx_size(sym->dims[0], true, true);
	else if(sym->ty == SymbolType::BOOL)
		return vm_type_size<VMType::BOOL, true>;
	else if(sym->ty == SymbolType::STRING)
		return get_vm_str_size(sym->dims[0], true, true);
	else
		throw std::runtime_error("Invalid symbol type for \"" + sym->name + "\".");

	return 0;
}



// ----------------------------------------------------------------------------
// variables
// ----------------------------------------------------------------------------
t_astret ZeroACAsm::visit(const ASTVarDecl* ast)
{
	bool is_global = !m_curscope.size();
	t_astret sym_ret = nullptr;

	for(const auto& varname : ast->GetVariables())
	{
		// get variable from symbol table and assign an address
		t_astret sym = GetSym(varname);
		if(!sym)
			throw std::runtime_error("ASTVarDecl: Variable \"" + varname + "\" is not in symbol table.");
		if(sym->addr)
			throw std::runtime_error("ASTVarDecl: Variable \"" + varname + "\" already declared.");

		sym->is_global = is_global;
		if(is_global)
		{
			m_global_stack += GetSymSize(sym);
			sym->addr = -m_global_stack;
		}
		else
		{
			const t_str& cur_func = *m_curscope.rbegin();

			// start of local stack frame?
			if(m_local_stack.find(cur_func) == m_local_stack.end())
				m_local_stack[cur_func] = 0;

			m_local_stack[cur_func] += GetSymSize(sym);
			sym->addr = -m_local_stack[cur_func];
		}

		if(ast->GetAssignment())
		{
			// initialise variable using given assignment
			ast->GetAssignment()->accept(this);
		}
		else
		{
			// initialise variable to 0 if no assignment is given
			if(sym->ty == SymbolType::INT)
			{
				PushIntConst(t_vm_int(0));
				AssignVar(sym);
			}
			else if(sym->ty == SymbolType::REAL)
			{
				PushRealConst(t_vm_real(0));
				AssignVar(sym);
			}
			else if(sym->ty == SymbolType::CPLX)
			{
				PushCplxConst(t_vm_cplx(0, 0));
				AssignVar(sym);
			}
			else if(sym->ty == SymbolType::BOOL)
			{
				PushBoolConst(t_vm_real(0));
				AssignVar(sym);
			}
			else if(sym->ty == SymbolType::STRING)
			{
				PushStrConst(t_vm_str(""));
				AssignVar(sym);
			}
			else if(sym->ty == SymbolType::REAL_ARRAY)
			{
				std::vector<t_vm_real> vec(sym->dims[0]);
				PushRealVecConst(vec);
				AssignVar(sym);
			}
			else if(sym->ty == SymbolType::INT_ARRAY)
			{
				std::vector<t_vm_int> vec(sym->dims[0]);
				PushIntVecConst(vec);
				AssignVar(sym);
			}
			else if(sym->ty == SymbolType::CPLX_ARRAY)
			{
				std::vector<t_vm_cplx> vec(sym->dims[0]);
				PushCplxVecConst(vec);
				AssignVar(sym);
			}
		}

		if(!sym_ret)
			sym_ret = sym;
	}

	return sym_ret;
}


t_astret ZeroACAsm::visit(const ASTVar* ast)
{
	const t_str& varname = ast->GetIdent();

	// get variable from symbol table
	t_astret sym = GetSym(varname);
	if(!sym)
		throw std::runtime_error("ASTVar: Variable \"" + varname + "\" is not in symbol table.");
	if(!sym->addr)
		throw std::runtime_error("ASTVar: Variable \"" + varname + "\" has not been declared.");

	// push variable address
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	m_ostr->put(static_cast<t_vm_byte>(
		sym->is_global ? VMType::ADDR_GBP : VMType::ADDR_BP));
	t_vm_addr addr = static_cast<t_vm_addr>(*sym->addr);
	m_ostr->write(reinterpret_cast<const char*>(&addr),
		vm_type_size<VMType::ADDR_BP, false>);

	// dereference the variable
	if(sym->ty != SymbolType::FUNC)
		m_ostr->put(static_cast<t_vm_byte>(OpCode::RDMEM));

	return sym;
}


/**
 * assign symbol variable to current value on the stack
 */
void ZeroACAsm::AssignVar(t_astret sym)
{
	// push variable address
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	m_ostr->put(static_cast<t_vm_byte>(
		sym->is_global ? VMType::ADDR_GBP : VMType::ADDR_BP));
	t_vm_addr addr = static_cast<t_vm_addr>(*sym->addr);
	m_ostr->write(reinterpret_cast<const char*>(&addr),
		vm_type_size<VMType::ADDR_BP, false>);

	// assign variable
	m_ostr->put(static_cast<t_vm_byte>(OpCode::WRMEM));
}


t_astret ZeroACAsm::visit(const ASTAssign* ast)
{
	if(ast->GetExpr())
		ast->GetExpr()->accept(this);
	t_astret sym_ret = nullptr;

	for(const t_str& varname : ast->GetIdents())
	{
		t_astret sym = GetSym(varname);
		if(!sym)
			throw std::runtime_error("ASTAssign: Variable \"" + varname + "\" is not in symbol table.");
		if(!sym->addr)
			throw std::runtime_error("ASTAssign: Variable \"" + varname + "\" has not been declared.");

		CastTo(sym, std::nullopt, true);
		AssignVar(sym);

		if(!sym_ret)
			sym_ret = sym;
	}

	return sym_ret;
}


t_astret ZeroACAsm::visit([[maybe_unused]] const ASTVarRange* ast)
{
	// handled in ranged loop
	return nullptr;
}
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

void ZeroACAsm::PushRealConst(t_vm_real val)
{
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	// write type descriptor byte
	m_ostr->put(static_cast<t_vm_byte>(VMType::REAL));
	// write value
	m_ostr->write(reinterpret_cast<const char*>(&val),
		vm_type_size<VMType::REAL, false>);
}


void ZeroACAsm::PushIntConst(t_vm_int val)
{
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	// write type descriptor byte
	m_ostr->put(static_cast<t_vm_byte>(VMType::INT));
	// write data
	m_ostr->write(reinterpret_cast<const char*>(&val),
		vm_type_size<VMType::INT, false>);
}


void ZeroACAsm::PushCplxConst(const t_vm_cplx& val)
{
	t_real real = val.real();
	t_real imag = val.imag();

	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	// write type descriptor byte
	m_ostr->put(static_cast<t_vm_byte>(VMType::CPLX));

	// write value components
	m_ostr->write(reinterpret_cast<const char*>(&real),
		vm_type_size<VMType::REAL, false>);
	m_ostr->write(reinterpret_cast<const char*>(&imag),
		vm_type_size<VMType::REAL, false>);
}


void ZeroACAsm::PushBoolConst(t_vm_bool val)
{
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	// write type descriptor byte
	m_ostr->put(static_cast<t_vm_byte>(VMType::BOOL));
	// write data
	m_ostr->write(reinterpret_cast<const char*>(&val),
		vm_type_size<VMType::BOOL, false>);
}


void ZeroACAsm::PushStrConst(const t_vm_str& val)
{
	// get string constant address
	std::streampos str_addr = m_consttab.AddConst(val);

	// push string constant address
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_IP));

	std::streampos addr_pos = m_ostr->tellp();
	str_addr -= addr_pos;
	str_addr -= static_cast<std::streampos>(
		vm_type_size<VMType::ADDR_IP, true>);

	m_const_addrs.push_back(std::make_tuple(addr_pos, str_addr));

	m_ostr->write(reinterpret_cast<const char*>(&str_addr),
		vm_type_size<VMType::ADDR_MEM, false>);

	// dereference string constant address
	m_ostr->put(static_cast<t_vm_byte>(OpCode::RDMEM));
}


/**
 * push the number of elements in an array
 */
void ZeroACAsm::PushVecSize(std::size_t size)
{
	t_vm_addr num_elems = static_cast<t_vm_addr>(size);
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_MEM));
	m_ostr->write(reinterpret_cast<const char*>(&num_elems),
		vm_type_size<VMType::ADDR_MEM, false>);
}


void ZeroACAsm::PushRealVecConst(const std::vector<t_vm_real>& vec)
{
	// push elements
	for(t_vm_real val : vec)
		PushRealConst(val);

	// push number of elements
	PushVecSize(vec.size());

	m_ostr->put(static_cast<t_vm_byte>(OpCode::MAKEREALARR));
}


void ZeroACAsm::PushIntVecConst(const std::vector<t_vm_int>& vec)
{
	// push elements
	for(t_vm_int val : vec)
		PushIntConst(val);

	// push number of elements
	PushVecSize(vec.size());

	m_ostr->put(static_cast<t_vm_byte>(OpCode::MAKEINTARR));
}


void ZeroACAsm::PushCplxVecConst(const std::vector<t_vm_cplx>& vec)
{
	// push elements
	for(const t_vm_cplx& val : vec)
		PushCplxConst(val);

	// push number of elements
	PushVecSize(vec.size());

	m_ostr->put(static_cast<t_vm_byte>(OpCode::MAKECPLXARR));
}


t_astret ZeroACAsm::visit(const ASTNumConst<t_real>* ast)
{
	t_vm_real val = static_cast<t_vm_real>(ast->GetVal());
	PushRealConst(val);
	return m_real_const;
}


t_astret ZeroACAsm::visit(const ASTNumConst<t_int>* ast)
{
	t_vm_int val = static_cast<t_vm_int>(ast->GetVal());
	PushIntConst(val);
	return m_int_const;
}


t_astret ZeroACAsm::visit(const ASTNumConst<t_cplx>* ast)
{
	t_vm_cplx val = static_cast<t_vm_cplx>(ast->GetVal());
	PushCplxConst(val);
	return m_cplx_const;
}


t_astret ZeroACAsm::visit(const ASTNumConst<bool>* ast)
{
	t_vm_bool val = static_cast<t_vm_bool>(ast->GetVal());
	PushBoolConst(val);
	return m_bool_const;
}


t_astret ZeroACAsm::visit(const ASTStrConst* ast)
{
	const t_str& val = ast->GetVal();
	PushStrConst(val);
	return m_str_const;
}
// ----------------------------------------------------------------------------
