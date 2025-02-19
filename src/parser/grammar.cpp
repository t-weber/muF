/**
 * muF grammar
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date feb-2025
 * @license see 'LICENSE' file
 */

#include "grammar.h"
#include "lexer.h"

#define DEFAULT_STRING_SIZE 128


void Grammar::CreateGrammar()
{
#ifdef CREATE_PRODUCTION_RULES
	// non-terminals
	start = std::make_shared<lalr1::NonTerminal>(START, "start");
	expression = std::make_shared<lalr1::NonTerminal>(EXPRESSION, "expression");
	expressions = std::make_shared<lalr1::NonTerminal>(EXPRESSIONS, "expressions");
	statement = std::make_shared<lalr1::NonTerminal>(STATEMENT, "statement");
	statements = std::make_shared<lalr1::NonTerminal>(STATEMENTS, "statements");
	variables = std::make_shared<lalr1::NonTerminal>(VARIABLES, "variables");
	full_argumentlist = std::make_shared<lalr1::NonTerminal>(FULL_ARGUMENTLIST, "full_argumentlist");
	argumentlist = std::make_shared<lalr1::NonTerminal>(ARGUMENTLIST, "argumentlist");
	identlist = std::make_shared<lalr1::NonTerminal>(IDENTLIST, "identlist");
	typelist = std::make_shared<lalr1::NonTerminal>(TYPELIST, "typelist");
	function = std::make_shared<lalr1::NonTerminal>(FUNCTION, "function");
	typedecl = std::make_shared<lalr1::NonTerminal>(TYPEDECL, "typedecl");
	opt_assign = std::make_shared<lalr1::NonTerminal>(OPT_ASSIGN, "opt_assign");
	var_range = std::make_shared<lalr1::NonTerminal>(VAR_RANGE, "var_range");

	// terminals
	op_assign = std::make_shared<lalr1::Terminal>('=', "=");
	op_plus = std::make_shared<lalr1::Terminal>('+', "+");
	op_minus = std::make_shared<lalr1::Terminal>('-', "-");
	op_mult = std::make_shared<lalr1::Terminal>('*', "*");
	op_div = std::make_shared<lalr1::Terminal>('/', "/");
	op_mod = std::make_shared<lalr1::Terminal>('%', "%");
	op_norm = std::make_shared<lalr1::Terminal>('|', "|");
	op_pow = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::POW), "**");

	op_equ = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::EQU), "==");
	op_neq = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::NEQ), "!=");
	op_geq = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::GEQ), ">=");
	op_leq = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::LEQ), "<=");
	op_and = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::AND), "and");
	op_or = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::OR), "or");
	op_xor = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::XOR), "xor");
	op_not = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::NOT), "not");
	op_gt = std::make_shared<lalr1::Terminal>('>', ">");
	op_lt = std::make_shared<lalr1::Terminal>('<', "<");

	bracket_open = std::make_shared<lalr1::Terminal>('(', "(");
	bracket_close = std::make_shared<lalr1::Terminal>(')', ")");
	array_begin = std::make_shared<lalr1::Terminal>('[', "[");
	array_end = std::make_shared<lalr1::Terminal>(']', "]");
	range = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::RANGE), "~");

	comma = std::make_shared<lalr1::Terminal>(',', ",");
	//stmt_end = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::ENDSTMT), "stmt_end");

	sym_real = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::REAL), "real");
	sym_int = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::INT), "integer");
	sym_bool = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::BOOL), "bool");
	sym_str = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::STR), "string");
	ident = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::IDENT), "ident");
	label = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::LABEL), "label");

	type_sep = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::TYPESEP), "::");
	real_decl = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::REALDECL), "real_decl");
	cplx_decl = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::CPLXDECL), "cplx_decl");
	int_decl = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::INTDECL), "integer_decl");
	arr_decl = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::ARRDECL), "array_decl");
	str_decl = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::STRINGDECL), "string_decl");

	keyword_if = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::IF), "if");
	keyword_then = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::THEN), "then");
	keyword_else = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::ELSE), "else");
	keyword_while = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::WHILE), "while");
	keyword_do = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::DO), "do");
	keyword_func = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::FUNC), "function");
	keyword_procedure = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::PROC), "procedure");
	keyword_program = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::PROGRAM), "program");
	keyword_ret = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::RET), "return");
	keyword_result = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::RESULT), "result");
	keyword_results = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::RESULTS), "results");
	keyword_next = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::NEXT), "next");
	keyword_end = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::END), "end");
	keyword_break = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::BREAK), "break");
	keyword_assign = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::ASSIGN), "assign");
	keyword_goto = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::GOTO), "goto");
	keyword_comefrom = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::COMEFROM), "comefrom");

	// for the if/else s/r conflict shift "else"
	// see: https://www.gnu.org/software/bison/manual/html_node/Non-Operators.html
	keyword_if->SetPrecedence(100, 'l');
	keyword_then->SetPrecedence(100, 'l');
	keyword_else->SetPrecedence(110, 'l');
	ident->SetPrecedence(120, 'l');
	keyword_func->SetPrecedence(0, 'l');

	// s/r conflict because of missing statement end tokens
	//keyword_ret->SetPrecedence(200, 'l');
	keyword_break->SetPrecedence(200, 'l');
	keyword_next->SetPrecedence(200, 'l');
	sym_int->SetPrecedence(210, 'l');
	sym_bool->SetPrecedence(210, 'l');

	// operator precedences and associativities
	// see: https://en.wikipedia.org/wiki/Order_of_operations
	comma->SetPrecedence(305, 'l');
	op_assign->SetPrecedence(310, 'r');

	op_xor->SetPrecedence(320, 'l');
	op_or->SetPrecedence(321, 'l');
	op_and->SetPrecedence(322, 'l');

	op_lt->SetPrecedence(330, 'l');
	op_gt->SetPrecedence(330, 'l');

	op_geq->SetPrecedence(330, 'l');
	op_leq->SetPrecedence(330, 'l');

	op_equ->SetPrecedence(340, 'l');
	op_neq->SetPrecedence(340, 'l');

	op_plus->SetPrecedence(350, 'l');
	op_minus->SetPrecedence(350, 'l');

	op_mult->SetPrecedence(360, 'l');
	op_div->SetPrecedence(360, 'l');
	op_mod->SetPrecedence(360, 'l');

	op_pow->SetPrecedence(370, 'r');

	op_not->SetPrecedence(380, 'r');
	// TODO: unary_ops->SetPrecedence(375, 'r');

	bracket_open->SetPrecedence(390, 'l');
	array_begin->SetPrecedence(390, 'l');
	op_norm->SetPrecedence(390, 'l');
#endif

	// rule semantic id number
	lalr1::t_semantic_id semanticindex = 0;

	// --------------------------------------------------------------------------------
	// start
	// --------------------------------------------------------------------------------
	// start -> statements
#ifdef CREATE_PRODUCTION_RULES
	start->AddRule({ statements }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		auto stmts = std::dynamic_pointer_cast<ASTStmts>(args[0]);
		m_context.SetStatements(stmts);
		return stmts;
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// statements
	// --------------------------------------------------------------------------------
	// list of statements: statements -> statement statements
#ifdef CREATE_PRODUCTION_RULES
	statements->AddRule({ statement, statements }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		auto stmt = std::dynamic_pointer_cast<AST>(args[0]);
		auto stmts = std::dynamic_pointer_cast<ASTStmts>(args[1]);
		stmts->AddStatement(stmt);
		return stmts;
	}));
#endif
	++semanticindex;

	// statements -> epsilon
#ifdef CREATE_PRODUCTION_RULES
	statements->AddRule({ lalr1::g_eps }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, [[maybe_unused]] const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		return std::make_shared<ASTStmts>();
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// variables
	// --------------------------------------------------------------------------------
	// several variables
#ifdef CREATE_PRODUCTION_RULES
	variables->AddRule({ ident, comma, variables }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto varident = std::dynamic_pointer_cast<ASTStrConst>(args[0]);
		const t_str& name = varident->GetVal();
		t_str symName = m_context.AddScopedSymbol(name)->scoped_name;

		auto lst = std::dynamic_pointer_cast<ASTVarDecl>(args[2]);
		lst->AddVariable(symName);
		return lst;
	}));
#endif
	++semanticindex;

	// a single variable
#ifdef CREATE_PRODUCTION_RULES
	variables->AddRule({ ident }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto ident = std::dynamic_pointer_cast<ASTStrConst>(args[0]);
		const t_str& name = ident->GetVal();
		t_str symName = m_context.AddScopedSymbol(name)->scoped_name;

		auto lst = std::make_shared<ASTVarDecl>();
		lst->AddVariable(symName);
		return lst;
	}));
#endif
	++semanticindex;

	// a variable with an assignment
#ifdef CREATE_PRODUCTION_RULES
	variables->AddRule({ ident, op_assign, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto ident = std::dynamic_pointer_cast<ASTStrConst>(args[0]);
		const t_str& name = ident->GetVal();
		t_str symName = m_context.AddScopedSymbol(name)->scoped_name;

		auto term = std::dynamic_pointer_cast<AST>(args[2]);

		auto lst = std::make_shared<ASTVarDecl>(std::make_shared<ASTAssign>(name, term));
		lst->AddVariable(symName);
		return lst;
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// statement
	// --------------------------------------------------------------------------------
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

	// statement -> expression
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ expression/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		return args[0];
	}));
#endif
	++semanticindex;

	// statement -> program ident statements end program
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_program, ident, statements, keyword_end, keyword_program }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[/*this*/](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		auto stmts = std::dynamic_pointer_cast<ASTStmts>(args[2]);
		//m_context.SetStatements(stmts);
		return stmts;
	}));
#endif
	++semanticindex;

	// statement -> function
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ function }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		return args[0];
	}));
#endif
	++semanticindex;

	// (multi-)return
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_ret, expressions/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		auto terms = std::dynamic_pointer_cast<ASTExprList>(args[1]);
		return std::make_shared<ASTReturn>(terms);
	}));
#endif
	++semanticindex;

	// real declaration
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ real_decl, type_sep, variables/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 1)
			m_context.SetSymType(SymbolType::REAL);

		if(!full_match)
			return nullptr;
		return args[2];
	}));
#endif
	++semanticindex;

	// complex declaration
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ cplx_decl, type_sep, variables/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 1)
			m_context.SetSymType(SymbolType::CPLX);

		if(!full_match)
			return nullptr;
		return args[2];
	}));
#endif
	++semanticindex;

	// array declaration
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ arr_decl, type_sep, sym_int, variables/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 3)
		{
			auto dim_node = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[2]);
			const t_int dim = dim_node->GetVal();

			m_context.SetSymType(SymbolType::VECTOR);
			m_context.SetSymDims(std::size_t(dim));
		}

		if(!full_match)
			return nullptr;
		return args[3];
	}));
#endif
	++semanticindex;

	// string declaration with default size
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ str_decl, type_sep, variables/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 2)
		{
			m_context.SetSymType(SymbolType::STRING);
			m_context.SetSymDims(std::size_t(DEFAULT_STRING_SIZE));
		}

		if(!full_match)
			return nullptr;
		return args[2];
	}));
#endif
	++semanticindex;

	// string declaration with given static size
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ str_decl, type_sep, sym_int, variables/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 3)
		{
			auto dim_node = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[2]);
			const t_int dim = dim_node->GetVal();

			m_context.SetSymType(SymbolType::STRING);
			m_context.SetSymDims(std::size_t(dim));
		}

		if(!full_match)
			return nullptr;
		return args[3];
	}));
#endif
	++semanticindex;

	// int declaration
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ int_decl, type_sep, variables/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == /*1*/ 0)  // check
			m_context.SetSymType(SymbolType::INT);

		if(!full_match)
			return nullptr;
		return args[2];
	}));
#endif
	++semanticindex;

	// conditional
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_if, expression, keyword_then, statement, keyword_end, keyword_if }, semanticindex);
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
	statement->AddRule({ keyword_if, expression, keyword_then, statement, keyword_else, statement, keyword_end, keyword_if }, semanticindex);
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

	// do while loop
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_do, keyword_while, bracket_open, expression, bracket_close, statements, keyword_end, keyword_do }, semanticindex);
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
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// typedecl
	// --------------------------------------------------------------------------------
	// real declaration
#ifdef CREATE_PRODUCTION_RULES
	typedecl->AddRule({ real_decl }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, [[maybe_unused]] const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		return std::make_shared<ASTTypeDecl>(SymbolType::REAL);
	}));
#endif
	++semanticindex;

	// complex declaration
#ifdef CREATE_PRODUCTION_RULES
	typedecl->AddRule({ cplx_decl }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, [[maybe_unused]] const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		return std::make_shared<ASTTypeDecl>(SymbolType::CPLX);
	}));
#endif
	++semanticindex;

	// array declaration
#ifdef CREATE_PRODUCTION_RULES
	typedecl->AddRule({ arr_decl, sym_int }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto dim_node = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[1]);
		const t_int dim = dim_node->GetVal();
		return std::make_shared<ASTTypeDecl>(SymbolType::VECTOR,
			std::vector<std::size_t>{ std::size_t(dim) });
	}));
#endif
	++semanticindex;

	// string declaration with default size
#ifdef CREATE_PRODUCTION_RULES
	typedecl->AddRule({ str_decl }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, [[maybe_unused]] const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		return std::make_shared<ASTTypeDecl>(SymbolType::STRING,
			std::vector<std::size_t>{ DEFAULT_STRING_SIZE });
	}));
#endif
	++semanticindex;

	// string declaration with given static size
#ifdef CREATE_PRODUCTION_RULES
	typedecl->AddRule({ str_decl, sym_int }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto dim_node = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[1]);
		const t_int dim = dim_node->GetVal();
		return std::make_shared<ASTTypeDecl>(SymbolType::STRING,
			std::vector<std::size_t>{ std::size_t(dim) });
	}));
#endif
	++semanticindex;

	// int declaration
#ifdef CREATE_PRODUCTION_RULES
	typedecl->AddRule({ int_decl }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, [[maybe_unused]] const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		return std::make_shared<ASTTypeDecl>(SymbolType::INT);
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// opt_assign
	// --------------------------------------------------------------------------------
#ifdef CREATE_PRODUCTION_RULES
	opt_assign->AddRule({ op_assign, expression }, semanticindex);
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

	// opt_assign -> eps
#ifdef CREATE_PRODUCTION_RULES
	opt_assign->AddRule({ lalr1::g_eps }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[]([[maybe_unused]] bool full_match, [[maybe_unused]] const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		return nullptr;
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------


	// --------------------------------------------------------------------------------
	// full_argumentlist
	// --------------------------------------------------------------------------------
#ifdef CREATE_PRODUCTION_RULES
	full_argumentlist->AddRule({ argumentlist }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		return args[0];
	}));
#endif
	++semanticindex;

	// full_argumentlist -> eps
#ifdef CREATE_PRODUCTION_RULES
	full_argumentlist->AddRule({ lalr1::g_eps }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, [[maybe_unused]] const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		return std::make_shared<ASTArgNames>();
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// argumentlist
	// --------------------------------------------------------------------------------
#ifdef CREATE_PRODUCTION_RULES
	argumentlist->AddRule({ typedecl, type_sep, ident, comma, argumentlist }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto ty = std::dynamic_pointer_cast<ASTTypeDecl>(args[0]);
		auto argname = std::dynamic_pointer_cast<ASTStrConst>(args[2]);
		auto arglist = std::dynamic_pointer_cast<ASTArgNames>(args[4]);

		arglist->AddArg(argname->GetVal(), ty->GetType(), ty->GetDims());
		return arglist;
	}));
#endif
	++semanticindex;

#ifdef CREATE_PRODUCTION_RULES
	argumentlist->AddRule({ typedecl, type_sep, ident }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto ty = std::dynamic_pointer_cast<ASTTypeDecl>(args[0]);
		auto argname = std::dynamic_pointer_cast<ASTStrConst>(args[2]);

		auto arglist = std::make_shared<ASTArgNames>();
		arglist->AddArg(argname->GetVal(), ty->GetType(), ty->GetDims());
		return arglist;
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// identlist
	// --------------------------------------------------------------------------------
#ifdef CREATE_PRODUCTION_RULES
	identlist->AddRule({ ident, comma, identlist }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto argname = std::dynamic_pointer_cast<ASTStrConst>(args[0]);
		auto idents = std::dynamic_pointer_cast<ASTArgNames>(args[2]);

		idents->AddArg(argname->GetVal());
		return idents;
	}));
#endif
	++semanticindex;

#ifdef CREATE_PRODUCTION_RULES
	identlist->AddRule({ ident }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto argname = std::dynamic_pointer_cast<ASTStrConst>(args[0]);
		auto idents = std::make_shared<ASTArgNames>();

		idents->AddArg(argname->GetVal());
		return idents;
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// typelist
	// --------------------------------------------------------------------------------
#ifdef CREATE_PRODUCTION_RULES
	typelist->AddRule({ typedecl, comma, typelist }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto ty = std::dynamic_pointer_cast<ASTTypeDecl>(args[0]);
		auto arglist = std::dynamic_pointer_cast<ASTArgNames>(args[2]);

		arglist->AddArg("return", ty->GetType(), ty->GetDims());
		return arglist;
	}));
#endif
	++semanticindex;

#ifdef CREATE_PRODUCTION_RULES
	typelist->AddRule({ typedecl }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto ty = std::dynamic_pointer_cast<ASTTypeDecl>(args[0]);

		auto arglist = std::make_shared<ASTArgNames>();
		arglist->AddArg("return", ty->GetType(), ty->GetDims());
		return arglist;
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// expressions
	// --------------------------------------------------------------------------------
#ifdef CREATE_PRODUCTION_RULES
	expressions->AddRule({ expression, comma, expressions }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr = std::dynamic_pointer_cast<AST>(args[0]);
		auto exprs = std::dynamic_pointer_cast<ASTExprList>(args[2]);
		exprs->AddExpr(expr);
		return exprs;
	}));
#endif
	++semanticindex;

#ifdef CREATE_PRODUCTION_RULES
	expressions->AddRule({ expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr = std::dynamic_pointer_cast<AST>(args[0]);
		auto exprs = std::make_shared<ASTExprList>();
		exprs->AddExpr(expr);
		return exprs;
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

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

	// expression -> real
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ sym_real }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto num_node = std::dynamic_pointer_cast<ASTNumConst<t_real>>(args[0]);
		const t_real num = num_node->GetVal();
		return std::make_shared<ASTNumConst<t_real>>(num);
	}));
#endif
	++semanticindex;

	// expression -> int
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ sym_int }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto num_node = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[0]);
		const t_int num = num_node->GetVal();
		return std::make_shared<ASTNumConst<t_int>>(num);
	}));
#endif
	++semanticindex;

	// expression -> ( real, real ) [complex constant]
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ bracket_open, sym_real, comma, sym_real, bracket_close }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto real_node = std::dynamic_pointer_cast<ASTNumConst<t_real>>(args[1]);
		auto imag_node = std::dynamic_pointer_cast<ASTNumConst<t_real>>(args[3]);
		const t_cplx num{real_node->GetVal(), imag_node->GetVal()};
		return std::make_shared<ASTNumConst<t_cplx>>(num);
	}));
#endif
	++semanticindex;

	// expression -> bool
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ sym_bool }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		auto num_node = std::dynamic_pointer_cast<ASTNumConst<bool>>(args[0]);
		const bool val = num_node->GetVal();
		return std::make_shared<ASTNumConst<bool>>(val);
	}));
#endif
	++semanticindex;

	// expression -> string
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ sym_str }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto str_node = std::dynamic_pointer_cast<ASTStrConst>(args[0]);
		const t_str& str = str_node->GetVal();
		return std::make_shared<ASTStrConst>(str);
	}));
#endif
	++semanticindex;

	// real array
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ array_begin, expressions, array_end }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto exprs = std::dynamic_pointer_cast<ASTExprList>(args[1]);
		exprs->SetScalarArray(true);
		return exprs;
	}));
#endif
	++semanticindex;

	// variable
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ ident }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto identnode = std::dynamic_pointer_cast<ASTStrConst>(args[0]);
		const t_str& identstr = identnode->GetVal();

		// does the identifier name a constant?
		auto pair = m_context.GetConst(identstr);
		if(std::get<0>(pair))
		{
			auto variant = std::get<1>(pair);
			if(std::holds_alternative<t_real>(variant))
				return std::make_shared<ASTNumConst<t_real>>(std::get<t_real>(variant));
			else if(std::holds_alternative<t_int>(variant))
				return std::make_shared<ASTNumConst<t_int>>(std::get<t_int>(variant));
			else if(std::holds_alternative<t_str>(variant))
				return std::make_shared<ASTStrConst>(std::get<t_str>(variant));
		}

		// identifier names a variable
		else
		{
			// try finding local symbol
			const Symbol* sym = m_context.FindScopedSymbol(identstr);

			// try finding global symbol
			if(!sym)
				sym = m_context.FindGlobalSymbol(identstr);

			if(sym)
				++sym->refcnt;
			else
				std::cerr << "Cannot find symbol \"" << identstr << "\"." << std::endl;

			return std::make_shared<ASTVar>(identstr);
		}

		return nullptr;
	}));
#endif
	++semanticindex;

	// array access and assignment
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression /*0*/, array_begin,
		expression /*2*/, array_end,
		opt_assign /*4*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto term = std::dynamic_pointer_cast<AST>(args[0]);
		auto idx = std::dynamic_pointer_cast<AST>(args[2]);

		if(!args[4])
		{
			// array access into an array expression
			return std::make_shared<ASTArrayAccess>(term, idx);
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
					var->GetIdent(), opt_term, idx);
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
			return std::make_shared<ASTArrayAccess>(
				term, idx1, idx2, nullptr, nullptr, true);
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
					var->GetIdent(), opt_term,
					idx1, idx2, nullptr, nullptr, true);
			}
		}

		return nullptr;
	}));
#endif
	++semanticindex;

	// function call without arguments
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ ident, bracket_open, bracket_close }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto identnode = std::dynamic_pointer_cast<ASTStrConst>(args[0]);
		const t_str& funcname = identnode->GetVal();
		const Symbol* sym = m_context.GetSymbols().FindSymbol(funcname);

		if(sym && sym->ty == SymbolType::FUNC)
		{
			++sym->refcnt;
		}
		else
		{
			// TODO: move this check into semantics.cpp, as only the functions
			// that have already been parsed are registered at this point
			std::cerr << "Cannot find function \"" << funcname << "\"." << std::endl;
		}

		return std::make_shared<ASTCall>(funcname);
	}));
#endif
	++semanticindex;

	// function call with arguments
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ ident, bracket_open, expressions, bracket_close }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto identnode = std::dynamic_pointer_cast<ASTStrConst>(args[0]);
		const t_str& funcname = identnode->GetVal();
		const Symbol* sym = m_context.GetSymbols().FindSymbol(funcname);

		if(sym && sym->ty == SymbolType::FUNC)
		{
			++sym->refcnt;
		}
		else
		{
			// TODO: move this check into semantics.cpp, as only the functions
			// that have already been parsed are registered at this point
			std::cerr << "Cannot find function \"" << funcname << "\"." << std::endl;
		}

		auto funcargs = std::dynamic_pointer_cast<ASTExprList>(args[2]);
		return std::make_shared<ASTCall>(funcname, funcargs);
	}));
#endif
	++semanticindex;

	// assignment
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ ident, op_assign, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto identnode = std::dynamic_pointer_cast<ASTStrConst>(args[0]);
		const t_str& ident = identnode->GetVal();

		auto term = std::dynamic_pointer_cast<AST>(args[2]);
		return std::make_shared<ASTAssign>(ident, term);
	}));
#endif
	++semanticindex;

	// multi-assignment
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ keyword_assign, identlist, op_assign, expression }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto idents = std::dynamic_pointer_cast<ASTArgNames>(args[1]);
		auto term = std::dynamic_pointer_cast<AST>(args[3]);
		return std::make_shared<ASTAssign>(idents->GetArgIdents(), term);
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// function
	// --------------------------------------------------------------------------------
	// function with a single return value
#ifdef CREATE_PRODUCTION_RULES
	function->AddRule({ keyword_func, ident /*1*/,
		bracket_open, full_argumentlist /*3*/, bracket_close,
		keyword_result, bracket_open, typedecl /*7*/, bracket_close,
		statements /*9*/, keyword_end, keyword_func }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		//std::cout << args.size() << std::endl;
		if(args.size() == 5)  // keyword_result is the first unique partial match
		{
			auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[1]);
			m_context.EnterScope(funcname->GetVal());
			//std::cout << "Entering function \"" << funcname->GetVal() << "\" scope." << std::endl;
		}
		else if(args.size() == 8)
		{
			auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[1]);
			auto funcargs = std::dynamic_pointer_cast<ASTArgNames>(args[3]);
			auto rettype = std::dynamic_pointer_cast<ASTTypeDecl>(args[7]);

			// register argument variables
			std::size_t argidx = 0;
			for(const auto& arg : funcargs->GetArgs())
			{
				Symbol* sym = m_context.AddScopedSymbol(std::get<0>(arg));
				sym->ty = std::get<1>(arg);
				sym->is_arg = true;
				sym->argidx = argidx;
				sym->dims.resize(2);
				sym->dims = std::get<2>(arg);
				++argidx;
			}

			// register the function in the symbol map
			m_context.GetSymbols().AddFunc(
				m_context.GetScopeName(1), funcname->GetVal(),
				rettype->GetType(), funcargs->GetArgTypes(), &rettype->GetDims());
		}

		if(!full_match)
			return nullptr;

		auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[1]);
		auto funcargs = std::dynamic_pointer_cast<ASTArgNames>(args[3]);
		auto rettype = std::dynamic_pointer_cast<ASTTypeDecl>(args[7]);
		auto funcblock = std::dynamic_pointer_cast<ASTStmts>(args[9]);

		auto res = std::make_shared<ASTFunc>(
			funcname->GetVal(), rettype, funcargs, funcblock);
		m_context.LeaveScope(funcname->GetVal());
		return res;
	}));
#endif
	++semanticindex;

	// procedure with no return value
#ifdef CREATE_PRODUCTION_RULES
	function->AddRule({ keyword_procedure, ident /*1*/,
		bracket_open, full_argumentlist /*3*/, bracket_close,
		statements /*5*/, keyword_end, keyword_func }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 2)
		{
			auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[1]);
			m_context.EnterScope(funcname->GetVal());
			//std::cout << "Entering function \"" << funcname->GetVal() << "\" scope." << std::endl;
		}
		else if(args.size() == /*5*/ 4) // check
		{
			auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[1]);
			auto funcargs = std::dynamic_pointer_cast<ASTArgNames>(args[3]);

			// register argument variables
			std::size_t argidx = 0;
			for(const auto& arg : funcargs->GetArgs())
			{
				Symbol* sym = m_context.AddScopedSymbol(std::get<0>(arg));
				sym->ty = std::get<1>(arg);
				sym->is_arg = true;
				sym->argidx = argidx;
				sym->dims = std::get<2>(arg);
				++argidx;
			}

			// register the function in the symbol map
			m_context.GetSymbols().AddFunc(
				m_context.GetScopeName(1), funcname->GetVal(),
				SymbolType::VOID, funcargs->GetArgTypes());
		}

		if(!full_match)
			return nullptr;

		auto rettype = std::make_shared<ASTTypeDecl>(SymbolType::VOID);
		auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[1]);
		auto funcargs = std::dynamic_pointer_cast<ASTArgNames>(args[3]);
		auto funcblock = std::dynamic_pointer_cast<ASTStmts>(args[5]);

		auto res = std::make_shared<ASTFunc>(
			funcname->GetVal(), rettype, funcargs, funcblock);
		m_context.LeaveScope(funcname->GetVal());
		return res;
	}));
#endif
	++semanticindex;

	// function with multiple return values
#ifdef CREATE_PRODUCTION_RULES
	function->AddRule({ keyword_func, ident /*1*/,
		bracket_open, full_argumentlist /*3*/, bracket_close,
		keyword_results, bracket_open, typelist /*7*/, bracket_close,
		statements /*9*/, keyword_end, keyword_func }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 5)  // keyword_results is the first unique partial match
		{
			auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[1]);
			m_context.EnterScope(funcname->GetVal());
			//std::cout << "Entering function \"" << funcname->GetVal() << "\" scope." << std::endl;
		}
		else if(args.size() == 8)
		{
			auto retargs = std::dynamic_pointer_cast<ASTArgNames>(args[7]);
			auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[1]);
			auto funcargs = std::dynamic_pointer_cast<ASTArgNames>(args[3]);

			// register argument variables
			std::size_t argidx = 0;
			for(const auto& arg : funcargs->GetArgs())
			{
				Symbol* sym = m_context.AddScopedSymbol(std::get<0>(arg));
				sym->ty = std::get<1>(arg);
				sym->is_arg = true;
				sym->argidx = argidx;
				sym->dims = std::get<2>(arg);
				++argidx;
			}

			// register the function in the symbol map
			std::vector<SymbolType> multirettypes = retargs->GetArgTypes();
			m_context.GetSymbols().AddFunc(
				m_context.GetScopeName(1), funcname->GetVal(),
				SymbolType::COMP, funcargs->GetArgTypes(),
				nullptr, &multirettypes);
		}

		if(!full_match)
			return nullptr;

		auto rettype = std::make_shared<ASTTypeDecl>(SymbolType::COMP);
		auto retargs = std::dynamic_pointer_cast<ASTArgNames>(args[7]);
		auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[1]);
		auto funcargs = std::dynamic_pointer_cast<ASTArgNames>(args[3]);
		auto funcblock = std::dynamic_pointer_cast<ASTStmts>(args[9]);

		auto res = std::make_shared<ASTFunc>(
			funcname->GetVal(), rettype, funcargs, funcblock, retargs);
		m_context.LeaveScope(funcname->GetVal());
		return res;
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------
}
