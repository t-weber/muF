/**
 * parser
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 27-may-18
 * @license see 'LICENSE' file
 */

#ifndef __PARSER_CONTEXT_H__
#define __PARSER_CONTEXT_H__

#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <cmath>

#include "ast/ast.h"



/**
 * holds the common lexer state
 */
class LexerContext
{
protected:
	std::size_t m_curline = 1;

public:
	LexerContext() = default;
	virtual ~LexerContext() = default;

	LexerContext(const LexerContext&) = delete;
	const LexerContext& operator=(const LexerContext&) = delete;

	void IncCurLine() { ++m_curline; }
	std::size_t GetCurLine() const { return m_curline; }
};



/**
 * holds the common parser state
 */
class ParserContext
{
public:
	// possible data types
	using t_variant = std::variant<t_real, t_int, t_cplx, t_str>;


protected:
	std::shared_ptr<ASTStmts> m_statements{};

	SymTab m_symbols{};
	std::unordered_map<t_str, t_variant> m_consts
	{{
		{ "pi", t_real(M_PI) },
	}};

	// information about currently parsed symbol
	std::vector<t_str> m_curscope{};
	SymbolType m_symtype = SymbolType::REAL;
	std::vector<std::size_t> m_symdims{ 1 };


public:
	ParserContext() = default;
	virtual ~ParserContext() = default;


	virtual std::size_t GetCurLine() const
	{
		return 0;
	};


	// --------------------------------------------------------------------
	void SetStatements(std::shared_ptr<ASTStmts> stmts)
	{
		m_statements = stmts;
	}


	const std::shared_ptr<ASTStmts> GetStatements() const
	{
		return m_statements;
	}
	// --------------------------------------------------------------------


	// --------------------------------------------------------------------
	/**
	 * current function scope
	 */
	const std::vector<t_str>& GetScope() const
	{
		return m_curscope;
	}


	/**
	 * get the currently active scope name, ignoring the last "up" levels
	 */
	t_str GetScopeName(std::size_t up = 0, bool add_last_sep = true) const
	{
		t_str name;
		for(std::size_t level = 0; level < m_curscope.size() - up; ++level)
		{
			name += m_curscope[level];
			if(add_last_sep || level < m_curscope.size() - up - 1)
				name += Symbol::get_scopenameseparator();
		}
		return name;
	}


	void EnterScope(const t_str& name)
	{
		m_curscope.push_back(name);
	}


	void LeaveScope(const t_str& name)
	{
		const t_str& curscope = *m_curscope.rbegin();

		if(curscope != name)
		{
			std::cerr << "Error in line " << GetCurLine()
				<< ": Trying to leave scope " << name
				<< ", but the top scope is " << curscope
				<< "." << std::endl;
		}

		m_curscope.pop_back();
	}
	// --------------------------------------------------------------------


	// --------------------------------------------------------------------
	SymbolPtr AddScopedSymbol(const t_str& name)
	{
		const t_str scope = GetScopeName();

		// look for existing symbol
		if(SymbolPtr sym = m_symbols.FindSymbol(scope + name); sym)
		{
			// don't modify already registered functions here
			if(sym->ty == SymbolType::FUNC)
				return m_symbols.AddSymbol(scope, name, m_symtype, m_symdims, false);

			sym->ty = m_symtype;
			sym->dims = m_symdims;

			// if it's a function argument, set the type
			if(sym->is_arg)
			{
				const t_str funcname = GetScopeName(0, false);
				SymbolPtr func = m_symbols.FindSymbol(funcname);
				if(!func)
				{
					std::cerr << "Error in line " << GetCurLine()
						<< ": Could not find function " << funcname
						<< "." << std::endl;
					return nullptr;
				}

				if(sym->argidx >= func->argty.size())
				{
					std::cerr << "Error in line " << GetCurLine()
						<< ": Function argument index " << sym->argidx
						<< " out of bounds." << std::endl;
					return nullptr;
				}

				func->argty[sym->argidx] = sym->ty;
			}

			return sym;
		}

		// add a new symbol
		return m_symbols.AddSymbol(scope, name, m_symtype, m_symdims);
	}


	const SymbolPtr FindScopedSymbol(const t_str& name) const
	{
		const t_str scope = GetScopeName();
		return m_symbols.FindSymbol(scope + name);
	}


	const SymbolPtr FindGlobalSymbol(const t_str& name) const
	{
		return m_symbols.FindSymbol(name);
	}


	SymbolPtr FindGlobalSymbol(const t_str& name)
	{
		return m_symbols.FindSymbol(name);
	}


	const SymTab& GetSymbols() const
	{
		return m_symbols;
	}


	SymTab& GetSymbols()
	{
		return m_symbols;
	}


	/**
	 * type of current symbol
	 */
	void SetSymType(SymbolType ty)
	{
		m_symtype = ty;
	}


	SymbolType GetSymType() const
	{
		return m_symtype;
	}


	/**
	 * dimensions of n-d vector symbol
	 */
	void SetSymDims(const std::vector<std::size_t>& dims)
	{
		m_symdims = dims;
	}


	/**
	 * dimension of 1-d vector symbol
	 */
	void SetSymDim(std::size_t dim)
	{
		m_symdims.resize(1);
		m_symdims[0] = dim;
	}


	std::pair<bool, t_variant> GetConst(const t_str& name) const
	{
		auto iter = m_consts.find(name);
		if(iter == m_consts.end())
			return std::make_pair(false, t_real(0.));

		return std::make_pair(true, iter->second);
	}
	// --------------------------------------------------------------------


	void SetDebug(bool b)
	{
		m_symbols.SetDebug(b);
	}
};


#endif
