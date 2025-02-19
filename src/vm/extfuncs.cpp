/**
 * external functions
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 3-july-2022
 * @license see 'LICENSE.GPL' file
 */

#include "vm.h"


/**
 * call external function
 */
VM::t_data VM::CallExternal(const t_str& func_name)
{
	t_data retval;

	if(m_debug)
	{
		std::cout << "calling external function \"" << func_name << "\""
			//<< " with " << num_args << " arguments."
			<< "." << std::endl;
	}

	if(func_name == "abs" || func_name == "fabs" || func_name == "norm")
	{
		t_data dat = PopData();

		if(dat.index() == m_realidx)
		{
			t_real arg = std::get<m_realidx>(dat);
			if(arg < t_real(0))
				arg = -arg;
			retval = t_data{std::in_place_index<m_realidx>, arg};
		}
		else if(dat.index() == m_intidx)
		{
			t_real arg = std::get<m_realidx>(dat);
			if(arg < 0)
				arg = -arg;
			retval = t_data{std::in_place_index<m_intidx>, arg};
		}
		else if(dat.index() == m_vecidx)
		{	// 2-norm for vectors
			t_vec arg = std::get<m_vecidx>(dat);
			t_real len = m::norm<t_vec>(arg);
			retval = t_data{std::in_place_index<m_realidx>, len};
		}
		else
		{
			// keep original data for other types
			retval = dat;
		}
	}
	else if(func_name == "sqrt")
	{
		OpCast<m_realidx>();
		t_real arg = std::get<m_realidx>(PopData());

		retval = t_data{std::in_place_index<m_realidx>, std::sqrt(arg)};
	}
	else if(func_name == "pow")
	{
		OpCast<m_realidx>();
		t_real arg1 = std::get<m_realidx>(PopData());
		OpCast<m_realidx>();
		t_real arg2 = std::get<m_realidx>(PopData());

		retval = t_data{std::in_place_index<m_realidx>, std::pow(arg1, arg2)};
	}
	else if(func_name == "sin")
	{
		OpCast<m_realidx>();
		t_real arg = std::get<m_realidx>(PopData());

		retval = t_data{std::in_place_index<m_realidx>, std::sin(arg)};
	}
	else if(func_name == "cos")
	{
		OpCast<m_realidx>();
		t_real arg = std::get<m_realidx>(PopData());

		retval = t_data{std::in_place_index<m_realidx>, std::cos(arg)};
	}
	else if(func_name == "tan")
	{
		OpCast<m_realidx>();
		t_real arg = std::get<m_realidx>(PopData());

		retval = t_data{std::in_place_index<m_realidx>, std::tan(arg)};
	}
	else if(func_name == "set_eps")
	{
		OpCast<m_realidx>();
		m_eps = std::get<m_realidx>(PopData());
	}
	else if(func_name == "set_prec")
	{
		OpCast<m_intidx>();
		m_prec = std::get<m_intidx>(PopData());
		std::cout.precision(m_prec);
	}
	else if(func_name == "get_eps")
	{
		retval = t_data{std::in_place_index<m_realidx>, m_eps};
	}
	else if(func_name == "to_str" || func_name == "flt_to_str" || func_name == "int_to_str")
	{
		OpCast<m_stridx>();
	}
	else if(func_name == "print")
	{
		OpCast<m_stridx>();
		const t_str/*&*/ arg = std::get<m_stridx>(PopData());
		std::cout << arg << std::endl;
	}
	else if(func_name == "getflt")
	{
		OpCast<m_stridx>();
		const t_str/*&*/ arg = std::get<m_stridx>(PopData());
		std::cout << arg;
		std::cout.flush();

		t_real val{};
		std::cin >> val;

		retval = t_data{std::in_place_index<m_realidx>, val};
	}
	else if(func_name == "getint")
	{
		OpCast<m_stridx>();
		const t_str/*&*/ arg = std::get<m_stridx>(PopData());
		std::cout << arg;
		std::cout.flush();

		t_int val{};
		std::cin >> val;

		retval = t_data{std::in_place_index<m_intidx>, val};
	}
	else if(func_name == "set_isr")
	{
		OpCast<m_intidx>();
		t_addr num = static_cast<t_addr>(std::get<m_intidx>(PopData()));
		t_addr addr = PopAddress();

		SetISR(num, addr);
	}
	else if(func_name == "sleep")
	{
		OpCast<m_intidx>();
		t_int num = std::get<m_intidx>(PopData());

		std::chrono::milliseconds ms{num};
		std::this_thread::sleep_for(ms);
	}
	else if(func_name == "set_timer")
	{
		OpCast<m_intidx>();
		t_int delay = std::get<m_intidx>(PopData());

		if(delay < 0)
		{
			StopTimer();
		}
		else
		{
			m_timer_ticks = std::chrono::milliseconds{delay};
			StartTimer();
		}
	}
	else if(func_name == "set_debug")
	{
		OpCast<m_intidx>();
		m_debug = (std::get<m_intidx>(PopData()) != 0);
	}

	return retval;
}
