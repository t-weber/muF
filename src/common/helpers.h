/**
 * helper functions
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date july-2022
 * @license see 'LICENSE' file
 */

#ifndef __MUF_HELPERS__
#define __MUF_HELPERS__


#include <chrono>
#include <type_traits>
#include <cmath>

#include "mathlibs/matrix_concepts.h"
#include "mathlibs/matrix_algos.h"



/**
 * val1 ** val2
 */
template<class t_val1, class t_val2>
t_val1 power(const t_val1& val1, const t_val2& val2)
{
	if constexpr(std::is_floating_point_v<t_val1>)
	{
		return std::pow(val1, val2);
	}
	else if constexpr(m::is_complex<t_val1>)
	{
		return std::pow(val1, val2);
	}
	else if constexpr(m::is_quat<t_val1> && m::is_scalar<t_val2>)
	{
		using t_val = typename t_quat::value_type;
		using t_vec = m::vec<t_val>;
		return m::pow<t_quat, t_vec>(val1, val2);
	}
	else if constexpr(std::is_integral_v<t_val1>)
	{
		if(val2 == 0)
			return 1;
		else if(val2 < 0)
			return 0;

		t_val1 result = val1;
		for(t_val1 i = 1; i < val2; ++i)
			result *= val1;
		return result;
	}

	return t_val1{};
}



/**
 * keeps indices inside the given range
 */
template<class t_int>
t_int safe_array_index(t_int idx, t_int size)
{
	idx %= size;
	if(idx < 0)
		idx += size;

	if(idx >= size || idx < 0)
		throw std::runtime_error("Array index out of bounds.");
	return idx;
}



using t_clock = std::chrono::steady_clock;
using t_timepoint = std::chrono::time_point<t_clock>;

template<class t_dur_to>
using dur_cast = decltype([](const auto& dur_from) -> t_dur_to
{
	return std::chrono::duration_cast<t_dur_to>(dur_from);
});



/**
 * get time since start_time
 */
template<class t_real, class t_timepoint>
std::tuple<t_real, std::string>
get_elapsed_time(const t_timepoint& start_time)
{
	using t_duration_ms = std::chrono::duration<t_real, std::ratio<1, 1000>>;
	using t_clock = typename t_timepoint::clock;
	t_duration_ms ms = /*dur_cast<t_duration_ms>()*/(t_clock::now() - start_time);
	t_real run_time = ms.count();

	std::string time_unit = "ms";
	if(run_time >= t_real(1000.))
	{
		using t_duration_s = std::chrono::duration<t_real, std::ratio<1, 1>>;
		t_duration_s s = /*dur_cast<t_duration_s>()*/(ms);
		run_time = s.count();
		time_unit = "s";

		if(run_time >= t_real(60.))
		{
			using t_duration_min = std::chrono::duration<t_real, std::ratio<60, 1>>;
			t_duration_min min = /*dur_cast<t_duration_min>()*/(s);
			run_time = min.count();
			time_unit = "min";
		}
	}

	return std::make_tuple(run_time, time_unit);
}


#endif
