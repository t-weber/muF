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

	// terminals
	op_assign = std::make_shared<lalr1::Terminal>('=', "=");
	op_plus = std::make_shared<lalr1::Terminal>('+', "+");
	op_minus = std::make_shared<lalr1::Terminal>('-', "-");
	op_mult = std::make_shared<lalr1::Terminal>('*', "*");
	op_div = std::make_shared<lalr1::Terminal>('/', "/");
	op_mod = std::make_shared<lalr1::Terminal>('%', "%");
	op_pow = std::make_shared<lalr1::Terminal>('^', "^");
	op_norm = std::make_shared<lalr1::Terminal>('|', "|");
	op_trans = std::make_shared<lalr1::Terminal>('\'', "'");

	op_equ = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::EQU), "==");
	op_neq = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::NEQ), "!=");
	op_geq = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::GEQ), ">=");
	op_leq = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::LEQ), "<=");
	op_and = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::AND), "&&");
	op_or = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::OR), "||");
	op_xor = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::XOR), "xor");
	op_gt = std::make_shared<lalr1::Terminal>('>', ">");
	op_lt = std::make_shared<lalr1::Terminal>('<', "<");
	op_not = std::make_shared<lalr1::Terminal>('!', "#");

	bracket_open = std::make_shared<lalr1::Terminal>('(', "(");
	bracket_close = std::make_shared<lalr1::Terminal>(')', ")");
	array_begin = std::make_shared<lalr1::Terminal>('[', "[");
	array_end = std::make_shared<lalr1::Terminal>(']', "]");
	range = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::RANGE), "~");

	comma = std::make_shared<lalr1::Terminal>(',', ",");
	//stmt_end = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::ENDSTMT), "stmt_end");

	sym_real = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::REAL), "real");
	sym_int = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::INT), "integer");
	sym_str = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::STR), "string");
	ident = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::IDENT), "ident");

	type_sep = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::TYPESEP), "::");
	real_decl = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::REALDECL), "real_decl");
	vec_decl = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::VECTORDECL), "vector_decl");
	mat_decl = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::MATRIXDECL), "matrix_decl");
	int_decl = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::INTDECL), "integer_decl");
	str_decl = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::STRINGDECL), "string_decl");

	keyword_if = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::IF), "if");
	keyword_then = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::THEN), "then");
	keyword_else = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::ELSE), "else");
	keyword_while = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::WHILE), "while");
	keyword_do = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::DO), "do");
	keyword_func = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::FUNC), "function");
	keyword_program = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::PROGRAM), "program");
	keyword_ret = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::RET), "ret");
	keyword_next = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::NEXT), "next");
	keyword_end = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::END), "end");
	keyword_break = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::BREAK), "break");
	keyword_assign = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::ASSIGN), "assign");

	// for the if/else s/r conflict shift "else"
	// see: https://www.gnu.org/software/bison/manual/html_node/Non-Operators.html
	keyword_if->SetPrecedence(100, 'l');
	keyword_then->SetPrecedence(100, 'l');
	keyword_else->SetPrecedence(110, 'l');
	ident->SetPrecedence(120, 'l');
	keyword_func->SetPrecedence(0, 'l');

	// s/r conflict because of missing statement end tokens
	keyword_break->SetPrecedence(200, 'l');
	keyword_next->SetPrecedence(200, 'l');
	sym_int->SetPrecedence(210, 'l');

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

	op_not->SetPrecedence(370, 'l');
	// TODO: unary_ops->SetPrecedence(75, 'r');

	op_pow->SetPrecedence(380, 'r');

	bracket_open->SetPrecedence(390, 'l');
	array_begin->SetPrecedence(390, 'l');
	op_norm->SetPrecedence(390, 'l');
	op_trans->SetPrecedence(390, 'r');
#endif

	// rule semantic id number
	lalr1::t_semantic_id semanticindex = 0;

	// --------------------------------------------------------------------------------
	// start
	// --------------------------------------------------------------------------------
	// start -> program ident statements end program
#ifdef CREATE_PRODUCTION_RULES
	start->AddRule({ keyword_program, ident, statements, keyword_end, keyword_program }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		auto stmts = std::dynamic_pointer_cast<ASTStmts>(args[2]);
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
	// statement -> expression ;
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

	// vector declaration
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ vec_decl, type_sep, sym_int, variables/*, stmt_end*/ }, semanticindex);
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

	// matrix declaration
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ mat_decl, type_sep, sym_int, sym_int, variables/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 4)
		{
			auto dim1_node = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[2]);
			auto dim2_node = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[3]);
			const t_int dim1 = dim1_node->GetVal();
			const t_int dim2 = dim2_node->GetVal();

			m_context.SetSymType(SymbolType::MATRIX);
			m_context.SetSymDims(std::size_t(dim1), std::size_t(dim2));

		}

		if(!full_match)
			return nullptr;
		return args[4];
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

	// while loop
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

	// vector declaration
#ifdef CREATE_PRODUCTION_RULES
	typedecl->AddRule({ vec_decl, sym_int }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto dim_node = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[1]);
		const t_int dim = dim_node->GetVal();
		return std::make_shared<ASTTypeDecl>(SymbolType::VECTOR, dim);
	}));
#endif
	++semanticindex;

	// matrix declaration
#ifdef CREATE_PRODUCTION_RULES
	typedecl->AddRule({ mat_decl, sym_int, sym_int }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto dim_node1 = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[1]);
		auto dim_node2 = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[2]);
		const t_int dim1 = dim_node1->GetVal();
		const t_int dim2 = dim_node2->GetVal();
		return std::make_shared<ASTTypeDecl>(SymbolType::MATRIX, dim1, dim2);
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
		return std::make_shared<ASTTypeDecl>(SymbolType::STRING, DEFAULT_STRING_SIZE);
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
		return std::make_shared<ASTTypeDecl>(SymbolType::STRING, dim);
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
	argumentlist->AddRule({ typedecl, ident, comma, argumentlist }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto ty = std::dynamic_pointer_cast<ASTTypeDecl>(args[0]);
		auto argname = std::dynamic_pointer_cast<ASTStrConst>(args[1]);
		auto arglist = std::dynamic_pointer_cast<ASTArgNames>(args[3]);

		arglist->AddArg(argname->GetVal(), ty->GetType(), ty->GetDim(0), ty->GetDim(1));
		return arglist;
	}));
#endif
	++semanticindex;

#ifdef CREATE_PRODUCTION_RULES
	argumentlist->AddRule({ typedecl, ident }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto ty = std::dynamic_pointer_cast<ASTTypeDecl>(args[0]);
		auto argname = std::dynamic_pointer_cast<ASTStrConst>(args[1]);

		auto arglist = std::make_shared<ASTArgNames>();
		arglist->AddArg(argname->GetVal(), ty->GetType(), ty->GetDim(0), ty->GetDim(1));
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

		arglist->AddArg("ret", ty->GetType(), ty->GetDim(0), ty->GetDim(1));
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
		arglist->AddArg("ret", ty->GetType(), ty->GetDim(0), ty->GetDim(1));
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
	++semanticindex;

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

	// expression -> expression ^ expression
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

	// vector access and assignment
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
			// array access into any vector expression
			return std::make_shared<ASTArrayAccess>(term, idx);
		}
		else
		{
			// assignment of a vector element
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

	// vector ranged access and assignment
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
			// array access into any vector expression
			return std::make_shared<ASTArrayAccess>(
				term, idx1, idx2, nullptr, nullptr, true);
		}
		else
		{
			// assignment of a vector element
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

	// matrix access and assignment
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression /*0*/, array_begin,
		expression /*2*/, comma, expression /*4*/,
		array_end, opt_assign /*6*/ }, semanticindex);
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
			// array access into any matrix expression
			return std::make_shared<ASTArrayAccess>(term, idx1, idx2);
		}
		else
		{
			// assignment of a matrix element
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
					var->GetIdent(), opt_term, idx1, idx2);
			}
		}

		return nullptr;
	}));
#endif
	++semanticindex;

	// matrix ranged access and assignment
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression /*0*/, array_begin,
		expression /*2*/, range, expression /*4*/, comma,
		expression /*6*/, range, expression /*8*/,
		array_end, opt_assign /*10*/ }, semanticindex);
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
		auto idx3 = std::dynamic_pointer_cast<AST>(args[6]);
		auto idx4 = std::dynamic_pointer_cast<AST>(args[8]);

		if(!args[10])
		{
			// array access into any matrix expression
			return std::make_shared<ASTArrayAccess>(
				term, idx1, idx2, idx3, idx4, true, true);
		}
		else
		{
			// assignment of a matrix element
			if(term->type() != ASTType::Var)
			{
				std::cerr << "Can only assign to an l-value symbol." << std::endl;
				return nullptr;
			}
			else
			{
				auto opt_term = std::dynamic_pointer_cast<AST>(args[10]);
				auto var = std::static_pointer_cast<ASTVar>(term);
				return std::make_shared<ASTArrayAssign>(
					var->GetIdent(), opt_term,
					idx1, idx2, idx3, idx4, true, true);
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

	// transpose
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ expression, op_trans }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto expr = std::dynamic_pointer_cast<AST>(args[0]);
		return std::make_shared<ASTTransp>(expr);
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// function
	// --------------------------------------------------------------------------------
	// function with a single return value
#ifdef CREATE_PRODUCTION_RULES
	function->AddRule({ keyword_func, typedecl /*1*/, ident /*2*/,
		bracket_open, full_argumentlist /*4*/, bracket_close,
		statements /*6*/, keyword_end, keyword_func }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 3)
		{
			auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[2]);
			m_context.EnterScope(funcname->GetVal());
		}
		else if(args.size() == /*6*/ 5) // check
		{
			auto rettype = std::dynamic_pointer_cast<ASTTypeDecl>(args[1]);
			auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[2]);
			auto funcargs = std::dynamic_pointer_cast<ASTArgNames>(args[4]);

			// register argument variables
			std::size_t argidx = 0;
			for(const auto& arg : funcargs->GetArgs())
			{
				Symbol* sym = m_context.AddScopedSymbol(std::get<0>(arg));
				sym->ty = std::get<1>(arg);
				sym->is_arg = true;
				sym->argidx = argidx;
				std::get<0>(sym->dims) = std::get<2>(arg);
				std::get<1>(sym->dims) = std::get<3>(arg);
				++argidx;
			}

			// register the function in the symbol map
			std::array<std::size_t, 2> retdims{{
				rettype->GetDim(0), rettype->GetDim(1)}};
			m_context.GetSymbols().AddFunc(
				m_context.GetScopeName(1), funcname->GetVal(),
				rettype->GetType(), funcargs->GetArgTypes(), &retdims);
		}

		if(!full_match)
			return nullptr;

		auto rettype = std::dynamic_pointer_cast<ASTTypeDecl>(args[1]);
		auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[2]);
		auto funcargs = std::dynamic_pointer_cast<ASTArgNames>(args[4]);
		auto funcblock = std::dynamic_pointer_cast<ASTStmts>(args[6]);

		auto res = std::make_shared<ASTFunc>(
			funcname->GetVal(), rettype, funcargs, funcblock);
		m_context.LeaveScope(funcname->GetVal());
		return res;
	}));
#endif
	++semanticindex;

	// function with no return value
#ifdef CREATE_PRODUCTION_RULES
	function->AddRule({ keyword_func, ident /*1*/,
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
				std::get<0>(sym->dims) = std::get<2>(arg);
				std::get<1>(sym->dims) = std::get<3>(arg);
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
	function->AddRule({ keyword_func,
		bracket_open, typelist /*2*/, bracket_close,
		ident /*4*/, bracket_open, full_argumentlist /*6*/, bracket_close,
		statements /*8*/, keyword_end, keyword_func }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 5)
		{
			auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[4]);
			m_context.EnterScope(funcname->GetVal());
		}
		else if(args.size() == /*8*/ 7) // check
		{
			auto retargs = std::dynamic_pointer_cast<ASTArgNames>(args[2]);
			auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[4]);
			auto funcargs = std::dynamic_pointer_cast<ASTArgNames>(args[6]);

			// register argument variables
			std::size_t argidx = 0;
			for(const auto& arg : funcargs->GetArgs())
			{
				Symbol* sym = m_context.AddScopedSymbol(std::get<0>(arg));
				sym->ty = std::get<1>(arg);
				sym->is_arg = true;
				sym->argidx = argidx;
				std::get<0>(sym->dims) = std::get<2>(arg);
				std::get<1>(sym->dims) = std::get<3>(arg);
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
		auto retargs = std::dynamic_pointer_cast<ASTArgNames>(args[2]);
		auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[4]);
		auto funcargs = std::dynamic_pointer_cast<ASTArgNames>(args[6]);
		auto funcblock = std::dynamic_pointer_cast<ASTStmts>(args[8]);

		auto res = std::make_shared<ASTFunc>(
			funcname->GetVal(), rettype, funcargs, funcblock, retargs);
		m_context.LeaveScope(funcname->GetVal());
		return res;
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------
}
