/**
 * symbol table
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 13-apr-20
 * @license see 'LICENSE' file
 */

#include "sym.h"

#include <iomanip>



/**
 * get the corresponding data type name
 */
t_str Symbol::get_type_name(SymbolType ty)
{
	switch(ty)
	{
		case SymbolType::REAL: return "real";
		case SymbolType::INT: return "integer";
		case SymbolType::CPLX: return "complex";
		case SymbolType::QUAT: return "quaternion";
		case SymbolType::BOOL: return "bool";

		case SymbolType::REAL_ARRAY: return "real_array";
		case SymbolType::INT_ARRAY: return "integer_array";
		case SymbolType::CPLX_ARRAY: return "complex_array";
		case SymbolType::QUAT_ARRAY: return "quaternion_array";

		case SymbolType::STRING: return "string";

		case SymbolType::VOID: return "void";
		case SymbolType::COMP: return "comp";
		case SymbolType::FUNC: return "func";

		case SymbolType::UNKNOWN: return "unknown";
	}

	return "invalid";
}


const t_str& Symbol::get_scopenameseparator()
{
	static const t_str sep{"::"};
	return sep;
}


SymbolPtr SymTab::AddSymbol(const t_str& scope,
	const t_str& name, SymbolType ty,
	const std::vector<std::size_t>& dims,
	bool is_temp)
{
	SymbolPtr sym = std::make_shared<Symbol>();

	sym->name = name;
	sym->scoped_name = scope + name;
	sym->scope_name = scope;
	sym->ty = ty;
	sym->dims = dims;
	sym->elems = {};
	sym->is_tmp = is_temp;
	sym->refcnt = 0;

	auto pair = m_syms.insert_or_assign(sym->scoped_name, sym);
	return pair.first->second;
}


SymbolPtr SymTab::AddFunc(const t_str& scope,
	const t_str& name, SymbolType retty,
	const std::vector<SymbolType>& argtypes,
	const std::vector<std::size_t>* retdims,
	const std::vector<SymbolType>* rettypes,
	bool is_external)
{
	SymbolPtr sym = std::make_shared<Symbol>();

	sym->name = name;
	sym->scoped_name = scope + name;
	sym->scope_name = scope;
	sym->ty = SymbolType::FUNC;
	sym->argty = argtypes;
	sym->retty = retty;
	sym->is_external = is_external;
	sym->refcnt = 0;

	if(retdims)
		sym->retdims = *retdims;

	if(rettypes)
	{
		for(SymbolType ty : *rettypes)
		{
			auto retsym = std::make_shared<Symbol>();
			retsym->ty = ty;
			sym->elems.emplace_back(retsym);
		}
	}

	auto pair = m_syms.insert_or_assign(sym->scoped_name, sym);
	return pair.first->second;
}


SymbolPtr SymTab::AddExtFunc(const t_str& scope,
	const t_str& name, const t_str& extfunc_name,
	SymbolType retty,
	const std::vector<SymbolType>& argtypes,
	const std::vector<std::size_t>* retdims,
	const std::vector<SymbolType>* rettypes)
{
	SymbolPtr sym =
		AddFunc(scope, name, retty, argtypes, retdims, rettypes, true);
	sym->ext_name = extfunc_name;
	return sym;
}


SymbolPtr SymTab::FindSymbol(const t_str& name) const
{
	auto iter = m_syms.find(name);
	if(iter == m_syms.end())
		return nullptr;
	return iter->second;
}


std::vector<SymbolPtr> SymTab::FindSymbolsWithSameScope(
	const t_str& scope, bool no_args) const
{
	std::vector<SymbolPtr> syms;

	for(const auto& [name, sym] : m_syms)
	{
		//if(sym->addr)
		//	std::cout << name << ": " << *sym->addr << std::endl;

		if(no_args && sym->is_arg)
			continue;

		if(sym->scope_name == scope)
			syms.push_back(sym);
	}

	return syms;
}


const std::unordered_map<t_str, SymbolPtr>& SymTab::GetSymbols() const
{
	return m_syms;
}


std::ostream& operator<<(std::ostream& ostr, const SymTab& tab)
{
	const int name_len = 32;
	const int type_len = 24;
	const int refs_len = 8;
	const int addr_len = 16;
	const int dims_len = 8;

	ostr
		<< std::left << std::setw(name_len) << "full name"
		<< std::left << std::setw(type_len) << "type"
		<< std::left << std::setw(refs_len) << "refs"
		<< std::left << std::setw(addr_len) << "addr"
		<< std::left << std::setw(dims_len) << "dims"
		<< '\n';
	for(int i = 0; i < name_len + type_len + refs_len + addr_len + dims_len; ++i)
		ostr << '-';
	ostr << '\n';

	for(const auto& pair : tab.m_syms)
	{
		const Symbol& sym = *pair.second;

		std::string ty = Symbol::get_type_name(sym.ty);
		if(sym.is_external)
			ty += " (ext)";
		if(sym.is_global)
			ty += " (global)";
		if(sym.is_arg)
			ty += " (arg " + std::to_string(sym.argidx) + ")";
		if(sym.is_ret)
			ty += " (ret " + std::to_string(sym.retidx) + ")";
		if(sym.is_tmp)
			ty += " (tmp)";

		std::string addr = "";
		if(sym.addr)
			addr = std::to_string(*sym.addr);
		if(sym.end_addr)
			addr += " - " + std::to_string(*sym.end_addr);

		ostr << std::left << std::setw(name_len) << pair.first
			<< std::left << std::setw(type_len) << ty
			<< std::left << std::setw(refs_len) << sym.refcnt
			<< std::left << std::setw(addr_len) << addr;
		for(std::size_t i = 0; i < sym.dims.size(); ++i)
			ostr << std::left << std::setw(dims_len) << sym.dims[i];
		ostr << '\n';
	}

	return ostr;
}
