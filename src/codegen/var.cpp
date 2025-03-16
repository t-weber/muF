/**
 * zero-address code generator -- variables and constants
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 10-july-2022
 * @license see 'LICENSE' file
 */

#include "codegen.h"


/**
 * find the symbol with a specific name in the symbol table
 */
t_astret Codegen::GetSym(const t_str& name) const
{
	t_str scoped_name;
	for(const t_str& scope : m_curscope)
		scoped_name += scope + Symbol::get_scopenameseparator();
	scoped_name += name;

	SymbolPtr sym;
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


SymbolPtr Codegen::GetTypeConst(SymbolType ty) const
{
	switch(ty)
	{
		case SymbolType::REAL:
			return m_real_const;
		case SymbolType::INT:
			return m_int_const;
		case SymbolType::CPLX:
			return m_cplx_const;
		case SymbolType::QUAT:
			return m_quat_const;
		case SymbolType::REAL_ARRAY:
			return m_real_array_const;
		case SymbolType::INT_ARRAY:
			return m_int_array_const;
		case SymbolType::CPLX_ARRAY:
			return m_cplx_array_const;
		case SymbolType::QUAT_ARRAY:
			return m_quat_array_const;
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
std::size_t Codegen::GetSymSize(const SymbolPtr sym) const
{
	if(sym->ty == SymbolType::REAL)
		return vm_type_size<VMType::REAL, true>;
	else if(sym->ty == SymbolType::INT)
		return vm_type_size<VMType::INT, true>;
	else if(sym->ty == SymbolType::CPLX)
		return vm_type_size<VMType::CPLX, true>;
	else if(sym->ty == SymbolType::QUAT)
		return vm_type_size<VMType::QUAT, true>;
	else if(sym->ty == SymbolType::REAL_ARRAY)
		return get_vm_vec_real_size(sym->get_total_size(), true, true);
	else if(sym->ty == SymbolType::INT_ARRAY)
		return get_vm_vec_int_size(sym->get_total_size(), true, true);
	else if(sym->ty == SymbolType::CPLX_ARRAY)
		return get_vm_vec_cplx_size(sym->get_total_size(), true, true);
	else if(sym->ty == SymbolType::QUAT_ARRAY)
		return get_vm_vec_quat_size(sym->get_total_size(), true, true);
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
t_astret Codegen::visit(const ASTVarDecl* ast)
{
	bool is_global = !m_curscope.size();
	t_astret sym_ret = nullptr;

	for(const auto& varname : ast->GetVariables())
	{
		// get variable from symbol table and assign an address
		t_astret sym = GetSym(varname);
		if(!sym)
			throw std::runtime_error("ASTVarDecl: Variable \"" + varname + "\" is not in symbol table.");
		if(sym->is_arg)
			continue;  // arguments already declared with function
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
			else if(sym->ty == SymbolType::QUAT)
			{
				PushQuatConst(t_vm_quat(0, 0, 0, 0));
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
				std::vector<t_vm_real> vec(sym->get_total_size());
				PushRealVecConst(vec);
				AssignVar(sym);
			}
			else if(sym->ty == SymbolType::INT_ARRAY)
			{
				std::vector<t_vm_int> vec(sym->get_total_size());
				PushIntVecConst(vec);
				AssignVar(sym);
			}
			else if(sym->ty == SymbolType::CPLX_ARRAY)
			{
				std::vector<t_vm_cplx> vec(sym->get_total_size());
				PushCplxVecConst(vec);
				AssignVar(sym);
			}
			else if(sym->ty == SymbolType::QUAT_ARRAY)
			{
				std::vector<t_vm_quat> vec(sym->get_total_size());
				PushQuatVecConst(vec);
				AssignVar(sym);
			}
		}

		if(!sym_ret)
			sym_ret = sym;
	}

	return sym_ret;
}


/**
 * generate instructions to push a variable onto the stack
 */
t_astret Codegen::PushVar(const t_str& varname)
{
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


t_astret Codegen::visit(const ASTVar* ast)
{
	return PushVar(ast->GetIdent());
}


/**
 * assign symbol variable to current value on the stack
 */
void Codegen::AssignVar(t_astret sym)
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


t_astret Codegen::visit(const ASTAssign* ast)
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


t_astret Codegen::visit([[maybe_unused]] const ASTVarRange* ast)
{
	// handled in ranged loop
	return nullptr;
}
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

void Codegen::PushRealConst(t_vm_real val)
{
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	// write type descriptor byte
	m_ostr->put(static_cast<t_vm_byte>(VMType::REAL));
	// write value
	m_ostr->write(reinterpret_cast<const char*>(&val),
		vm_type_size<VMType::REAL, false>);
}


void Codegen::PushIntConst(t_vm_int val)
{
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	// write type descriptor byte
	m_ostr->put(static_cast<t_vm_byte>(VMType::INT));
	// write data
	m_ostr->write(reinterpret_cast<const char*>(&val),
		vm_type_size<VMType::INT, false>);
}


void Codegen::PushCplxConst(const t_vm_cplx& val)
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


void Codegen::PushQuatConst(const t_vm_quat& val)
{
	t_real real = val.real();
	t_real imag1 = val.imag1();
	t_real imag2 = val.imag2();
	t_real imag3 = val.imag3();

	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	// write type descriptor byte
	m_ostr->put(static_cast<t_vm_byte>(VMType::QUAT));

	// write value components
	m_ostr->write(reinterpret_cast<const char*>(&real),
		vm_type_size<VMType::REAL, false>);
	m_ostr->write(reinterpret_cast<const char*>(&imag1),
		vm_type_size<VMType::REAL, false>);
	m_ostr->write(reinterpret_cast<const char*>(&imag2),
		vm_type_size<VMType::REAL, false>);
	m_ostr->write(reinterpret_cast<const char*>(&imag3),
		vm_type_size<VMType::REAL, false>);
}


void Codegen::PushBoolConst(t_vm_bool val)
{
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	// write type descriptor byte
	m_ostr->put(static_cast<t_vm_byte>(VMType::BOOL));
	// write data
	m_ostr->write(reinterpret_cast<const char*>(&val),
		vm_type_size<VMType::BOOL, false>);
}


void Codegen::PushStrConst(const t_vm_str& val)
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
void Codegen::PushVecSize(std::size_t size)
{
	t_vm_addr num_elems = static_cast<t_vm_addr>(size);
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_MEM));
	m_ostr->write(reinterpret_cast<const char*>(&num_elems),
		vm_type_size<VMType::ADDR_MEM, false>);
}


void Codegen::PushRealVecConst(const std::vector<t_vm_real>& vec)
{
	// push elements
	for(t_vm_real val : vec)
		PushRealConst(val);

	// push number of elements
	PushVecSize(vec.size());

	m_ostr->put(static_cast<t_vm_byte>(OpCode::MAKEREALARR));
}


void Codegen::PushIntVecConst(const std::vector<t_vm_int>& vec)
{
	// push elements
	for(t_vm_int val : vec)
		PushIntConst(val);

	// push number of elements
	PushVecSize(vec.size());

	m_ostr->put(static_cast<t_vm_byte>(OpCode::MAKEINTARR));
}


void Codegen::PushCplxVecConst(const std::vector<t_vm_cplx>& vec)
{
	// push elements
	for(const t_vm_cplx& val : vec)
		PushCplxConst(val);

	// push number of elements
	PushVecSize(vec.size());

	m_ostr->put(static_cast<t_vm_byte>(OpCode::MAKECPLXARR));
}


void Codegen::PushQuatVecConst(const std::vector<t_vm_quat>& vec)
{
	// push elements
	for(const t_vm_quat& val : vec)
		PushQuatConst(val);

	// push number of elements
	PushVecSize(vec.size());

	m_ostr->put(static_cast<t_vm_byte>(OpCode::MAKEQUATARR));
}


t_astret Codegen::visit(const ASTNumConst<t_real>* ast)
{
	t_vm_real val = static_cast<t_vm_real>(ast->GetVal());
	PushRealConst(val);
	return m_real_const;
}


t_astret Codegen::visit(const ASTNumConst<t_int>* ast)
{
	t_vm_int val = static_cast<t_vm_int>(ast->GetVal());
	PushIntConst(val);
	return m_int_const;
}


t_astret Codegen::visit(const ASTNumConst<t_cplx>* ast)
{
	//t_vm_cplx val = static_cast<t_vm_cplx>(ast->GetVal());
	const t_vm_cplx& val = ast->GetVal();
	PushCplxConst(val);
	return m_cplx_const;
}


t_astret Codegen::visit(const ASTNumConst<t_quat>* ast)
{
	//t_vm_quat val = static_cast<t_vm_quat>(ast->GetVal());
	const t_vm_quat& val = ast->GetVal();
	PushQuatConst(val);
	return m_quat_const;
}


t_astret Codegen::visit(const ASTNumConst<bool>* ast)
{
	t_vm_bool val = static_cast<t_vm_bool>(ast->GetVal());
	PushBoolConst(val);
	return m_bool_const;
}


t_astret Codegen::visit(const ASTStrConst* ast)
{
	const t_str& val = ast->GetVal();
	PushStrConst(val);
	return m_str_const;
}


t_astret Codegen::visit(const ASTNumConstList<t_int>*)
{
	// directly handled in grammar
	return nullptr;
}
// ----------------------------------------------------------------------------
