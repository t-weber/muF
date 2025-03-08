/**
 * muF grammar -- functions
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date feb-2025
 * @license see 'LICENSE' file
 */

#include "grammar.h"


void Grammar::CreateFunctions()
{
	// --------------------------------------------------------------------------------
	// typedecl
	// --------------------------------------------------------------------------------
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

	// bool declaration
#ifdef CREATE_PRODUCTION_RULES
	typedecl->AddRule({ bool_decl }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, [[maybe_unused]] const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		return std::make_shared<ASTTypeDecl>(SymbolType::BOOL);
	}));
#endif
	++semanticindex;

	// int array declaration
#ifdef CREATE_PRODUCTION_RULES
	typedecl->AddRule({ int_decl, /*comma,*/
		keyword_dim, bracket_open, int_constants, bracket_close }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto dim_node = std::dynamic_pointer_cast<ASTNumConstList<t_int>>(args[3]);
		std::vector<std::size_t> dims;
		dims.reserve(dim_node->GetSize());
		for(t_int dim : dim_node->GetValues())
			dims.push_back(static_cast<std::size_t>(dim));

		return std::make_shared<ASTTypeDecl>(SymbolType::INT_ARRAY, dims);
	}));
#endif
	++semanticindex;

	// real array declaration
#ifdef CREATE_PRODUCTION_RULES
	typedecl->AddRule({ real_decl, /*comma,*/
		keyword_dim, bracket_open, sym_int, bracket_close }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto dim_node = std::dynamic_pointer_cast<ASTNumConstList<t_int>>(args[3]);
		std::vector<std::size_t> dims;
		dims.reserve(dim_node->GetSize());
		for(t_int dim : dim_node->GetValues())
			dims.push_back(static_cast<std::size_t>(dim));

		return std::make_shared<ASTTypeDecl>(SymbolType::REAL_ARRAY, dims);
	}));
#endif
	++semanticindex;

	// complex array declaration
#ifdef CREATE_PRODUCTION_RULES
	typedecl->AddRule({ cplx_decl, /*comma,*/
		keyword_dim, bracket_open, sym_int, bracket_close }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto dim_node = std::dynamic_pointer_cast<ASTNumConstList<t_int>>(args[3]);
		std::vector<std::size_t> dims;
		dims.reserve(dim_node->GetSize());
		for(t_int dim : dim_node->GetValues())
			dims.push_back(static_cast<std::size_t>(dim));

		return std::make_shared<ASTTypeDecl>(SymbolType::CPLX_ARRAY, dims);
	}));
#endif
	++semanticindex;

	// string declaration with default size
#ifdef CREATE_PRODUCTION_RULES
	typedecl->AddRule({ str_decl }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this](bool full_match, [[maybe_unused]] const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		return std::make_shared<ASTTypeDecl>(SymbolType::STRING,
			std::vector<std::size_t>{ default_string_size });
	}));
#endif
	++semanticindex;

	// string declaration with given static size
#ifdef CREATE_PRODUCTION_RULES
	typedecl->AddRule({ str_decl, /*comma,*/
		keyword_dim, bracket_open, sym_int, bracket_close }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;

		auto dim_node = std::dynamic_pointer_cast<ASTNumConst<t_int>>(args[3]);
		const t_int dim = dim_node->GetVal();
		return std::make_shared<ASTTypeDecl>(SymbolType::STRING,
			std::vector<std::size_t>{ std::size_t(dim) });
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

/*	// full_argumentlist -> eps
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
	++semanticindex;*/
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
	// function
	// --------------------------------------------------------------------------------
	// function with full argument types and a single return value
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
				sym->is_arg = true;
				sym->argidx = argidx;
				sym->ty = std::get<1>(arg);
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

	// procedure with full argument types and no return value
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
				sym->is_arg = true;
				sym->argidx = argidx;
				sym->ty = std::get<1>(arg);
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

	// function with full argument types and multiple return values
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
				sym->is_arg = true;
				sym->argidx = argidx;
				sym->ty = std::get<1>(arg);
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

	// function with argument identifiers only and a single return value
#ifdef CREATE_PRODUCTION_RULES
	function->AddRule({ keyword_func, ident /*1*/,
		bracket_open, full_identlist /*3*/, bracket_close,
		keyword_result, bracket_open, typedecl /*7*/, bracket_close,
		statements /*9*/, keyword_end, keyword_func }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	// fill in missing type infos for arguments using symbol table
	auto update_func_arg_types = [this](std::shared_ptr<ASTArgNames> args)
	{
		for(std::size_t idx = 0; idx < args->GetNumArgs(); ++idx)
		{
			const t_str& ident = args->GetArgIdent(idx);
			const Symbol *sym = m_context.FindScopedSymbol(ident);
			if(!sym)
			{
				std::cerr << "Cannot find symbol \"" << ident << "\"." << std::endl;
				return;
			}
			args->SetArgType(idx, sym->ty);
			args->SetArgDims(idx, sym->dims);
		}
	};

	rules.emplace(std::make_pair(semanticindex,
	[this, update_func_arg_types](
		bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
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
				sym->is_arg = true;
				sym->argidx = argidx;
				//sym->ty = std::get<1>(arg);
				//sym->dims = std::get<2>(arg);
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

		// fill in missing type infos for arguments
		update_func_arg_types(funcargs);

		auto res = std::make_shared<ASTFunc>(
			funcname->GetVal(), rettype, funcargs, funcblock);
		m_context.LeaveScope(funcname->GetVal());
		return res;
	}));
#endif
	++semanticindex;

	// procedure with argument identifiers only and no return value
#ifdef CREATE_PRODUCTION_RULES
	function->AddRule({ keyword_procedure, ident /*1*/,
		bracket_open, full_identlist /*3*/, bracket_close,
		statements /*5*/, keyword_end, keyword_func }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this, update_func_arg_types](
		bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
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
				sym->is_arg = true;
				sym->argidx = argidx;
				//sym->ty = std::get<1>(arg);
				//sym->dims = std::get<2>(arg);
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

		// fill in missing type infos for arguments
		update_func_arg_types(funcargs);

		auto res = std::make_shared<ASTFunc>(
			funcname->GetVal(), rettype, funcargs, funcblock);
		m_context.LeaveScope(funcname->GetVal());
		return res;
	}));
#endif
	++semanticindex;

	// function with argument identifiers only and multiple return values
#ifdef CREATE_PRODUCTION_RULES
	function->AddRule({ keyword_func, ident /*1*/,
		bracket_open, full_identlist /*3*/, bracket_close,
		keyword_results, bracket_open, typelist /*7*/, bracket_close,
		statements /*9*/, keyword_end, keyword_func }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this, update_func_arg_types](
		bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
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
				sym->is_arg = true;
				sym->argidx = argidx;
				//sym->ty = std::get<1>(arg);
				//sym->dims = std::get<2>(arg);
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

		// fill in missing type infos for arguments
		update_func_arg_types(funcargs);

		auto res = std::make_shared<ASTFunc>(
			funcname->GetVal(), rettype, funcargs, funcblock, retargs);
		m_context.LeaveScope(funcname->GetVal());
		return res;
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// expression
	// --------------------------------------------------------------------------------
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
			// TODO: move this check into semantics.cpp, as only the functions that have
			// already been parsed are registered at this point (so e.g. no recursive ones)
			std::cerr << "Cannot (yet) find function \"" << funcname << "\"." << std::endl;
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
			// TODO: move this check into semantics.cpp, as only the functions that have
			// already been parsed are registered at this point (so e.g. no recursive ones)
			std::cerr << "Cannot (yet) find function \"" << funcname << "\"." << std::endl;
		}

		auto funcargs = std::dynamic_pointer_cast<ASTExprList>(args[2]);
		return std::make_shared<ASTCall>(funcname, funcargs);
	}));
#endif
	++semanticindex;
	// --------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------
	// statement
	// --------------------------------------------------------------------------------
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
	// --------------------------------------------------------------------------------
}
