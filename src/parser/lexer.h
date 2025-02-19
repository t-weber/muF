/**
 * muF lexer
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date feb-2025
 * @license see 'LICENSE' file
 */

#ifndef __MUF_LEXER_H__
#define __MUF_LEXER_H__


#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <optional>

#include "lval.h"
#include "lalr1/common.h"


// [ token, lvalue, line number ]
using t_lexer_match = std::tuple<lalr1::t_symbol_id, t_lval, std::size_t>;


/**
 * terminal symbol identifiers
 */
enum class Token : lalr1::t_symbol_id
{
	// tokens with an lvalue
	REAL        = 1000,
	INT         = 1001,
	BOOL        = 1002,
	STR         = 1003,
	IDENT       = 1004,

	// type declarations
	TYPESEP     = 2000,
	REALDECL    = 2010,
	CPLXDECL    = 2011,
	INTDECL     = 2012,
	ARRDECL     = 2020,
	STRINGDECL  = 2030,

	// logical operators
	EQU         = 3000,
	NEQ         = 3001,
	GEQ         = 3002,
	LEQ         = 3003,
	AND         = 3004,
	OR          = 3005,
	XOR         = 3006,
	NOT         = 3010,

	// arithmetical operators
	POW         = 3500,

	ASSIGN      = 4000,
	RANGE       = 4001,

	// conditionals
	IF          = 5000,
	THEN        = 5001,
	ELSE        = 5002,

	// loops
	WHILE       = 6000,
	DO          = 6001,
	BREAK       = 6002,
	NEXT        = 6003,

	END         = 6010,
	//ENDSTMT     = 6011,

	// functions
	FUNC        = 7000,
	PROC        = 7001,
	RET         = 7002,
	RESULT      = 7003,
	RESULTS     = 7004,
	PROGRAM     = 7010,

	// jumps
	LABEL       = 8000,
	GOTO        = 8001,
	COMEFROM    = 8002,

	// partial match (should not be left after final lexer run)
	PARTIAL     = 9999,

	HALT        = lalr1::END_IDENT,
};


class Lexer
{
public:
	Lexer(std::istream* = &std::cin);

	// get all tokens and attributes
	std::vector<lalr1::t_toknode> GetAllTokens();

	void SetTermIdxMap(const lalr1::t_mapIdIdx* map) { m_mapTermIdx = map; }


protected:
	// get next token and attribute
	t_lexer_match GetNextToken(std::size_t* line = nullptr);

	// find all matching tokens for input string
	std::vector<t_lexer_match> GetMatchingTokens(
		const std::string& str, std::size_t line);


private:
	std::istream* m_istr{nullptr};
	const lalr1::t_mapIdIdx* m_mapTermIdx{nullptr};
};


#endif
