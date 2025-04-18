/**
 * common front-end functions
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 20-dec-19
 * @license see 'LICENSE' file
 */

#ifndef __MUF_MAIN_H__
#define __MUF_MAIN_H__


#include "context.h"
#include <cstdint>


/**
 * registers external runtime functions which should be available to the compiler
 */
template<class t_real, class t_int>
void add_ext_funcs(ParserContext& ctx, bool skip_some = false)
{
	// real functions
	if constexpr(std::is_same_v<std::decay_t<t_real>, float>)
	{
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "pow", "powf",
			SymbolType::REAL, {SymbolType::REAL, SymbolType::REAL});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "exp", "expf",
			SymbolType::REAL, {SymbolType::REAL});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "sin", "sinf",
			SymbolType::REAL, {SymbolType::REAL});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "cos", "cosf",
			SymbolType::REAL, {SymbolType::REAL});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "sqrt", "sqrtf",
			SymbolType::REAL, {SymbolType::REAL});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "fabs", "fabsf",
			SymbolType::REAL, {SymbolType::REAL});
	}
	else if constexpr(std::is_same_v<std::decay_t<t_real>, double>)
	{
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "pow", "pow",
			SymbolType::REAL, {SymbolType::REAL, SymbolType::REAL});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "exp", "exp",
			SymbolType::REAL, {SymbolType::REAL});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "sin", "sin",
			SymbolType::REAL, {SymbolType::REAL});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "cos", "cos",
			SymbolType::REAL, {SymbolType::REAL});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "sqrt", "sqrt",
			SymbolType::REAL, {SymbolType::REAL});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "fabs", "fabs",
			SymbolType::REAL, {SymbolType::REAL});
	}
	else if constexpr(std::is_same_v<std::decay_t<t_real>, long double>)
	{
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "pow", "powl",
			SymbolType::REAL, {SymbolType::REAL, SymbolType::REAL});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "exp", "expl",
			SymbolType::REAL, {SymbolType::REAL});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "sin", "sinl",
			SymbolType::REAL, {SymbolType::REAL});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "cos", "cosl",
			SymbolType::REAL, {SymbolType::REAL});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "sqrt", "sqrtl",
			SymbolType::REAL, {SymbolType::REAL});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "fabs", "fabsl",
			SymbolType::REAL, {SymbolType::REAL});
	}

	// int functions
	if constexpr(std::is_same_v<std::decay_t<t_real>, std::int32_t>)
	{
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "abs", "abs",
			SymbolType::INT, {SymbolType::INT});
	}
	else if constexpr(std::is_same_v<std::decay_t<t_real>, std::int64_t>)
	{
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "abs", "labs",
			SymbolType::INT, {SymbolType::INT});
	}

	ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "strlen", "strlen",
		SymbolType::INT, {SymbolType::STRING});

	ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "set_debug", "set_debug",
		SymbolType::VOID, {SymbolType::INT});

	ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "sleep", "sleep",
		SymbolType::VOID, {SymbolType::INT});
	ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "set_timer", "set_timer",
		SymbolType::VOID, {SymbolType::INT});

	ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "set_eps", "set_eps",
		SymbolType::VOID, {SymbolType::REAL});
	ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "get_eps", "get_eps",
		SymbolType::REAL, {});

	// functions that could also be declared as internals
	if(!skip_some)
	{
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "write", "write",
			SymbolType::VOID, {SymbolType::STRING});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "write_no_cr", "write_no_cr",
			SymbolType::VOID, {SymbolType::STRING});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "read_real", "read_real",
			SymbolType::REAL, {SymbolType::STRING});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "read_integer", "read_integer",
			SymbolType::INT, {SymbolType::STRING});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "real_to_string", "real_to_string",
			SymbolType::VOID, {SymbolType::REAL, SymbolType::STRING, SymbolType::INT});
		ctx.GetSymbols().AddExtFunc(ctx.GetScopeName(), "integer_to_string", "integer_to_string",
			SymbolType::VOID, {SymbolType::INT, SymbolType::STRING, SymbolType::INT});
	}
}


#endif
