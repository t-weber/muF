/**
 * symbol table
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 13-apr-20
 * @license see 'LICENSE' file
 */

#ifndef __SYMTAB_H__
#define __SYMTAB_H__

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <numeric>
#include <optional>
#include <iostream>

#include "types.h"



struct Symbol;
using SymbolPtr = std::shared_ptr<Symbol>;



enum class SymbolType
{
	VOID,

	REAL,
	INT,
	CPLX,
	QUAT,

	BOOL,
	STRING,

	REAL_ARRAY,
	INT_ARRAY,
	CPLX_ARRAY,
	QUAT_ARRAY,

	COMP,     // compound
	FUNC,     // function pointer

	UNKNOWN,
};



struct Symbol
{
	t_str name{};                       // local symbol identifier
	t_str scoped_name{};                // full identifier with scope prefixes
	t_str scope_name{};                 // scope prefixes
	std::optional<t_str> ext_name{};    // name of external symbol (if different from internal name)

	SymbolType ty{ SymbolType::VOID };  // symbol type
	std::vector<std::size_t> dims{ 1 }; // array dimensions

	// for functions
	bool is_arg{ false };               // symbol is a function argument
	bool is_ret{ false };               // symbol is a function return value
	std::size_t argidx{ 0 };            // argument index
	std::size_t retidx{ 0 };            // return value index
	std::vector<SymbolType> argty{};
	SymbolType retty = SymbolType::VOID;
	std::vector<std::size_t> retdims{ 1 };

	// for compound type
	std::vector<SymbolPtr> elems{};

	bool is_tmp{false};                 // temporary or declared variable?
	bool is_external{false};            // link to external variable or function?
	bool is_global{false};              // symbol is global
	std::optional<t_int> addr{};        // optional address of function or variable
	std::optional<t_int> end_addr{};    // optional address of function

	mutable std::size_t refcnt{ 0 };    // number of reference to this symbol

	/**
	 * get total element count for arrays
	 */
	std::size_t get_total_size(std::size_t start_dim = 0) const
	{
		return std::accumulate(std::next(dims.begin(), start_dim), dims.end(), 1,
			[](std::size_t i, std::size_t j) -> std::size_t
		{
			return i * j;
		});
	}

	// get the corresponding data type name
	static t_str get_type_name(SymbolType ty);

	static const t_str& get_scopenameseparator();
};



/**
 * symbol table
 */
class SymTab
{
public:
	SymbolPtr AddSymbol(const t_str& scope,
		const t_str& name, SymbolType ty,
		const std::vector<std::size_t>& dims,
		bool is_temp = false);

	SymbolPtr AddFunc(const t_str& scope,
		const t_str& name, SymbolType retty,
		const std::vector<SymbolType>& argtypes,
		const std::vector<std::size_t>* retdims = nullptr,
		const std::vector<SymbolType>* multirettypes = nullptr,
		bool is_external = false);

	SymbolPtr AddExtFunc(const t_str& scope,
		const t_str& name, const t_str& extfunc_name,
		SymbolType retty,
		const std::vector<SymbolType>& argtypes,
		const std::vector<std::size_t>* retdims = nullptr,
		const std::vector<SymbolType>* multirettypes = nullptr);

	SymbolPtr FindSymbol(const t_str& name) const;

	std::vector<SymbolPtr> FindSymbolsWithSameScope(
		const t_str& scope, bool no_args = true) const;

	const std::unordered_map<t_str, SymbolPtr>& GetSymbols() const;

	friend std::ostream& operator<<(std::ostream& ostr, const SymTab& tab);


private:
	std::unordered_map<t_str, SymbolPtr> m_syms{};
};


#endif
