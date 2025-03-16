/**
 * basic data types
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 10-july-2022
 * @license see 'LICENSE' file
 */

#ifndef __MUF_LVAL_TYPES_H__
#define __MUF_LVAL_TYPES_H__


#include <cstdint>
#include <complex>
#include <string>

#include "mathlibs/matrix_algos.h"
#include "mathlibs/matrix_conts.h"


using t_str = std::string;
using t_real = double;
using t_int = std::int64_t;
using t_cplx = std::complex<t_real>;
using t_quat = m::quat<t_real>;


#endif
