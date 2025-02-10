/**
 * helper functions
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date july-2022
 * @license: see 'LICENSE.GPL' file
 */

#ifndef __MUF_HELPERS__
#define __MUF_HELPERS__


#include <chrono>


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
