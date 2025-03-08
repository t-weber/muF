/**
 * muF grammar -- operators
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date feb-2025
 * @license see 'LICENSE' file
 */

#include "grammar.h"


void Grammar::CreateOperators()
{
	// --------------------------------------------------------------------------------
	// expression
	// --------------------------------------------------------------------------------
	// expression -> ( expression )
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ bracket_open, expression, bracket_close }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		return args[1];
	}));
#endif
	++semanticindex;

/*
// TODO: since there's no statement end symbols like ';', the grammar can't yet distiguish
	unary+/- of a following statement and binary +/- of the current statement
	=> handle newline tokens as statement deliminators in the grammar, not in the lexer
	// unary plus
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ op_plus, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		return args[1];
	}));
#endif
	//std::cout << "Unary+ rule index: " << semanticindex << std::endl;
	++semanticindex;

	// unary minus
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ op_minus, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr = std::dynamic_pointer_cast<AST>(args[1]);
		return std::make_shared<ASTUMinus>(expr);
	}));
#endif
	//std::cout << "Unary- rule index: " << semanticindex << std::endl;
	++semanticindex;
*/

	// norm
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ op_norm, expression, op_norm }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr = std::dynamic_pointer_cast<AST>(args[1]);
		return std::make_shared<ASTNorm>(expr);
	}));
#endif
	++semanticindex;

	// boolean not
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ op_not, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr = std::dynamic_pointer_cast<AST>(args[1]);
		return std::make_shared<ASTBool>(expr, ASTBool::NOT);
	}));
#endif
	++semanticindex;

	// expression -> expression + expression
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression, op_plus, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr1 = std::dynamic_pointer_cast<AST>(args[0]);
		auto expr2 = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTPlus>(expr1, expr2, 0);
	}));
#endif
	++semanticindex;

	// expression -> expression - expression
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression, op_minus, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr1 = std::dynamic_pointer_cast<AST>(args[0]);
		auto expr2 = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTPlus>(expr1, expr2, 1);
	}));
#endif
	++semanticindex;

	// expression -> expression * expression
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression, op_mult, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr1 = std::dynamic_pointer_cast<AST>(args[0]);
		auto expr2 = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTMult>(expr1, expr2, 0);
	}));
#endif
	++semanticindex;

	// expression -> expression / expression
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression, op_div, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr1 = std::dynamic_pointer_cast<AST>(args[0]);
		auto expr2 = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTMult>(expr1, expr2, 1);
	}));
#endif
	++semanticindex;

	// expression -> expression % expression
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression, op_mod, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr1 = std::dynamic_pointer_cast<AST>(args[0]);
		auto expr2 = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTMod>(expr1, expr2);
	}));
#endif
	++semanticindex;

	// expression -> expression ** expression
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression, op_pow, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr1 = std::dynamic_pointer_cast<AST>(args[0]);
		auto expr2 = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTPow>(expr1, expr2);
	}));
#endif
	++semanticindex;

	// expression -> expression AND expression
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression, op_and, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr1 = std::dynamic_pointer_cast<AST>(args[0]);
		auto expr2 = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTBool>(expr1, expr2, ASTBool::AND);
	}));
#endif
	++semanticindex;

	// expression -> expression OR expression
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression, op_or, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr1 = std::dynamic_pointer_cast<AST>(args[0]);
		auto expr2 = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTBool>(expr1, expr2, ASTBool::OR);
	}));
#endif
	++semanticindex;

	// expression -> expression XOR expression
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression, op_xor, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr1 = std::dynamic_pointer_cast<AST>(args[0]);
		auto expr2 = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTBool>(expr1, expr2, ASTBool::XOR);
	}));
#endif
	++semanticindex;

	// expression -> expression == expression
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression, op_equ, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr1 = std::dynamic_pointer_cast<AST>(args[0]);
		auto expr2 = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTComp>(expr1, expr2, ASTComp::EQU);
	}));
#endif
	++semanticindex;

	// expression -> expression != expression
#ifdef CREATE_PRODUCTION_RULES
		expression->AddRule({ expression, op_neq, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr1 = std::dynamic_pointer_cast<AST>(args[0]);
		auto expr2 = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTComp>(expr1, expr2, ASTComp::NEQ);
	}));
#endif
	++semanticindex;

	// expression -> expression > expression
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression, op_gt, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr1 = std::dynamic_pointer_cast<AST>(args[0]);
		auto expr2 = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTComp>(expr1, expr2, ASTComp::GT);
	}));
#endif
	++semanticindex;

	// expression -> expression < expression
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression, op_lt, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr1 = std::dynamic_pointer_cast<AST>(args[0]);
		auto expr2 = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTComp>(expr1, expr2, ASTComp::LT);
	}));
#endif
	++semanticindex;

	// expression -> expression >= expression
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression, op_geq, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr1 = std::dynamic_pointer_cast<AST>(args[0]);
		auto expr2 = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTComp>(expr1, expr2, ASTComp::GEQ);
	}));
#endif
	++semanticindex;

	// expression -> expression <= expression
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression, op_leq, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr1 = std::dynamic_pointer_cast<AST>(args[0]);
		auto expr2 = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTComp>(expr1, expr2, ASTComp::LEQ);
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------
}
