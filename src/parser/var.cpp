/**
 * muF grammar -- variables and constants
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date feb-2025
 * @license see 'LICENSE' file
 */

#include "grammar.h"


void Grammar::CreateVariables()
{
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

		auto lst = std::dynamic_pointer_cast<ASTVarDecl>(args[2]);
		lst->AddVariable(m_context.AddScopedSymbol(name)->scoped_name);
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

		auto lst = std::make_shared<ASTVarDecl>();
		lst->AddVariable(m_context.AddScopedSymbol(name)->scoped_name);
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

		auto term = std::dynamic_pointer_cast<AST>(args[2]);

		auto lst = std::make_shared<ASTVarDecl>(std::make_shared<ASTAssign>(name, term));
		lst->AddVariable(m_context.AddScopedSymbol(name)->scoped_name);
		return lst;
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// constants
	// --------------------------------------------------------------------------------
	// several int constants
#ifdef CREATE_PRODUCTION_RULES
	int_constants->AddRule({ sym_int, comma, int_constants }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto int_node = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[0]);
		const t_int val = int_node->GetVal();

		auto lst = std::dynamic_pointer_cast<ASTNumConstList<t_int>>(args[2]);
		lst->AddValue(val);
		return lst;
	}));
#endif
	++semanticindex;

	// a single int constant
#ifdef CREATE_PRODUCTION_RULES
	int_constants->AddRule({ sym_int }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto int_node = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[0]);
		const t_int val = int_node->GetVal();

		auto lst = std::make_shared<ASTNumConstList<t_int>>();
		lst->AddValue(val);
		return lst;
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// statement
	// --------------------------------------------------------------------------------
	// int declaration
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ int_decl, type_sep, variables/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 1)
			m_context.SetSymType(SymbolType::INT);

		if(!full_match)
			return nullptr;
		return args[2];
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

	// quaternion declaration
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ quat_decl, type_sep, variables/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 1)
			m_context.SetSymType(SymbolType::QUAT);

		if(!full_match)
			return nullptr;
		return args[2];
	}));
#endif
	++semanticindex;

	// bool declaration
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ bool_decl, type_sep, variables/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 1)
			m_context.SetSymType(SymbolType::BOOL);

		if(!full_match)
			return nullptr;
		return args[2];
	}));
#endif
	++semanticindex;

	// int array declaration
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ int_decl, comma,
		keyword_dim, bracket_open, int_constants, bracket_close,
		type_sep, variables/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 6)
		{
			auto dim_node = std::dynamic_pointer_cast<ASTNumConstList<t_int>>(args[4]);
			std::vector<std::size_t> dims;
			dims.reserve(dim_node->GetSize());
			for(t_int dim : dim_node->GetValues())
				dims.push_back(static_cast<std::size_t>(dim));

			m_context.SetSymType(SymbolType::INT_ARRAY);
			m_context.SetSymDims(dims);
		}

		if(!full_match)
			return nullptr;
		return args[7];
	}));
#endif
	++semanticindex;

	// real array declaration
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ real_decl, comma,
		keyword_dim, bracket_open, int_constants, bracket_close,
		type_sep, variables/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 6)
		{
			auto dim_node = std::dynamic_pointer_cast<ASTNumConstList<t_int>>(args[4]);
			std::vector<std::size_t> dims;
			dims.reserve(dim_node->GetSize());
			for(t_int dim : dim_node->GetValues())
				dims.push_back(static_cast<std::size_t>(dim));


			m_context.SetSymType(SymbolType::REAL_ARRAY);
			m_context.SetSymDims(dims);
		}

		if(!full_match)
			return nullptr;
		return args[7];
	}));
#endif
	++semanticindex;

	// complex array declaration
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ cplx_decl, comma,
		keyword_dim, bracket_open, sym_int, bracket_close,
		type_sep, variables/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 6)
		{
			auto dim_node = std::dynamic_pointer_cast<ASTNumConstList<t_int>>(args[4]);
			std::vector<std::size_t> dims;
			dims.reserve(dim_node->GetSize());
			for(t_int dim : dim_node->GetValues())
				dims.push_back(static_cast<std::size_t>(dim));

			m_context.SetSymType(SymbolType::CPLX_ARRAY);
			m_context.SetSymDims(dims);
		}

		if(!full_match)
			return nullptr;
		return args[7];
	}));
#endif
	++semanticindex;

	// quaternion array declaration
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ quat_decl, comma,
		keyword_dim, bracket_open, sym_int, bracket_close,
		type_sep, variables/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 6)
		{
			auto dim_node = std::dynamic_pointer_cast<ASTNumConstList<t_int>>(args[4]);
			std::vector<std::size_t> dims;
			dims.reserve(dim_node->GetSize());
			for(t_int dim : dim_node->GetValues())
				dims.push_back(static_cast<std::size_t>(dim));

			m_context.SetSymType(SymbolType::QUAT_ARRAY);
			m_context.SetSymDims(dims);
		}

		if(!full_match)
			return nullptr;
		return args[7];
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
			m_context.SetSymDim(default_string_size);
		}

		if(!full_match)
			return nullptr;
		return args[2];
	}));
#endif
	++semanticindex;

	// string declaration with given static size
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ str_decl, comma,
		keyword_dim, bracket_open, sym_int, bracket_close,
		type_sep, variables/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(args.size() == 6)
		{
			auto dim_node = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[4]);
			const t_int dim = dim_node->GetVal();

			m_context.SetSymType(SymbolType::STRING);
			m_context.SetSymDim(std::size_t(dim));
		}

		if(!full_match)
			return nullptr;
		return args[7];
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
        // full_identlist
        // --------------------------------------------------------------------------------
#ifdef CREATE_PRODUCTION_RULES
        full_identlist->AddRule({ identlist }, semanticindex);
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

        // full_identlist -> eps
#ifdef CREATE_PRODUCTION_RULES
        full_identlist->AddRule({ lalr1::g_eps }, semanticindex);
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
	// expression
	// --------------------------------------------------------------------------------
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

	// expression -> ( real, real, real, real ) [quaternion constant]
#ifdef CREATE_PRODUCTION_RULES
	expression->AddRule({ bracket_open,
		sym_real, comma, sym_real, comma, sym_real, comma, sym_real,
		bracket_close }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto real_node = std::dynamic_pointer_cast<ASTNumConst<t_real>>(args[1]);
		auto imag1_node = std::dynamic_pointer_cast<ASTNumConst<t_real>>(args[3]);
		auto imag2_node = std::dynamic_pointer_cast<ASTNumConst<t_real>>(args[5]);
		auto imag3_node = std::dynamic_pointer_cast<ASTNumConst<t_real>>(args[7]);
		const t_quat num{real_node->GetVal(), imag1_node->GetVal(), imag2_node->GetVal(), imag3_node->GetVal()};
		return std::make_shared<ASTNumConst<t_quat>>(num);
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
			SymbolPtr sym = m_context.FindScopedSymbol(identstr);

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
}
