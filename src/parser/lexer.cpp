/**
 * muF lexer
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date feb-2025
 * @license see 'LICENSE' file
 */

#include "lexer.h"
#include "ast/ast.h"

#include <sstream>
#include <memory>
#include <regex>
#include <bitset>
#include <type_traits>
#include <boost/algorithm/string.hpp>

using namespace lalr1;


template<template<std::size_t, class...> class t_func, class t_params, std::size_t ...seq>
constexpr void constexpr_loop(const std::index_sequence<seq...>&, const t_params& params)
{
	( (std::apply(t_func<seq>{}, params)), ... );
}


Lexer::Lexer(std::istream* istr) : m_istr{istr}
{
}


/**
 * find all matching tokens for input string
 */
std::vector<t_lexer_match>
Lexer::GetMatchingTokens(const std::string& str, std::size_t line)
{
	std::vector<t_lexer_match> matches;
	if(str == "")
		return matches;

	// int
	static const std::regex regex_int{"(0[xb])?([0-9]+)"};
	std::smatch smatch_int;
	if(std::regex_match(str, smatch_int, regex_int))
	{
		t_int val{};

		if(smatch_int.str(1) == "0x")
		{
			// hexadecimal integers
			std::istringstream{smatch_int.str(0)} >> std::hex >> val;
		}
		else if(smatch_int.str(1) == "0b")
		{
			// binary integers
			using t_bits = std::bitset<sizeof(t_int)*8>;
			t_bits bits(smatch_int.str(2));

			using t_ulong = std::invoke_result_t<decltype(&t_bits::to_ulong), t_bits*>;
			if constexpr(sizeof(t_ulong) >= sizeof(t_int))
				val = static_cast<t_int>(bits.to_ulong());
			else
				val = static_cast<t_int>(bits.to_ullong());
		}
		else
		{
			// decimal integers
			std::istringstream{smatch_int.str(2)} >> std::dec >> val;
		}

		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::INT), val, line));
	}
	else if(str == "0x" || str == "0b")
	{
		// dummy matches to continue searching for longest int
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::INT), 0, line));
	}

	// real
	//static const std::regex regex_real{"[0-9]+(\\.[0-9]*)?"};
	static const std::regex regex_real{"[0-9]+(\\.[0-9]*)?([Ee][+-]?[0-9]*)?"};
	std::smatch smatch_real;
	if(std::regex_match(str, smatch_real, regex_real))
	{
		t_real val{};
		std::istringstream{str} >> val;
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::REAL), val, line));
	}

	// keywords and identifiers
	if(str == "if")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::IF), str, line));
	}
	else if(str == "then")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::THEN), str, line));
	}
	else if(str == "else")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::ELSE), str, line));
	}
	else if(str == "while")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::WHILE), str, line));
	}
	else if(str == "break")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::BREAK), str, line));
	}
	else if(str == "next")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::NEXT), str, line));
	}
	else if(str == "do")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::DO), str, line));
	}
	else if(str == "select")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::SELECT), str, line));
	}
	else if(str == "case")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::CASE), str, line));
	}
	else if(str == "default")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::DEFAULT), str, line));
	}
	else if(str == "end")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::END), str, line));
	}
	else if(str == "function")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::FUNC), str, line));
	}
	else if(str == "procedure")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::PROC), str, line));
	}
	else if(str == "return")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::RET), str, line));
	}
	else if(str == "result")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::RESULT), str, line));
	}
	else if(str == "results")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::RESULTS), str, line));
	}
	else if(str == "assign")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::ASSIGN), str, line));
	}
	else if(str == "integer")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::INTDECL), str, line));
	}
	else if(str == "real")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::REALDECL), str, line));
	}
	else if(str == "logical")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::BOOLDECL), str, line));
	}
	else if(str == "complex")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::CPLXDECL), str, line));
	}
	else if(str == "string")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::STRINGDECL), str, line));
	}
	else if(str == "dimension")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::DIM), str, line));
	}
	else if(str == "program")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::PROGRAM), str, line));
	}
	else if(str == "goto")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::GOTO), str, line));
	}
	else if(str == "comefrom")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::COMEFROM), str, line));
	}
	else if(str == ".true.")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::BOOL), true, line));
	}
	else if(str == ".false.")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::BOOL), false, line));
	}
	else
	{
		bool found_match = false;

		// identifier
		static const std::regex regex_ident{"[_A-Za-z]+[_A-Za-z0-9]*"};
		std::smatch smatch_ident;
		if(std::regex_match(str, smatch_ident, regex_ident))
		{
			matches.emplace_back(std::make_tuple(
				static_cast<t_symbol_id>(Token::IDENT), str, line));

			found_match = true;
		}

		if(!found_match)
		{
			// labels
			static const std::regex regex_label{"\\.[_A-Za-z]+[_A-Za-z0-9]*"};
			std::smatch smatch_label;
			if(std::regex_match(str, smatch_label, regex_label))
			{
				matches.emplace_back(std::make_tuple(
					static_cast<t_symbol_id>(Token::LABEL), str, line));

				found_match = true;
			}
		}

		if(!found_match)
		{
			// partially matching keywords
			// otherwise the lexer would give up before seeing a full keyword like ".true."
			static const std::regex regex_partial{"\\.[_A-Za-z]*"};
			std::smatch smatch_partial;
			if(std::regex_match(str, smatch_partial, regex_partial))
			{
				matches.emplace_back(std::make_tuple(
					static_cast<t_symbol_id>(Token::PARTIAL), str, line));

				found_match = true;
			}
		}
	}

	// operators
	if(str == "==" || str == ".eq.")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::EQU), str, line));
	}
	else if(str == "/=" || str == ".ne.")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::NEQ), str, line));
	}
	else if(str == "||" || str == ".or.")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::OR), str, line));
	}
	else if(str == "&&" || str == ".and.")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::AND), str, line));
	}
	else if(str == "<=" || str == ".le.")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::LEQ), str, line));
	}
	else if(str == ">=" || str == ".ge.")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::GEQ), str, line));
	}
	else if(str == ".lt.")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>('<'), std::nullopt, line));
	}
	else if(str == ".gt.")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>('>'), std::nullopt, line));
	}
	else if(str == ".not.")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::NOT), str, line));
	}
	else if(str == "xor")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::XOR), str, line));
	}
	else if(str == "::")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::TYPESEP), str, line));
	}
	else if(str == "**")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::POW), str, line));
	}
	else if(str == "~")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::RANGE), str, line));
	}
	/*else if(str == ";")
	{
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(Token::ENDSTMT), str, line));
	}*/

	// tokens represented by themselves
	else if(str == "+" || str == "-" || str == "*" || str == "/" ||
		str == "%" || str == ":" || str == "," || str == "=" ||
		str == "(" || str == ")" || str == "[" || str == "]" ||
		str == ">" || str == "<" || str == "|" || str == "&")
		matches.emplace_back(std::make_tuple(
			static_cast<t_symbol_id>(str[0]), std::nullopt, line));

	//std::cerr << "Input \"" << str << "\" has " << matches.size() << " matches." << std::endl;
	return matches;
}


/**
 * replace escape sequences
 */
static void replace_escapes(std::string& str)
{
	boost::replace_all(str, "\\n", "\n");
	boost::replace_all(str, "\\t", "\t");
	boost::replace_all(str, "\\r", "\r");
}


/**
 * get next token and attribute
 */
t_lexer_match Lexer::GetNextToken(std::size_t* _line)
{
	std::string input;
	std::vector<t_lexer_match> longest_lexer_matching;
	bool eof = false;
	bool in_line_comment = false;
	bool in_string = false;

	std::size_t dummy_line = 1;
	std::size_t *line = _line;
	if(!line) line = &dummy_line;

	// find longest matching token
	while(!(eof = m_istr->eof()))
	{
		int c = m_istr->get();
		if(c == std::char_traits<char>::eof())
		{
			eof = true;
			break;
		}
		//std::cout << "Input: " << char(c) << " (0x" << std::hex << int(c) << ")." << std::endl;

		if(in_line_comment && c != '\n')
			continue;

		// if outside any other match...
		if(longest_lexer_matching.size() == 0)
		{
			if(c == '\"' && !in_line_comment)
			{
				if(!in_string)
				{
					in_string = true;
					continue;
				}

				replace_escapes(input);
				in_string = false;
				return std::make_tuple(
					static_cast<t_symbol_id>(Token::STR), input, *line);
			}

			// ... ignore comments
			if(c == '!' && !in_string)
			{
				in_line_comment = true;
				continue;
			}

			// ... ignore white spaces
			if((c==' ' || c=='\t') && !in_string)
				continue;

			// ... new line
			else if(c=='\n')
			{
				in_line_comment = false;
				++(*line);
				continue;
			}
		}

		input += c;
		if(in_string)
			continue;

		auto matching = GetMatchingTokens(input, *line);
		if(matching.size())
		{
			longest_lexer_matching = matching;

			if(m_istr->peek() == std::char_traits<char>::eof())
			{
				eof = true;
				break;
			}
		}
		else
		{
			// no more matches
			m_istr->putback(c);
			break;
		}
	}

	if(longest_lexer_matching.size() == 0 && eof)
		return std::make_tuple((t_symbol_id)Token::HALT, std::nullopt, *line);

	if(longest_lexer_matching.size() == 0)
	{
		std::ostringstream ostrErr;
		ostrErr << "Line " << *line << ": Invalid input in lexer: \""
			<< input << "\"" << " (length: " << input.length() << ").";
		throw std::runtime_error(ostrErr.str());
	}

	return longest_lexer_matching[0];
}


/**
 * create an l-value constant ast node
 */
template<std::size_t IDX> struct _Lval_LoopFunc
{
	void operator()(
		std::vector<t_toknode>* vec, t_symbol_id id, t_index tableidx,
		const t_lval& lval, std::size_t line) const
	{
		using t_val = std::variant_alternative_t<IDX, typename t_lval::value_type>;

		if(std::holds_alternative<t_val>(*lval))
		{
			t_toknode astnode = nullptr;

			if constexpr(std::is_same_v<t_val, std::string>)
				astnode = std::make_shared<ASTStrConst>(std::get<IDX>(*lval));
			else
				astnode = std::make_shared<ASTNumConst<t_val>>(std::get<IDX>(*lval));

			astnode->SetId(id);
			astnode->SetTableIndex(tableidx);
			astnode->SetLineRange(std::make_pair(line, line));
			astnode->SetTerminalOverride(true);
			vec->emplace_back(std::move(astnode));
		}
	};
};


/**
 * get all tokens and attributes
 */
std::vector<t_toknode> Lexer::GetAllTokens()
{
	std::vector<t_toknode> vec;
	std::size_t line = 1;

	while(true)
	{
		auto tup = GetNextToken(&line);
		t_symbol_id id = std::get<0>(tup);
		const t_lval& lval = std::get<1>(tup);
		std::size_t line = std::get<2>(tup);

		// get index into parse tables
		t_index tableidx = 0;
		if(m_mapTermIdx)
		{
			auto iter = m_mapTermIdx->find(id);
			if(iter != m_mapTermIdx->end())
				tableidx = iter->second;
		}

		// does this token have an attribute?
		if(lval)
		{
			// find the correct type in the variant
			auto seq = std::make_index_sequence<
				std::variant_size_v<typename t_lval::value_type>>();

			constexpr_loop<_Lval_LoopFunc>(
				seq, std::make_tuple(&vec, id, tableidx, lval, line));
		}
		else
		{
			t_toknode astnode = std::make_shared<ASTNumConst<t_int>>(-1);
			astnode->SetId(id);
			astnode->SetTableIndex(tableidx);
			astnode->SetLineRange(std::make_pair(line, line));
			astnode->SetTerminalOverride(true);
			vec.emplace_back(std::move(astnode));
		}

		if(id == (t_symbol_id)Token::HALT)
			break;
	}

	return vec;
}
