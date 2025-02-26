/**
 * zero-address code generator
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 10-july-2022
 * @license see 'LICENSE' file
 */

#include "asm.h"


// ----------------------------------------------------------------------------
// arrays
// ----------------------------------------------------------------------------
std::pair<Symbol*, Symbol*> ZeroACAsm::GetArrayTypeConst(SymbolType ty) const
{
	Symbol *arr_ty = nullptr;
	Symbol *arr_elem_ty = nullptr;

	switch(ty)
	{
		case SymbolType::STRING:
			arr_ty = m_str_const;
			arr_elem_ty = m_str_const;
			break;
		case SymbolType::REAL_ARRAY:
			arr_ty = m_real_array_const;
			arr_elem_ty = m_real_const;
			break;
		case SymbolType::INT_ARRAY:
			arr_ty = m_int_array_const;
			arr_elem_ty = m_int_const;
			break;
		case SymbolType::CPLX_ARRAY:
			arr_ty = m_cplx_array_const;
			arr_elem_ty = m_cplx_const;
			break;
		default:
			break;
	}

	return std::make_pair(arr_ty, arr_elem_ty);
}


t_astret ZeroACAsm::visit(const ASTArrayAccess* ast)
{
	t_astret term = ast->GetTerm()->accept(this);

	bool ranged12 = ast->IsRanged12();
	bool ranged34 = ast->IsRanged34();

	const ASTPtr num1 = ast->GetNum1();
	const ASTPtr num2 = ast->GetNum2();
	const ASTPtr num3 = ast->GetNum3();
	const ASTPtr num4 = ast->GetNum4();

	// single-element array access
	if(!ranged12 && !ranged34 && num1 && !num2 && !num3 && !num4)
	{
		t_astret num1sym = num1->accept(this);
		if(num1sym->ty != SymbolType::INT)
			CastTo(m_int_const);

		m_ostr->put(static_cast<t_vm_byte>(OpCode::RDARR));

		if(auto [arr_ty, arr_elem_ty] = GetArrayTypeConst(term->ty); arr_elem_ty)
			return arr_elem_ty;

		throw std::runtime_error("ASTArrayAccess: Invalid array type of \"" + term->name + "\".");
	}

	// ranged array access
	else if(ranged12 && !ranged34 && num1 && num2 && !num3 && !num4)
	{
		t_astret num1sym = num1->accept(this);
		if(num1sym->ty != SymbolType::INT)
			CastTo(m_int_const);
		t_astret num2sym = num2->accept(this);
		if(num2sym->ty != SymbolType::INT)
			CastTo(m_int_const);

		m_ostr->put(static_cast<t_vm_byte>(OpCode::RDARRR));

		if(auto [arr_ty, arr_elem_ty] = GetArrayTypeConst(term->ty); arr_elem_ty)
			return arr_ty;

		throw std::runtime_error("ASTArrayAccess: Invalid array type of \"" + term->name + "\".");
	}

	throw std::runtime_error("ASTArrayAccess: Invalid array access to \"" + term->name + "\".");
}


t_astret ZeroACAsm::visit(const ASTArrayAssign* ast)
{
	// get variable from symbol table
	const t_str& varname = ast->GetIdent();
	t_astret sym = GetSym(varname);
	if(!sym)
		throw std::runtime_error("ASTArrayAssign: Variable \"" + varname + "\" is not in symbol table.");
	if(!sym->addr)
		throw std::runtime_error("ASTArrayAssign: Variable \"" + varname + "\" has not been declared.");

	// push variable address
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	m_ostr->put(static_cast<t_vm_byte>(
		sym->is_global ? VMType::ADDR_GBP : VMType::ADDR_BP));
	t_vm_addr addr = static_cast<t_vm_addr>(*sym->addr);
	m_ostr->write(reinterpret_cast<const char*>(&addr), vm_type_size<VMType::ADDR_BP, false>);

	// evaluate the rhs expression
	t_astret expr = ast->GetExpr()->accept(this);

	bool ranged12 = ast->IsRanged12();
	bool ranged34 = ast->IsRanged34();

	const ASTPtr num1 = ast->GetNum1();
	const ASTPtr num2 = ast->GetNum2();
	const ASTPtr num3 = ast->GetNum3();
	const ASTPtr num4 = ast->GetNum4();

	// single-element array assignment
	if(!ranged12 && !ranged34 && num1 && !num2 && !num3 && !num4)
	{
		Symbol *elem_sym_type = nullptr;
		if(auto [arr_ty, arr_elem_ty] = GetArrayTypeConst(sym->ty); arr_elem_ty)
			elem_sym_type = arr_elem_ty;

		if(!elem_sym_type)
			throw std::runtime_error("ASTArrayAssign: Invalid array element type in \"" + varname + "\".");

		if(expr->ty != sym->ty)
			CastTo(elem_sym_type);

		t_astret num1sym = num1->accept(this);
		if(num1sym->ty != SymbolType::INT)
			CastTo(m_int_const);

		m_ostr->put(static_cast<t_vm_byte>(OpCode::WRARR));
	}

	// ranged array assignment
	else if(ranged12 && !ranged34 && num1 && num2 && !num3 && !num4)
	{
		t_astret num1sym = num1->accept(this);
		if(num1sym->ty != SymbolType::INT)
			CastTo(m_int_const);
		t_astret num2sym = num2->accept(this);
		if(num2sym->ty != SymbolType::INT)
			CastTo(m_int_const);

		m_ostr->put(static_cast<t_vm_byte>(OpCode::WRARRR));
	}

	return expr;
}


t_astret ZeroACAsm::visit(const ASTExprList* ast)
{
	t_astret sym_ret = nullptr;
	const bool is_arr = ast->IsArray();
	const SymbolType arr_sym_ty = ast->GetArrayType();

	auto [arr_ty, arr_elem_ty] = GetArrayTypeConst(arr_sym_ty);
	if(!arr_ty || !arr_elem_ty)
	{
		arr_ty = m_real_array_const;
		arr_elem_ty = m_real_const;
	}

	t_vm_addr num_elems = 0;
	for(const auto& elem : ast->GetList())
	{
		t_astret sym = elem->accept(this);

		// make sure all (array) elements are of the element type
		if(is_arr)
			CastTo(arr_elem_ty);

		if(!sym_ret)
			sym_ret = sym;

		++num_elems;
	}

	// create a vector out of the elements on the stack
	if(is_arr)
	{
		// push number of elements
		m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
		m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_MEM));
		m_ostr->write(reinterpret_cast<const char*>(&num_elems),
			vm_type_size<VMType::ADDR_MEM, false>);

		if(arr_sym_ty == SymbolType::REAL_ARRAY)
			m_ostr->put(static_cast<t_vm_byte>(OpCode::MAKEREALARR));
		else if(arr_sym_ty == SymbolType::INT_ARRAY)
			m_ostr->put(static_cast<t_vm_byte>(OpCode::MAKEINTARR));
		else if(arr_sym_ty == SymbolType::CPLX_ARRAY)
			m_ostr->put(static_cast<t_vm_byte>(OpCode::MAKECPLXARR));
		else
			 throw std::runtime_error("ASTExprList: Invalid array type.");
		sym_ret = arr_ty;
	}

	return sym_ret;
}
// ----------------------------------------------------------------------------
