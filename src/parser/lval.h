/**
 * value types for lexer
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 26-nov-2022
 * @license see 'LICENSE' file
 */

#ifndef __MUF_LVAL_H__
#define __MUF_LVAL_H__


#include <variant>
#include <optional>
#include <string>
#include <cstdint>


using t_real = double;
using t_int = std::int64_t;
using t_bool = bool;

using t_lval = std::optional<std::variant<t_real, t_int, t_bool, std::string>>;


#endif
