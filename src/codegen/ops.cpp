/**
 * zero-address code generator -- operators
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 10-july-2022
 * @license see 'LICENSE' file
 */

#include "codegen.h"


// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------
/**
 * returns common type of a binary operation
 * @returns [ first type cast, second type cast, operation result type ]
 */
std::tuple<t_astret, t_astret, t_astret>
Codegen::GetCastSymType(t_astret term1, t_astret term2)
{
	if(!term1 || !term2)
		return std::make_tuple(nullptr, nullptr, nullptr);

	SymbolType ty1 = term1->ty;
	SymbolType ty2 = term2->ty;

	// use return type for function
	if(ty1 == SymbolType::FUNC)
		term1 = GetTypeConst(term1->retty);
	if(ty2 == SymbolType::FUNC)
		term2 = GetTypeConst(term2->retty);

	// already same type?
	if(ty1 == ty2)
		return std::make_tuple(nullptr, nullptr, term1);

	else if(ty1 == SymbolType::INT && ty2 == SymbolType::REAL)
		return std::make_tuple(term2, nullptr, term2);
	else if(ty1 == SymbolType::REAL && ty2 == SymbolType::INT)
		return std::make_tuple(nullptr, term1, term1);

	else if(ty1 == SymbolType::INT && ty2 == SymbolType::CPLX)
		return std::make_tuple(term2, nullptr, term2);
	else if(ty1 == SymbolType::CPLX && ty2 == SymbolType::INT)
		return std::make_tuple(nullptr, term1, term1);

	else if(ty1 == SymbolType::REAL && ty2 == SymbolType::CPLX)
		return std::make_tuple(term2, nullptr, term2);
	else if(ty1 == SymbolType::CPLX && ty2 == SymbolType::REAL)
		return std::make_tuple(nullptr, term1, term1);

	else if(ty1 == SymbolType::STRING && ty2 == SymbolType::REAL)
		return std::make_tuple(nullptr, term1, term1);
	else if(ty1 == SymbolType::STRING && ty2 == SymbolType::INT)
		return std::make_tuple(nullptr, term1, term1);
	else if(ty1 == SymbolType::REAL && ty2 == SymbolType::STRING)
		return std::make_tuple(term2, nullptr, term2);
	else if(ty1 == SymbolType::INT && ty2 == SymbolType::STRING)
		return std::make_tuple(term2, nullptr, term2);

	// no casts between real array/scalar operations
	else if(ty1 == SymbolType::REAL_ARRAY && ty2 == SymbolType::REAL)
		return std::make_tuple(nullptr, nullptr, term1);
	else if(ty1 == SymbolType::REAL_ARRAY && ty2 == SymbolType::INT)
		return std::make_tuple(nullptr, m_real_const, term1);
	else if(ty1 == SymbolType::REAL && ty2 == SymbolType::REAL_ARRAY)
		return std::make_tuple(nullptr, nullptr, term2);
	else if(ty1 == SymbolType::INT && ty2 == SymbolType::REAL_ARRAY)
		return std::make_tuple(m_real_const, nullptr, term2);

	// no casts between int array/scalar operations
	else if(ty1 == SymbolType::INT_ARRAY && ty2 == SymbolType::INT)
		return std::make_tuple(nullptr, nullptr, term1);
	else if(ty1 == SymbolType::INT_ARRAY && ty2 == SymbolType::REAL)
		return std::make_tuple(nullptr, m_int_const, term1);
	else if(ty1 == SymbolType::INT && ty2 == SymbolType::INT_ARRAY)
		return std::make_tuple(nullptr, nullptr, term2);
	else if(ty1 == SymbolType::REAL && ty2 == SymbolType::INT_ARRAY)
		return std::make_tuple(m_int_const, nullptr, term2);

	// no casts between complex array/scalar operations
	else if(ty1 == SymbolType::CPLX_ARRAY && ty2 == SymbolType::CPLX)
		return std::make_tuple(nullptr, nullptr, term1);
	else if(ty1 == SymbolType::CPLX_ARRAY && ty2 == SymbolType::INT)
		return std::make_tuple(nullptr, m_cplx_const, term1);
	else if(ty1 == SymbolType::CPLX_ARRAY && ty2 == SymbolType::REAL)
		return std::make_tuple(nullptr, m_cplx_const, term1);
	else if(ty1 == SymbolType::CPLX && ty2 == SymbolType::CPLX_ARRAY)
		return std::make_tuple(nullptr, nullptr, term2);
	else if(ty1 == SymbolType::INT && ty2 == SymbolType::CPLX_ARRAY)
		return std::make_tuple(m_cplx_const, nullptr, term2);
	else if(ty1 == SymbolType::REAL && ty2 == SymbolType::CPLX_ARRAY)
		return std::make_tuple(m_cplx_const, nullptr, term2);

	return std::make_tuple(nullptr, term1, term1);
}


/**
 * emit code to cast to given type
 */
void Codegen::CastTo(t_astret ty_to,
	const std::optional<std::streampos>& pos,
	bool allow_array_cast)
{
	if(!ty_to)
		return;

	t_vm_byte op = static_cast<t_vm_byte>(OpCode::NOP);
	bool to_arr = false;

	if(ty_to->ty == SymbolType::REAL)
		op = static_cast<t_vm_byte>(OpCode::TOR);
	else if(ty_to->ty == SymbolType::INT)
		op = static_cast<t_vm_byte>(OpCode::TOI);
	else if(ty_to->ty == SymbolType::CPLX)
		op = static_cast<t_vm_byte>(OpCode::TOC);
	else if(ty_to->ty == SymbolType::STRING)
		op = static_cast<t_vm_byte>(OpCode::TOS);
	else if(ty_to->ty == SymbolType::BOOL)
		op = static_cast<t_vm_byte>(OpCode::TOB);
	else if(ty_to->ty == SymbolType::REAL_ARRAY && allow_array_cast)
	{
		op = static_cast<t_vm_byte>(OpCode::TOREALARR);
		to_arr = true;
	}
	else if(ty_to->ty == SymbolType::INT_ARRAY && allow_array_cast)
	{
		op = static_cast<t_vm_byte>(OpCode::TOINTARR);
		to_arr = true;
	}
	else if(ty_to->ty == SymbolType::CPLX_ARRAY && allow_array_cast)
	{
		op = static_cast<t_vm_byte>(OpCode::TOCPLXARR);
		to_arr = true;
	}

	// target type is an array
	if(to_arr)
	{
		// TODO: this doesn't work if "pos" is also given
		// push vector length
		t_vm_addr cols = static_cast<t_vm_addr>(ty_to->get_total_size());

		m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
		m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_MEM));
		m_ostr->write(reinterpret_cast<const char*>(&cols),
			vm_type_size<VMType::ADDR_MEM, false>);
	}

	if(pos)
		m_ostr->seekp(*pos);
	m_ostr->write(reinterpret_cast<const char*>(&op), sizeof(t_vm_byte));
	if(pos)
		m_ostr->seekp(0, std::ios_base::end);
}


t_astret Codegen::visit(const ASTUMinus* ast)
{
	t_astret term = ast->GetTerm()->accept(this);
	m_ostr->put(static_cast<t_vm_byte>(OpCode::USUB));

	return term;
}


t_astret Codegen::visit(const ASTPlus* ast)
{
	t_astret term1 = ast->GetTerm1()->accept(this);
	std::streampos term1_pos = m_ostr->tellp();
	// placeholder for potential cast
	m_ostr->put(static_cast<t_vm_byte>(OpCode::NOP));

	t_astret term2 = ast->GetTerm2()->accept(this);
	std::streampos term2_pos = m_ostr->tellp();

	t_astret common_type = term1;

	// cast if needed
	auto [first_ty, second_ty, res_ty] = GetCastSymType(term1, term2);
	if(first_ty)
		CastTo(first_ty, term1_pos);
	if(second_ty)
		CastTo(second_ty, term2_pos);
	common_type = res_ty;

	if(ast->IsInverted())  // subtraction
		m_ostr->put(static_cast<t_vm_byte>(OpCode::SUB));
	else                   // addition
		m_ostr->put(static_cast<t_vm_byte>(OpCode::ADD));

	return common_type;
}


t_astret Codegen::visit(const ASTMult* ast)
{
	t_astret term1 = ast->GetTerm1()->accept(this);
	std::streampos term1_pos = m_ostr->tellp();
	// placeholder for potential cast
	m_ostr->put(static_cast<t_vm_byte>(OpCode::NOP));

	t_astret term2 = ast->GetTerm2()->accept(this);
	std::streampos term2_pos = m_ostr->tellp();

	t_astret common_type = term1;

	// cast if needed
	auto [first_ty, second_ty, res_ty] = GetCastSymType(term1, term2);
	if(first_ty)
		CastTo(first_ty, term1_pos);
	if(second_ty)
		CastTo(second_ty, term2_pos);
	common_type = res_ty;

	// division
	if(ast->IsInverted())
	{
		if(term2 && IsArray(term2->ty))
			throw std::runtime_error("ASTMult: Cannot divide by array.");

		m_ostr->put(static_cast<t_vm_byte>(OpCode::DIV));
	}
	// multiplication
	else
	{
		bool mat_mult = false;
		t_vm_int M1_rows = 0, M1_cols = 0;
		t_vm_int M2_rows = 0, M2_cols = 0;

		// matrix-vector multiplication
		if(common_type && IsArray(common_type->ty) &&
			term1->dims.size() == 2 && term2->dims.size() == 1)
		{
			mat_mult = true;
			M1_rows = term1->dims[0];
			M1_cols = term1->dims[1];
			M2_rows = term2->dims[0];
			M2_cols = 1;
		}
		// vector-matrix multiplication
		else if(common_type && IsArray(common_type->ty) &&
			term1->dims.size() == 1 && term2->dims.size() == 2)
		{
			mat_mult = true;
			M1_rows = 1;
			M1_cols = term1->dims[0];
			M2_rows = term2->dims[0];
			M2_cols = term2->dims[1];
		}
		// matrix-matrix multiplication
		else if(common_type && IsArray(common_type->ty) &&
			term1->dims.size() == 2 && term2->dims.size() == 2)
		{
			mat_mult = true;
			M1_rows = term1->dims[0];
			M1_cols = term1->dims[1];
			M2_rows = term2->dims[0];
			M2_cols = term2->dims[1];
		}

		// matrix multiplication
		if(mat_mult)
		{
			// push first matrix sizes
			m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
			m_ostr->put(static_cast<t_vm_byte>(VMType::INT));
			m_ostr->write(reinterpret_cast<const char*>(&M1_rows),
				vm_type_size<VMType::INT, false>);
			m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
			m_ostr->put(static_cast<t_vm_byte>(VMType::INT));
			m_ostr->write(reinterpret_cast<const char*>(&M1_cols),
				vm_type_size<VMType::INT, false>);

			// push second matrix sizes
			m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
			m_ostr->put(static_cast<t_vm_byte>(VMType::INT));
			m_ostr->write(reinterpret_cast<const char*>(&M2_rows),
				vm_type_size<VMType::INT, false>);
			m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
			m_ostr->put(static_cast<t_vm_byte>(VMType::INT));
			m_ostr->write(reinterpret_cast<const char*>(&M2_cols),
				vm_type_size<VMType::INT, false>);

			m_ostr->put(static_cast<t_vm_byte>(OpCode::MATMUL));
		}
		// scalar multiplication
		else
		{
			m_ostr->put(static_cast<t_vm_byte>(OpCode::MUL));
		}
	}

	return common_type;
}


t_astret Codegen::visit(const ASTMod* ast)
{
	t_astret term1 = ast->GetTerm1()->accept(this);
	std::streampos term1_pos = m_ostr->tellp();
	// placeholder for potential cast
	m_ostr->put(static_cast<t_vm_byte>(OpCode::NOP));

	t_astret term2 = ast->GetTerm2()->accept(this);
	std::streampos term2_pos = m_ostr->tellp();

	t_astret common_type = term1;

	// cast if needed
	auto [first_ty, second_ty, res_ty] = GetCastSymType(term1, term2);
	if(first_ty)
		CastTo(first_ty, term1_pos);
	if(second_ty)
		CastTo(second_ty, term2_pos);
	common_type = res_ty;

	m_ostr->put(static_cast<t_vm_byte>(OpCode::MOD));

	return common_type;
}


t_astret Codegen::visit(const ASTPow* ast)
{
	t_astret term1 = ast->GetTerm1()->accept(this);
	std::streampos term1_pos = m_ostr->tellp();
	// placeholder for potential cast
	m_ostr->put(static_cast<t_vm_byte>(OpCode::NOP));

	t_astret term2 = ast->GetTerm2()->accept(this);
	std::streampos term2_pos = m_ostr->tellp();

	t_astret common_type = term1;

	// cast if needed
	auto [first_ty, second_ty, res_ty] = GetCastSymType(term1, term2);
	if(first_ty)
		CastTo(first_ty, term1_pos);
	if(second_ty)
		CastTo(second_ty, term2_pos);
	common_type = res_ty;

	m_ostr->put(static_cast<t_vm_byte>(OpCode::POW));

	return common_type;
}


t_astret Codegen::visit(const ASTNorm* ast)
{
	t_astret term = ast->GetTerm()->accept(this);
	CallExternal("norm");
	return term;
}


t_astret Codegen::visit(const ASTComp* ast)
{
	ast->GetTerm1()->accept(this);
	ast->GetTerm2()->accept(this);

	switch(ast->GetOp())
	{
		case ASTComp::EQU:
			m_ostr->put(static_cast<t_vm_byte>(OpCode::EQU));
			break;
		case ASTComp::NEQ:
			m_ostr->put(static_cast<t_vm_byte>(OpCode::NEQU));
			break;
		case ASTComp::GT:
			m_ostr->put(static_cast<t_vm_byte>(OpCode::GT));
			break;
		case ASTComp::LT:
			m_ostr->put(static_cast<t_vm_byte>(OpCode::LT));
			break;
		case ASTComp::GEQ:
			m_ostr->put(static_cast<t_vm_byte>(OpCode::GEQU));
			break;
		case ASTComp::LEQ:
			m_ostr->put(static_cast<t_vm_byte>(OpCode::LEQU));
			break;
		default:
			throw std::runtime_error("ASTComp: Invalid operation.");
			break;
	}

	return nullptr;
}


t_astret Codegen::visit(const ASTBool* ast)
{
	ast->GetTerm1()->accept(this);
	if(ast->GetTerm2())
		ast->GetTerm2()->accept(this);

	switch(ast->GetOp())
	{
		case ASTBool::XOR:
			m_ostr->put(static_cast<t_vm_byte>(OpCode::XOR));
			break;
		case ASTBool::OR:
			m_ostr->put(static_cast<t_vm_byte>(OpCode::OR));
			break;
		case ASTBool::AND:
			m_ostr->put(static_cast<t_vm_byte>(OpCode::AND));
			break;
		case ASTBool::NOT:
			m_ostr->put(static_cast<t_vm_byte>(OpCode::NOT));
			break;
		default:
			throw std::runtime_error("ASTBool: Invalid operation.");
			break;
	}

	return nullptr;
}
// ----------------------------------------------------------------------------
