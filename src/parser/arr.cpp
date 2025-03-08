/**
 * muF grammar -- array access
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date feb-2025
 * @license see 'LICENSE' file
 */

#include "grammar.h"

#define DEFAULT_STRING_SIZE 128


void Grammar::CreateArrays()
{
	// --------------------------------------------------------------------------------
	// expression
	// --------------------------------------------------------------------------------
        // array
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ array_begin, expressions, array_end }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto exprs = std::dynamic_pointer_cast<ASTExprList>(args[1]);

		// set array symbol type from context (if available)
		SymbolType ctx_sym_ty = m_context.GetSymType();
		if(ctx_sym_ty == SymbolType::REAL_ARRAY ||
			ctx_sym_ty == SymbolType::INT_ARRAY ||
			ctx_sym_ty == SymbolType::CPLX_ARRAY)
			exprs->SetArrayType(ctx_sym_ty);
		else
			exprs->SetArrayType(SymbolType::REAL_ARRAY);

		return exprs;
	}));
#endif
	++semanticindex;

	// array access and assignment
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression /*0*/, array_begin,
		expressions /*2*/, array_end,
		opt_assign /*4*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto term = std::dynamic_pointer_cast<AST>(args[0]);
		auto indices = std::dynamic_pointer_cast<AST>(args[2]);

		if(!args[4])
		{
			// array access into an array expression
			return std::make_shared<ASTArrayAccess>(term, indices);
		}
		else
		{
			// assignment of a array element
			if(term->type() != ASTType::Var)
			{
				std::cerr << "Can only assign to an l-value symbol." << std::endl;
				return nullptr;
			}
			else
			{
				auto opt_term = std::dynamic_pointer_cast<AST>(args[4]);
				auto var = std::static_pointer_cast<ASTVar>(term);
				return std::make_shared<ASTArrayAssign>(
					var->GetIdent(), opt_term, indices);
			}
		}

		return nullptr;
	}));
#endif
	++semanticindex;

	// array ranged access and assignment
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression /*0*/, array_begin,
		expression /*2*/, range, expression /*4*/, array_end,
		opt_assign /*6*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto term = std::dynamic_pointer_cast<AST>(args[0]);
		auto idx1 = std::dynamic_pointer_cast<AST>(args[2]);
		auto idx2 = std::dynamic_pointer_cast<AST>(args[4]);

		if(!args[6])
		{
			// array access into an array expression
			return std::make_shared<ASTArrayAccess>(term, idx1, idx2, true);
		}
		else
		{
			// assignment of a array element
			if(term->type() != ASTType::Var)
			{
				std::cerr << "Can only assign to an l-value symbol." << std::endl;
				return nullptr;
			}
			else
			{
				auto opt_term = std::dynamic_pointer_cast<AST>(args[6]);
				auto var = std::static_pointer_cast<ASTVar>(term);
				return std::make_shared<ASTArrayAssign>(
					var->GetIdent(), opt_term, idx1, idx2, true);
			}
		}

		return nullptr;
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------
}
