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
	// function
	// --------------------------------------------------------------------------------
	// procedure with argument identifiers and no return value
#ifdef CREATE_PRODUCTION_RULES
	function->AddRule({ keyword_procedure, ident /*1*/,
		bracket_open, full_identlist /*3*/, bracket_close,
		statements /*5*/, keyword_end, keyword_func }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	// fill in missing type infos for arguments using symbol table
	auto update_func_arg_types = [this](std::shared_ptr<ASTArgNames> args)
	{
		for(std::size_t idx = 0; idx < args->GetNumArgs(); ++idx)
		{
			const t_str& ident = args->GetArgIdent(idx);
			const SymbolPtr sym = m_context.FindScopedSymbol(ident);
			if(!sym || !sym->is_arg)
			{
				std::cerr << "Cannot find argument symbol \""
					<< ident << "\"." << std::endl;
				return;
			}
			args->SetArgType(idx, sym->ty);
			args->SetArgDims(idx, sym->dims);
		}
	};

	// fill in missing type infos for return values using symbol table
	auto update_func_ret_types = [this](std::shared_ptr<ASTArgNames> args)
	{
		for(std::size_t idx = 0; idx < args->GetNumArgs(); ++idx)
		{
			const t_str& ident = args->GetArgIdent(idx);
			const SymbolPtr sym = m_context.FindScopedSymbol(ident);
			if(!sym || !sym->is_ret)
			{
				std::cerr << "Cannot find return symbol \""
					<< ident << "\"." << std::endl;
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
				SymbolPtr sym =
					m_context.AddScopedSymbol(std::get<0>(arg));
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

		auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[1]);
		auto funcargs = std::dynamic_pointer_cast<ASTArgNames>(args[3]);
		auto funcblock = std::dynamic_pointer_cast<ASTStmts>(args[5]);

		// fill in missing type infos for arguments
		update_func_arg_types(funcargs);

		auto res = std::make_shared<ASTFunc>(funcname->GetVal(), funcargs, funcblock);
		m_context.LeaveScope(funcname->GetVal());
		return res;
	}));
#endif
	++semanticindex;

	// function with argument identifiers and return values
#ifdef CREATE_PRODUCTION_RULES
	function->AddRule({ keyword_func, ident /*1*/,
		bracket_open, full_identlist /*3*/, bracket_close,
		keyword_results, bracket_open, full_identlist /*7*/, bracket_close,
		statements /*9*/, keyword_end, keyword_func }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[this, update_func_arg_types, update_func_ret_types](
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
			auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[1]);
			auto funcargs = std::dynamic_pointer_cast<ASTArgNames>(args[3]);
			auto retargs = std::dynamic_pointer_cast<ASTArgNames>(args[7]);

			// register argument variables
			std::size_t argidx = 0;
			for(const auto& arg : funcargs->GetArgs())
			{
				SymbolPtr sym =
					m_context.AddScopedSymbol(std::get<0>(arg));
				sym->is_arg = true;
				sym->argidx = argidx;
				//sym->ty = std::get<1>(arg);
				//sym->dims = std::get<2>(arg);
				++argidx;
			}

			// register return variables
			std::size_t retidx = 0;
			for(const auto& arg : retargs->GetArgs())
			{
				SymbolPtr sym =
					m_context.AddScopedSymbol(std::get<0>(arg));
				sym->is_ret = true;
				sym->retidx = retidx;
				//sym->ty = std::get<1>(arg);
				//sym->dims = std::get<2>(arg);
				++retidx;
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

		auto funcname = std::dynamic_pointer_cast<ASTStrConst>(args[1]);
		auto funcargs = std::dynamic_pointer_cast<ASTArgNames>(args[3]);
		auto retargs = std::dynamic_pointer_cast<ASTArgNames>(args[7]);
		auto funcblock = std::dynamic_pointer_cast<ASTStmts>(args[9]);

		// fill in missing type infos for arguments and return variables
		update_func_arg_types(funcargs);
		update_func_ret_types(retargs);

		auto res = std::make_shared<ASTFunc>(
			funcname->GetVal(), funcargs, funcblock, retargs);
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
		const SymbolPtr sym = m_context.GetSymbols().FindSymbol(funcname);

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
		const SymbolPtr sym = m_context.GetSymbols().FindSymbol(funcname);

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

	// return
#ifdef CREATE_PRODUCTION_RULES
	statement->AddRule({ keyword_ret/*, stmt_end*/ }, semanticindex);
#endif
#ifdef CREATE_SEMANTIC_RULES
	rules.emplace(std::make_pair(semanticindex,
	[](bool full_match, [[maybe_unused]] const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
	{
		if(!full_match)
			return nullptr;
		return std::make_shared<ASTReturn>(nullptr, true);
	}));
#endif
	++semanticindex;

//	// (multi-)return
//#ifdef CREATE_PRODUCTION_RULES
//	statement->AddRule({ keyword_ret, expressions/*, stmt_end*/ }, semanticindex);
//#endif
//#ifdef CREATE_SEMANTIC_RULES
//	rules.emplace(std::make_pair(semanticindex,
//	[](bool full_match, const lalr1::t_semanticargs& args, [[maybe_unused]] lalr1::t_astbaseptr retval) -> lalr1::t_astbaseptr
//	{
//		if(!full_match)
//			return nullptr;
//		auto terms = std::dynamic_pointer_cast<ASTExprList>(args[1]);
//		return std::make_shared<ASTReturn>(terms, false);
//	}));
//#endif
//	++semanticindex;
	// --------------------------------------------------------------------------------
}
