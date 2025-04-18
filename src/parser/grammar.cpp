/**
 * muF grammar
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date feb-2025
 * @license see 'LICENSE' file
 */

#include "grammar.h"
#include "lexer.h"


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
	int_constants = std::make_shared<lalr1::NonTerminal>(INT_CONSTANTS, "int_constants");
	full_identlist = std::make_shared<lalr1::NonTerminal>(FULL_IDENTLIST, "full_identlist");
	identlist = std::make_shared<lalr1::NonTerminal>(IDENTLIST, "identlist");
	function = std::make_shared<lalr1::NonTerminal>(FUNCTION, "function");
	opt_recursive = std::make_shared<lalr1::NonTerminal>(OPT_RECURSIVE, "opt_recursive");
	opt_intent = std::make_shared<lalr1::NonTerminal>(OPT_INTENT, "opt_intent");
	opt_assign = std::make_shared<lalr1::NonTerminal>(OPT_ASSIGN, "opt_assign");
	var_range = std::make_shared<lalr1::NonTerminal>(VAR_RANGE, "var_range");
	cases = std::make_shared<lalr1::NonTerminal>(CASES, "cases");

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
	quat_decl = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::QUATDECL), "quat_decl");
	int_decl = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::INTDECL), "integer_decl");
	bool_decl = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::BOOLDECL), "bool_decl");
	str_decl = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::STRINGDECL), "string_decl");

	keyword_if = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::IF), "if");
	keyword_then = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::THEN), "then");
	keyword_else = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::ELSE), "else");
	keyword_end = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::END), "end");

	keyword_select = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::SELECT), "select");
	keyword_case = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::CASE), "case");
	keyword_default = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::DEFAULT), "default");

	keyword_while = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::WHILE), "while");
	keyword_do = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::DO), "do");
	keyword_next = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::NEXT), "next");
	keyword_break = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::BREAK), "break");

	keyword_func = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::FUNC), "function");
	keyword_procedure = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::PROC), "procedure");
	keyword_ret = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::RET), "return");
	keyword_recursive = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::RECURSIVE), "recursive");
	keyword_intent = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::INTENT), "intent");
	keyword_in = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::IN), "in");
	keyword_out = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::OUT), "out");
	keyword_program = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::PROGRAM), "program");

	keyword_results = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::RESULTS), "results");

	keyword_assign = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::ASSIGN), "assign");
	keyword_dim = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::DIM), "dimension");

	keyword_read = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::READ), "read");
	keyword_print = std::make_shared<lalr1::Terminal>(static_cast<std::size_t>(Token::PRINT), "print");

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

	// current rule semantic id number
	semanticindex = 0;

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
		GetContext().SetStatements(stmts);
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
	// statement
	// --------------------------------------------------------------------------------
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
		//GetContext().SetStatements(stmts);
		return stmts;
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
	// input/output
	// --------------------------------------------------------------------------------
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_print, op_mult, comma, expressions }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto write_stmts = std::make_shared<ASTStmts>();

		// create a list of write statements out of the argument expressions
		auto exprs = std::dynamic_pointer_cast<ASTExprList>(args[3]);
		for(ASTPtr expr : exprs->GetList())
		{
			auto arg = std::make_shared<ASTExprList>(expr);
			auto write_stmt = std::make_shared<ASTCall>("write_no_cr", arg);
			write_stmts->AddStatement(write_stmt, true);
		}

		// newline
		auto arg = std::make_shared<ASTExprList>(std::make_shared<ASTStrConst>("\n"));
		auto write_stmt = std::make_shared<ASTCall>("write_no_cr", arg);
		write_stmts->AddStatement(write_stmt, true);

		return write_stmts;
	}));
#endif
	++semanticindex;

#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_read, op_mult, comma, identlist }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto read_stmts = std::make_shared<ASTStmts>();

		// create a list of read statements using each identifier
		auto idents = std::dynamic_pointer_cast<ASTInternalArgNames>(args[3]);
		for(const auto& [ident, _ty, _dims] : idents->GetArgs())
		{
			const SymbolPtr sym = GetContext().FindScopedSymbol(ident);
			if(!sym)
			{
				std::ostringstream ostr;
				ostr << "Cannot find symbol \"" << ident << "\" in read statement.";
				throw std::runtime_error(ostr.str());
			}

			std::shared_ptr<ASTCall> read_stmt;
			auto arg = std::make_shared<ASTExprList>(std::make_shared<ASTStrConst>(""));

			if(sym->ty == SymbolType::REAL)
			{
				read_stmt = std::make_shared<ASTCall>("read_real", arg);
			}
			else if(sym->ty == SymbolType::INT)
			{
				read_stmt = std::make_shared<ASTCall>("read_integer", arg);
			}
			else
			{
				std::ostringstream ostr;
				ostr << "Unsupported type \"" << Symbol::get_type_name(sym->ty)
					<< "\" for \"" << ident << "\" in read statement.";
				throw std::runtime_error(ostr.str());
			}

			auto assign = std::make_shared<ASTAssign>(ident, read_stmt);
			read_stmts->AddStatement(assign, true);
		}

		return read_stmts;
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	CreateOperators();
	CreateVariables();
	CreateArrays();
	CreateLoops();
	CreateFunctions();
}
