/**
 * muF grammar
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date feb-2025
 * @license see 'LICENSE' file
 */

#ifndef __MUF_GRAMMAR_H__
#define __MUF_GRAMMAR_H__

#include "lalr1/types.h"
#include "lalr1/symbol.h"
#include "lalr1/ast.h"
#include "ast/ast.h"
#include "common/context.h"


/**
 * non-terminal symbol identifiers
 */
enum : std::size_t
{
	START,

	EXPRESSION,
	EXPRESSIONS,

	STATEMENT,
	STATEMENTS,

	VARIABLES,
	FULL_ARGUMENTLIST,
	ARGUMENTLIST,
	IDENTLIST,
	TYPELIST,

	FUNCTION,

	TYPEDECL,
	OPT_ASSIGN,

	VAR_RANGE,
};


class Grammar
{
public:
	void CreateGrammar();

#ifdef CREATE_PRODUCTION_RULES
	template<template<class...> class t_cont = std::vector>
	t_cont<lalr1::NonTerminalPtr> GetAllNonTerminals() const
	{
		return t_cont<lalr1::NonTerminalPtr>{{
			start,
			expression, expressions,
			statement, statements,
			variables,
			full_argumentlist, argumentlist,
			identlist, typelist,
			function,
			typedecl,
			opt_assign,
			var_range,
		}};
	}

	const lalr1::NonTerminalPtr& GetStartNonTerminal() const { return start; }
#endif

#ifdef CREATE_SEMANTIC_RULES
	const lalr1::t_semanticrules& GetSemanticRules() const { return rules; }
#endif

	const ParserContext& GetContext() const { return m_context; }
	ParserContext& GetContext() { return m_context; }


private:
#ifdef CREATE_PRODUCTION_RULES
	// non-terminals
	lalr1::NonTerminalPtr start{},
		expressions{}, expression{},
		statements{}, statement{},
		variables{},
		full_argumentlist{}, argumentlist{},
		identlist{}, typelist{},
		function{},
		typedecl{},
		opt_assign{},
		var_range{};

	// terminals
	lalr1::TerminalPtr op_assign{}, op_plus{}, op_minus{},
		op_mult{}, op_div{}, op_mod{}, op_pow{},
		op_norm{}, op_trans{};
	lalr1::TerminalPtr op_and{}, op_or{}, op_not{}, op_xor{},
		op_equ{}, op_neq{},
		op_lt{}, op_gt{}, op_geq{}, op_leq{};
	lalr1::TerminalPtr bracket_open{}, bracket_close{};
	lalr1::TerminalPtr array_begin{}, array_end{}, range{};
	lalr1::TerminalPtr keyword_if{}, keyword_then{}, keyword_else{};
	lalr1::TerminalPtr keyword_while{}, keyword_do{},
		keyword_break{}, keyword_next{}, keyword_end{};
	lalr1::TerminalPtr keyword_func{}, keyword_ret{};
	lalr1::TerminalPtr keyword_procedure{};
	lalr1::TerminalPtr keyword_result{}, keyword_results{};
	lalr1::TerminalPtr keyword_program{};
	lalr1::TerminalPtr keyword_assign{};
	lalr1::TerminalPtr comma{}/*, stmt_end{}*/;
	lalr1::TerminalPtr sym_real{}, sym_int{}, sym_bool{}, sym_str{};
	lalr1::TerminalPtr ident{}, label{};
	lalr1::TerminalPtr keyword_goto{}, keyword_comefrom{};
	lalr1::TerminalPtr type_sep{}, real_decl{}, vec_decl{}, mat_decl{},
		int_decl{}, str_decl{};
#endif

	ParserContext m_context{};

#ifdef CREATE_SEMANTIC_RULES
	// semantic rules
	lalr1::t_semanticrules rules{};
#endif
};

#endif
