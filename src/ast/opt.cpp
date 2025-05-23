/**
 * syntax tree optimisations
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 9-mar-2025
 * @license see 'LICENSE' file
 */

#include "opt.h"
#include "common/helpers.h"

#include <variant>
#include <utility>



ASTOpt::ASTOpt()
{
}


/**
 * optimise constant expressions
 * returns a replacement node (or nullptr)
 */
ASTPtr ASTOpt::OptConsts(ASTPtr ast)
{
	if(!ast)
		return nullptr;

	// perform constant operations
	auto perform_op = [this]<class t_ast, class t_val>(auto _ast) -> ASTPtr
	{
		if(!_ast)
			return nullptr;

		auto ast = std::dynamic_pointer_cast<t_ast>(_ast);
		auto term1 = std::dynamic_pointer_cast<ASTNumConst<t_val>>(ast->GetTerm1());
		auto term2 = std::dynamic_pointer_cast<ASTNumConst<t_val>>(ast->GetTerm2());
		if(!term1 || !term2)
			return nullptr;

		if constexpr(std::is_same_v<t_ast, ASTPlus>)
		{
			++m_arith_opts;
			if(ast->IsInverted())
				term1->SetVal(term1->GetVal() - term2->GetVal());
			else
				term1->SetVal(term1->GetVal() + term2->GetVal());
		}
		else if constexpr(std::is_same_v<t_ast, ASTMult>)
		{
			++m_arith_opts;
			if(ast->IsInverted())
				term1->SetVal(term1->GetVal() / term2->GetVal());
			else
				term1->SetVal(term1->GetVal() * term2->GetVal());
		}
		else if constexpr(std::is_same_v<t_ast, ASTMod> && std::is_integral_v<t_val>)
		{
			++m_arith_opts;
			term1->SetVal(term1->GetVal() % term2->GetVal());
		}
		else if constexpr(std::is_same_v<t_ast, ASTMod> && std::is_floating_point_v<t_val>)
		{
			++m_arith_opts;
			term1->SetVal(std::fmod(term1->GetVal(), term2->GetVal()));
		}
		else if constexpr(std::is_same_v<t_ast, ASTPow>)
		{
			++m_arith_opts;
			term1->SetVal(power<t_val, t_val>(term1->GetVal(), term2->GetVal()));
		}
		else if constexpr(std::is_same_v<t_ast, ASTComp>)
		{
			++m_logic_opts;
			if(ast->GetOp() == ASTComp::EQU)
				return std::make_shared<ASTNumConst<bool>>(term1->GetVal() == term2->GetVal());
			else if(ast->GetOp() == ASTComp::NEQ)
				return std::make_shared<ASTNumConst<bool>>(term1->GetVal() != term2->GetVal());
			else if(ast->GetOp() == ASTComp::GT)
				return std::make_shared<ASTNumConst<bool>>(term1->GetVal() > term2->GetVal());
			else if(ast->GetOp() == ASTComp::LT)
				return std::make_shared<ASTNumConst<bool>>(term1->GetVal() < term2->GetVal());
			else if(ast->GetOp() == ASTComp::GEQ)
				return std::make_shared<ASTNumConst<bool>>(term1->GetVal() >= term2->GetVal());
			else if(ast->GetOp() == ASTComp::LEQ)
				return std::make_shared<ASTNumConst<bool>>(term1->GetVal() <= term2->GetVal());
			else
				--m_logic_opts;  // unknown logical operation
		}
		else if constexpr(std::is_same_v<t_ast, ASTBool>)
		{
			++m_logic_opts;
			if(ast->GetOp() == ASTBool::AND)
				return std::make_shared<ASTNumConst<bool>>(term1->GetVal() && term2->GetVal());
			else if(ast->GetOp() == ASTBool::OR)
				return std::make_shared<ASTNumConst<bool>>(term1->GetVal() || term2->GetVal());
			else if(ast->GetOp() == ASTBool::XOR)
				return std::make_shared<ASTNumConst<bool>>(term1->GetVal() ^ term2->GetVal());
			else if(ast->GetOp() == ASTBool::NOT)
				return std::make_shared<ASTNumConst<bool>>(!term1->GetVal());
			else
				--m_logic_opts;  // unknown logical operation
		}
		else
		{
			// no replacement found, keep original node
			return nullptr;
		}

		// return replacement node
		return term1;
	};


	// loop perform_op over the given types
	auto perform_op_loop = [perform_op]<class t_ast, class ...t_vals>(ASTPtr ast) -> ASTPtr
	{
		ASTPtr replacement = nullptr;
		auto op_func = [perform_op, &replacement]<class t_val>(ASTPtr ast)
		{
			if(replacement)  // replacement already found?
				return;
			replacement = perform_op.template operator()<t_ast, t_val>(ast);
		};

		( (op_func.template operator()<t_vals>(ast)), ... );
		return replacement;
	};


	if(ast->type() == ASTType::Plus)
	{
		if(ASTPtr replacement = perform_op_loop.template operator()<ASTPlus, t_int, t_real, t_cplx>(ast))
			return replacement;

		// string concatenation
		auto astplus = std::dynamic_pointer_cast<ASTPlus>(ast);
		if(astplus->GetTerm1()->type() == ASTType::StrConst &&
			astplus->GetTerm2()->type() == ASTType::StrConst &&
			!astplus->IsInverted())
		{
			++m_arith_opts;
			auto term1 = std::dynamic_pointer_cast<ASTStrConst>(astplus->GetTerm1());
			auto term2 = std::dynamic_pointer_cast<ASTStrConst>(astplus->GetTerm2());
			term1->SetVal(term1->GetVal() + term2->GetVal());
			return term1;
		}
	}
	else if(ast->type() == ASTType::Mult)
	{
		if(ASTPtr replacement = perform_op_loop.template operator()<ASTMult, t_int, t_real, t_cplx>(ast))
			return replacement;
	}
	else if(ast->type() == ASTType::Mod)
	{
		if(ASTPtr replacement = perform_op_loop.template operator()<ASTMod, t_int, t_real>(ast))
			return replacement;
	}
	else if(ast->type() == ASTType::Pow)
	{
		if(ASTPtr replacement = perform_op_loop.template operator()<ASTPow, t_int, t_real, t_cplx>(ast))
			return replacement;
	}
	else if(ast->type() == ASTType::Comp)
	{
		if(ASTPtr replacement = perform_op_loop.template operator()<ASTComp, t_int, t_real>(ast))
			return replacement;

		// string comparison
		auto astcmp = std::dynamic_pointer_cast<ASTComp>(ast);
		if(astcmp->GetTerm1()->type() == ASTType::StrConst &&
			astcmp->GetTerm2()->type() == ASTType::StrConst &&
			(astcmp->GetOp() == ASTComp::EQU || astcmp->GetOp() == ASTComp::NEQ))
		{
			++m_logic_opts;
			auto term1 = std::dynamic_pointer_cast<ASTStrConst>(astcmp->GetTerm1());
			auto term2 = std::dynamic_pointer_cast<ASTStrConst>(astcmp->GetTerm2());
			if(astcmp->GetOp() == ASTComp::EQU)
				return std::make_shared<ASTNumConst<bool>>(term1->GetVal() == term2->GetVal());
			else
				return std::make_shared<ASTNumConst<bool>>(term1->GetVal() != term2->GetVal());
		}
	}
	else if(ast->type() == ASTType::Bool)
	{
		if(ASTPtr replacement = perform_op_loop.template operator()<ASTBool, t_int>(ast))
			return replacement;
	}

	// keep current node
	return nullptr;
}


t_astret ASTOpt::visit(ASTUMinus* ast)
{
	ast->GetTerm()->accept(this);

	if(ASTPtr newterm = OptConsts(ast->GetTerm()); newterm)
		ast->SetTerm(newterm);

	return nullptr;
}


t_astret ASTOpt::visit(ASTPlus* ast)
{
	ast->GetTerm1()->accept(this);
	ast->GetTerm2()->accept(this);

	if(ASTPtr newterm = OptConsts(ast->GetTerm1()); newterm)
		ast->SetTerm1(newterm);
	if(ASTPtr newterm = OptConsts(ast->GetTerm2()); newterm)
		ast->SetTerm2(newterm);

	return nullptr;
}


t_astret ASTOpt::visit(ASTMult* ast)
{
	ast->GetTerm1()->accept(this);
	ast->GetTerm2()->accept(this);

	if(ASTPtr newterm = OptConsts(ast->GetTerm1()); newterm)
		ast->SetTerm1(newterm);
	if(ASTPtr newterm = OptConsts(ast->GetTerm2()); newterm)
		ast->SetTerm2(newterm);

	return nullptr;
}


t_astret ASTOpt::visit(ASTMod* ast)
{
	ast->GetTerm1()->accept(this);
	ast->GetTerm2()->accept(this);

	if(ASTPtr newterm = OptConsts(ast->GetTerm1()); newterm)
		ast->SetTerm1(newterm);
	if(ASTPtr newterm = OptConsts(ast->GetTerm2()); newterm)
		ast->SetTerm2(newterm);

	return nullptr;
}


t_astret ASTOpt::visit(ASTPow* ast)
{
	ast->GetTerm1()->accept(this);
	ast->GetTerm2()->accept(this);

	if(ASTPtr newterm = OptConsts(ast->GetTerm1()); newterm)
		ast->SetTerm1(newterm);
	if(ASTPtr newterm = OptConsts(ast->GetTerm2()); newterm)
		ast->SetTerm2(newterm);

	return nullptr;
}


t_astret ASTOpt::visit(ASTNorm* ast)
{
	ast->GetTerm()->accept(this);

	if(ASTPtr newterm = OptConsts(ast->GetTerm()); newterm)
		ast->SetTerm(newterm);

	return nullptr;
}


t_astret ASTOpt::visit([[maybe_unused]] ASTVar* ast)
{
	return nullptr;
}


t_astret ASTOpt::visit(ASTCall* ast)
{
	std::list<ASTPtr>& args = ast->GetArgumentList();

	for(auto iter = args.begin(); iter != args.end(); ++iter)
	{
		(*iter)->accept(this);

		if(ASTPtr newterm = OptConsts(*iter); newterm)
			*iter = newterm;
	}

	return nullptr;
}


t_astret ASTOpt::visit(ASTStmts* ast)
{
	for(auto& stmt : ast->GetStatementList())
		stmt->accept(this);

	return nullptr;
}


t_astret ASTOpt::visit(ASTVarDecl* ast)
{
	if(ast->GetAssignment())
		ast->GetAssignment()->accept(this);

	if(ast->GetAssignment() && ast->GetAssignment()->GetExpr())
	{
		if(ASTPtr newterm = OptConsts(ast->GetAssignment()->GetExpr()); newterm)
			ast->GetAssignment()->SetExpr(newterm);
	}

	return nullptr;
}


t_astret ASTOpt::visit(ASTFunc* ast)
{
	ast->GetStatements()->accept(this);

	return nullptr;
}


t_astret ASTOpt::visit(ASTReturn* ast)
{
	if(ast->OnlyJumpToFuncEnd())
		return nullptr;

	auto& retvals = ast->GetRets()->GetList();
	std::size_t numRets = retvals.size();

	if(numRets == 1)
	{
		(*retvals.begin())->accept(this);
	}
	else if(numRets > 1)
	{
		for(auto& elem : retvals)
			elem->accept(this);
	}

	return nullptr;
}


t_astret ASTOpt::visit(ASTAssign* ast)
{
	ast->GetExpr()->accept(this);

	if(ASTPtr newterm = OptConsts(ast->GetExpr()); newterm)
		ast->SetExpr(newterm);

	return nullptr;
}


t_astret ASTOpt::visit(ASTVarRange* ast)
{
	ast->GetBegin()->accept(this);
	ast->GetEnd()->accept(this);
	if(ast->GetInc())
		ast->GetInc()->accept(this);

	return nullptr;
}


t_astret ASTOpt::visit(ASTArrayAccess* ast)
{
	ast->GetNum1()->accept(this);

	if(ast->GetNum2())
		ast->GetNum2()->accept(this);

	ast->GetTerm()->accept(this);

	if(ASTPtr newterm = OptConsts(ast->GetNum1()); newterm)
		ast->SetNum1(newterm);
	if(ASTPtr newterm = OptConsts(ast->GetNum2()); newterm)
		ast->SetNum2(newterm);
	if(ASTPtr newterm = OptConsts(ast->GetTerm()); newterm)
		ast->SetTerm(newterm);

	return nullptr;
}


t_astret ASTOpt::visit(ASTArrayAssign* ast)
{
	ast->GetNum1()->accept(this);

	if(ast->GetNum2())
		ast->GetNum2()->accept(this);

	ast->GetExpr()->accept(this);

	if(ASTPtr newterm = OptConsts(ast->GetNum1()); newterm)
		ast->SetNum1(newterm);
	if(ASTPtr newterm = OptConsts(ast->GetNum2()); newterm)
		ast->SetNum2(newterm);
	if(ASTPtr newterm = OptConsts(ast->GetExpr()); newterm)
		ast->SetExpr(newterm);

	return nullptr;
}


t_astret ASTOpt::visit(ASTComp* ast)
{
	ast->GetTerm1()->accept(this);
	ast->GetTerm2()->accept(this);

	if(ASTPtr newterm = OptConsts(ast->GetTerm1()); newterm)
		ast->SetTerm1(newterm);
	if(ASTPtr newterm = OptConsts(ast->GetTerm2()); newterm)
		ast->SetTerm2(newterm);

	return nullptr;
}


t_astret ASTOpt::visit(ASTBool* ast)
{
	ast->GetTerm1()->accept(this);
	if(ast->GetTerm2())
		ast->GetTerm2()->accept(this);

	if(ASTPtr newterm = OptConsts(ast->GetTerm1()); newterm)
		ast->SetTerm1(newterm);
	if(ASTPtr newterm = OptConsts(ast->GetTerm2()); newterm)
		ast->SetTerm2(newterm);

	return nullptr;
}


t_astret ASTOpt::visit(ASTCond* ast)
{
	ast->GetCond()->accept(this);
	ast->GetIf()->accept(this);
	if(ast->GetElse())
		ast->GetElse()->accept(this);

	if(ASTPtr newterm = OptConsts(ast->GetCond()); newterm)
		ast->SetCond(newterm);

	return nullptr;
}


t_astret ASTOpt::visit(ASTCases* ast)
{
	ast->GetExpr()->accept(this);

	typename ASTCases::t_cases& cases = ast->GetCases();
	for(auto iter = cases.begin(); iter != cases.end(); ++iter)
	{
		iter->first->accept(this);  // condition
		iter->second->accept(this); // statement

		// optimise condition
		if(ASTPtr newcase = OptConsts(iter->first); newcase)
			iter->first = newcase;
	}

	if(ast->GetDefaultCase())
		ast->GetDefaultCase()->accept(this);

	return nullptr;
}


t_astret ASTOpt::visit(ASTLoop* ast)
{
	ast->GetCond()->accept(this);
	ast->GetLoopStmt()->accept(this);

	if(ASTPtr newterm = OptConsts(ast->GetCond()); newterm)
		ast->SetCond(newterm);

	return nullptr;
}


t_astret ASTOpt::visit(ASTRangedLoop* ast)
{
	ast->GetRange()->accept(this);
	ast->GetLoopStmt()->accept(this);

	return nullptr;
}


t_astret ASTOpt::visit([[maybe_unused]] ASTLoopBreak* ast)
{
	return nullptr;
}


t_astret ASTOpt::visit([[maybe_unused]] ASTLoopNext* ast)
{
	return nullptr;
}


t_astret ASTOpt::visit([[maybe_unused]] ASTStrConst* ast)
{
	return nullptr;
}


t_astret ASTOpt::visit(ASTExprList* ast)
{
	std::list<ASTPtr>& exprs = ast->GetList();

	for(auto iter = exprs.begin(); iter != exprs.end(); ++iter)
	{
		(*iter)->accept(this);

		if(ASTPtr newterm = OptConsts(*iter); newterm)
			*iter = newterm;
	}

	return nullptr;
}


t_astret ASTOpt::visit([[maybe_unused]] ASTNumConst<t_real>* ast)
{
	return nullptr;
}


t_astret ASTOpt::visit([[maybe_unused]] ASTNumConst<t_int>* ast)
{
	return nullptr;
}


t_astret ASTOpt::visit([[maybe_unused]] ASTNumConst<t_cplx>* ast)
{
	return nullptr;
}


t_astret ASTOpt::visit([[maybe_unused]] ASTNumConst<t_quat>* ast)
{
	return nullptr;
}


t_astret ASTOpt::visit([[maybe_unused]] ASTNumConst<bool>* ast)
{
	return nullptr;
}


t_astret ASTOpt::visit([[maybe_unused]] ASTNumConstList<t_int>* ast)
{
	return nullptr;
}


t_astret ASTOpt::visit([[maybe_unused]] ASTLabel* ast)
{
	return nullptr;
}


t_astret ASTOpt::visit([[maybe_unused]] ASTJump* ast)
{
	//ast->GetLabel()->accept(this);

	return nullptr;
}
