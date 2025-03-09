/**
 * muF grammar -- loops, conditionals, and jumps
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date feb-2025
 * @license see 'LICENSE' file
 */

#include "grammar.h"


void Grammar::CreateLoops()
{
	// --------------------------------------------------------------------------------
	// statement
	// --------------------------------------------------------------------------------
	// conditional
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_if, expression, keyword_then,
		statements, keyword_end, keyword_if }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto cond = std::dynamic_pointer_cast<AST>(args[1]);
		auto if_stmt = std::dynamic_pointer_cast<AST>(args[3]);
		return std::make_shared<ASTCond>(cond, if_stmt);
	}));
#endif
	++semanticindex;

	// conditional
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_if, expression, keyword_then,
		statements, keyword_else, statements, keyword_end, keyword_if }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto cond = std::dynamic_pointer_cast<AST>(args[1]);
		auto if_stmt = std::dynamic_pointer_cast<AST>(args[3]);
		auto else_stmt = std::dynamic_pointer_cast<AST>(args[5]);
		return std::make_shared<ASTCond>(cond, if_stmt, else_stmt);
	}));
#endif
	++semanticindex;

	// select case
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_select, keyword_case,
		expression /*2*/, cases /*3*/,
		keyword_end, keyword_select }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr = std::dynamic_pointer_cast<AST>(args[2]);
		auto cases = std::dynamic_pointer_cast<ASTCases>(args[3]);
		cases->SetExpr(expr);
		return cases;
	}));
#endif
	++semanticindex;

	// do while loop
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_do, keyword_while,
		bracket_open, expression, bracket_close,
		statements, keyword_end, keyword_do }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto cond = std::dynamic_pointer_cast<AST>(args[3]);
		auto stmt = std::dynamic_pointer_cast<AST>(args[5]);
		return std::make_shared<ASTLoop>(cond, stmt);
	}));
#endif
	++semanticindex;

	// ranged do loop
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_do, var_range, statements, keyword_end, keyword_do }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		auto range = std::dynamic_pointer_cast<ASTVarRange>(args[1]);
		auto stmt = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTRangedLoop>(range, stmt);
	}));
#endif
	++semanticindex;

	// break current loop
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_break/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, [[maybe_unused]] const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		return std::make_shared<ASTLoopBreak>();
	}));
#endif
	++semanticindex;

	// break multiple loops
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_break, sym_int/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		auto num_node = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[1]);
		const t_int num = num_node->GetVal();

		return std::make_shared<ASTLoopBreak>(num);
	}));
#endif
	++semanticindex;

	// continue current loop
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_next/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, [[maybe_unused]] const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		return std::make_shared<ASTLoopNext>();
	}));
#endif
	++semanticindex;

	// continue multiple loops
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_next, sym_int/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		auto num_node = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[1]);
		const t_int num = num_node->GetVal();

		return std::make_shared<ASTLoopNext>(num);
	}));
#endif
	++semanticindex;

	// statement -> label
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ label }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto varident = std::dynamic_pointer_cast<ASTStrConst>(args[0]);
		const t_str& ident = varident->GetVal();

		return std::make_shared<ASTLabel>(ident);
	}));
#endif
	++semanticindex;

	// statement -> goto label
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_goto, label }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto varident = std::dynamic_pointer_cast<ASTStrConst>(args[1]);
		const t_str& ident = varident->GetVal();

		return std::make_shared<ASTJump>(ident);
	}));
#endif
	++semanticindex;

	// statement -> comefrom label
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_comefrom, label }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto varident = std::dynamic_pointer_cast<ASTStrConst>(args[1]);
		const t_str& ident = varident->GetVal();

		return std::make_shared<ASTJump>(ident, true);
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// cases
	// --------------------------------------------------------------------------------
	// cases for select
#ifdef CREATE_PRODUCTION_RULES
	cases->AddRule({ keyword_case, bracket_open, expression /*2*/, bracket_close,
		statements /*4*/, cases /*5*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto cond = std::dynamic_pointer_cast<AST>(args[2]);
		auto stmts = std::dynamic_pointer_cast<AST>(args[4]);
		auto cases = std::dynamic_pointer_cast<ASTCases>(args[5]);
		cases->AddCase(cond, stmts);
		return cases;
	}));
#endif
	++semanticindex;

	// a single case
#ifdef CREATE_PRODUCTION_RULES
	cases->AddRule({ keyword_case, bracket_open, expression /*2*/, bracket_close,
		statements /*4*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto cond = std::dynamic_pointer_cast<AST>(args[2]);
		auto stmts = std::dynamic_pointer_cast<AST>(args[4]);
		auto cases = std::make_shared<ASTCases>();
		cases->AddCase(cond, stmts);
		return cases;
	}));
#endif
	++semanticindex;

	// default case (has to be last in the cases list)
#ifdef CREATE_PRODUCTION_RULES
	cases->AddRule({ keyword_case, keyword_default, statements }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto stmts = std::dynamic_pointer_cast<AST>(args[2]);
		auto cases = std::make_shared<ASTCases>();
		cases->SetDefaultCase(stmts);
		return cases;
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// variable range
	// --------------------------------------------------------------------------------
	// variable range (for do loops)
#ifdef CREATE_PRODUCTION_RULES
	var_range->AddRule({ ident, op_assign, expression, comma, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto identnode = std::dynamic_pointer_cast<ASTStrConst>(args[0]);
		const t_str& ident = identnode->GetVal();

		auto begin = std::dynamic_pointer_cast<AST>(args[2]);
		auto end = std::dynamic_pointer_cast<AST>(args[4]);

		return std::make_shared<ASTVarRange>(ident, begin, end);
	}));
#endif
	++semanticindex;

	// variable range with increment (for loops)
#ifdef CREATE_PRODUCTION_RULES
	var_range->AddRule({ ident, op_assign, expression, comma, expression, comma, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto identnode = std::dynamic_pointer_cast<ASTStrConst>(args[0]);
		const t_str& ident = identnode->GetVal();

		auto begin = std::dynamic_pointer_cast<AST>(args[2]);
		auto end = std::dynamic_pointer_cast<AST>(args[4]);
		auto inc = std::dynamic_pointer_cast<AST>(args[6]);

		return std::make_shared<ASTVarRange>(ident, begin, end, inc);
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------
}
