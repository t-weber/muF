/**
 * zero-address code vm -- operators
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 8-jun-2022
 * @license see 'LICENSE' file
 */

#ifndef __0ACVM_OPS_H__
#define __0ACVM_OPS_H__


#include <type_traits>
#include <variant>
#include <sstream>
#include <string>
#include <cstring>
#include <cmath>


/**
 * cast from one variable type to the other
 */
template<std::size_t toidx>
void VM::OpCast()
{
	using t_to = std::variant_alternative_t<toidx, t_data>;
	t_data data = TopData();

	// casting from real
	if(data.index() == m_realidx)
	{
		if constexpr(std::is_same_v<std::decay_t<t_to>, t_real>)
			return;  // don't need to cast to the same type

		t_real val = std::get<m_realidx>(data);

		// convert to string
		if constexpr(std::is_same_v<std::decay_t<t_to>, t_str>)
		{
			if(m::equals_0<t_real>(val, m_eps))
				val = t_real(0);

			std::ostringstream ostr;
			ostr.precision(m_prec);
			ostr << val;
			PopData();
			PushData(t_data{std::in_place_index<m_stridx>, ostr.str()});
		}

		// convert to primitive type
		else
		{
			PopData();
				PushData(t_data{std::in_place_index<toidx>,
				static_cast<t_to>(val)});
		}
	}

	// casting from int
	else if(data.index() == m_intidx)
	{
		if constexpr(std::is_same_v<std::decay_t<t_to>, t_int>)
			return;  // don't need to cast to the same type

		t_int val = std::get<m_intidx>(data);

		// convert to string
		if constexpr(std::is_same_v<std::decay_t<t_to>, t_str>)
		{
			std::ostringstream ostr;
			ostr.precision(m_prec);
			ostr << val;
			PopData();
			PushData(t_data{std::in_place_index<m_stridx>, ostr.str()});
		}

		// convert to primitive type
		else
		{
			PopData();
			PushData(t_data{std::in_place_index<toidx>,
				static_cast<t_to>(val)});
		}
	}

	// casting from bool
	else if(data.index() == m_boolidx)
	{
		if constexpr(std::is_same_v<std::decay_t<t_to>, t_bool>)
			return;  // don't need to cast to the same type

		bool val = std::get<m_boolidx>(data) != 0;

		// convert to string
		if constexpr(std::is_same_v<std::decay_t<t_to>, t_str>)
		{
			std::ostringstream ostr;
			ostr.precision(m_prec);
			ostr << std::boolalpha << val;
			PopData();
			PushData(t_data{std::in_place_index<m_stridx>, ostr.str()});
		}

		// convert to primitive type
		else
		{
			PopData();
			PushData(t_data{std::in_place_index<toidx>,
				static_cast<t_to>(val)});
		}
	}

	// casting from complex
	if(data.index() == m_cplxidx)
	{
		if constexpr(std::is_same_v<std::decay_t<t_to>, t_cplx>)
			return;  // don't need to cast to the same type

		t_cplx val = std::get<m_cplxidx>(data);

		// convert to string
		if constexpr(std::is_same_v<std::decay_t<t_to>, t_str>)
		{
			t_real real = val.real();
			t_real imag = val.imag();

			if(m::equals_0<t_real>(real, m_eps))
				real = t_real(0);
			if(m::equals_0<t_real>(imag, m_eps))
				imag = t_real(0);

			std::ostringstream ostr;
			ostr.precision(m_prec);
			ostr << "(" << real << ", " << imag << ")";
			PopData();
			PushData(t_data{std::in_place_index<m_stridx>, ostr.str()});
		}

		// convert to primitive type
		else
		{
			std::ostringstream msg;
			msg << "Invalid cast from complex to "
				<< GetDataTypeName(toidx) << ".";
			throw std::runtime_error(msg.str());
		}
	}

	// casting from string
	else if(data.index() == m_stridx)
	{
		if constexpr(std::is_same_v<std::decay_t<t_to>, t_str>)
			return;  // don't need to cast to the same type

		const t_str& val = std::get<m_stridx>(data);

		t_to conv_val{};
		std::istringstream{val} >> conv_val;
		PopData();
		PushData(t_data{std::in_place_index<toidx>, conv_val});
	}

	// casting from array
	else if(data.index() == m_realarridx)
	{
		if constexpr(std::is_same_v<std::decay_t<t_to>, t_vec_real>)
			return;  // don't need to cast to the same type

		const t_vec_real& val = std::get<m_realarridx>(data);

		// convert to string
		if constexpr(std::is_same_v<std::decay_t<t_to>, t_str>)
		{
			std::ostringstream ostr;
			ostr.precision(m_prec);
			ostr << "[ ";
			for(std::size_t i = 0; i < val.size(); ++i)
			{
				t_real elem = val[i];
				if(m::equals_0<t_real>(elem, m_eps))
					elem = t_real(0);

				ostr << elem;
				if(i != val.size()-1)
					ostr << ", ";
			}
			ostr << " ]";

			PopData();
			PushData(t_data{std::in_place_index<m_stridx>, ostr.str()});
		}
		else
		{
			std::ostringstream msg;
			msg << "Invalid cast from array to "
				<< GetDataTypeName(toidx) << ".";
			throw std::runtime_error(msg.str());
		}
	}
}


/**
 * cast to an array variable type
 */
template<std::size_t toidx>
void VM::OpArrayCast(t_addr size)
{
	//using t_to = std::variant_alternative_t<toidx, t_data>;
	t_data data = TopData();

	// casting to array
	if constexpr(toidx == m_realarridx)
	{
		// casting from array
		if(data.index() == m_realarridx)
			return;  // no action needed, TODO: check sizes

		// casting from real
		else if(data.index() == m_realidx)
		{
			t_real val = std::get<m_realidx>(data);
			PopData();

			// set every element of the array to the real value
			t_vec_real vec = m::create<t_vec_real>(size);
			for(t_addr i = 0; i < size; ++i)
				vec[i] = val;
			PushData(t_data{std::in_place_index<m_realarridx>, vec});
		}

		// casting from int
		else if(data.index() == m_intidx)
		{
			t_real val = std::get<m_intidx>(data);
			PopData();

			// set every element of the array to the int value
			t_vec_real vec = m::create<t_vec_real>(size);
			for(t_addr i = 0; i < size; ++i)
				vec[i] = t_real(val);
			PushData(t_data{std::in_place_index<m_realarridx>, vec});
		}
	}
}


/**
 * arithmetic operation
 */
template<class t_val, char op>
t_val VM::OpArithmetic(const t_val& val1, const t_val& val2)
{
	t_val result{};

	// string operators
	if constexpr(std::is_same_v<std::decay_t<t_val>, t_str>)
	{
		if constexpr(op == '+')
			result = val1 + val2;
	}

	// array operators
	else if constexpr(std::is_same_v<std::decay_t<t_val>, t_vec_real>)
	{
		if constexpr(op == '+')
			result = val1 + val2;
		else if constexpr(op == '-')
			result = val1 - val2;
	}

	// int / real operators
	else if constexpr(std::is_same_v<std::decay_t<t_val>, t_int> ||
		std::is_same_v<std::decay_t<t_val>, t_real>)
	{
		if constexpr(op == '+')
			result = val1 + val2;
		else if constexpr(op == '-')
			result = val1 - val2;
		else if constexpr(op == '*')
			result = val1 * val2;
		else if constexpr(op == '/')
			result = val1 / val2;
		else if constexpr(op == '%' && std::is_integral_v<t_val>)
			result = val1 % val2;
		else if constexpr(op == '%' && std::is_floating_point_v<t_val>)
			result = std::fmod(val1, val2);
		else if constexpr(op == '^' /*&& std::is_floating_point_v<t_val>*/)
			result = pow<t_val>(val1, val2);
	}

	// complex operators
	else if constexpr(std::is_same_v<std::decay_t<t_val>, t_cplx>)
	{
		if constexpr(op == '+')
			result = val1 + val2;
		else if constexpr(op == '-')
			result = val1 - val2;
		else if constexpr(op == '*')
			result = val1 * val2;
		else if constexpr(op == '/')
			result = val1 / val2;
		else if constexpr(op == '^')
			result = pow<t_val>(val1, val2);
	}

	return result;
}


/**
 * arithmetic operation
 */
template<char op>
void VM::OpArithmetic()
{
	t_data val2 = PopData();
	t_data val1 = PopData();
	t_data result;

	// dot product
	if(val1.index() == m_realarridx && val2.index() == m_realarridx && op == '*')
	{
		const t_vec_real& vec1 = std::get<m_realarridx>(val1);
		const t_vec_real& vec2 = std::get<m_realarridx>(val2);
		t_real dot = m::inner<t_vec_real>(vec1, vec2);
		result = t_data{std::in_place_index<m_realidx>, dot};
	}

	// scale array
	else if(val1.index() == m_realarridx && val2.index() == m_realidx && op == '*')
	{
		using namespace m_ops;
		const t_vec_real& vec = std::get<m_realarridx>(val1);
		const t_real s = std::get<m_realidx>(val2);
		result = t_data{std::in_place_index<m_realarridx>, s * vec};
	}

	// scale array
	else if(val1.index() == m_realarridx && val2.index() == m_realidx && op == '/')
	{
		using namespace m_ops;
		const t_vec_real& vec = std::get<m_realarridx>(val1);
		const t_real s = std::get<m_realidx>(val2);
		result = t_data{std::in_place_index<m_realarridx>, vec / s};
	}

	// scale array
	else if(val2.index() == m_realarridx && val1.index() == m_realidx && op == '*')
	{
		using namespace m_ops;
		const t_vec_real& vec = std::get<m_realarridx>(val2);
		const t_real s = std::get<m_realidx>(val1);
		result = t_data{std::in_place_index<m_realarridx>, s * vec};
	}

	// same-type operations
	else if(val1.index() == val2.index())
	{
		if(val1.index() == m_realidx)
		{
			result = t_data{std::in_place_index<m_realidx>, OpArithmetic<t_real, op>(
				std::get<m_realidx>(val1), std::get<m_realidx>(val2))};
		}
		else if(val1.index() == m_intidx)
		{
			result = t_data{std::in_place_index<m_intidx>, OpArithmetic<t_int, op>(
				std::get<m_intidx>(val1), std::get<m_intidx>(val2))};
		}
		else if(val1.index() == m_cplxidx)
		{
			result = t_data{std::in_place_index<m_cplxidx>, OpArithmetic<t_cplx, op>(
				std::get<m_cplxidx>(val1), std::get<m_cplxidx>(val2))};
		}
		else if(val1.index() == m_stridx)
		{
			result = t_data{std::in_place_index<m_stridx>, OpArithmetic<t_str, op>(
				std::get<m_stridx>(val1), std::get<m_stridx>(val2))};
		}
		else if(val1.index() == m_realarridx)
		{
			result = t_data{std::in_place_index<m_realarridx>, OpArithmetic<t_vec_real, op>(
				std::get<m_realarridx>(val1), std::get<m_realarridx>(val2))};
		}
	}
	else
	{
		std::ostringstream err;
		err << "Unknown arithmetic operation. "
			<< "Types: " << GetDataTypeName(val1.index())
			<< ", " << GetDataTypeName(val2.index()) << ".";
		throw std::runtime_error(err.str());
	}

	PushData(result);
}


/**
 * logical operation
 */
template<char op>
void VM::OpLogical()
{
	bool val2 = PopBool();
	bool val1 = PopBool();

	bool result = false;

	if constexpr(op == '&')
		result = val1 && val2;
	else if constexpr(op == '|')
		result = val1 || val2;
	else if constexpr(op == '^')
		result = val1 ^ val2;

	PushBool(result);
}


/**
 * binary operation
 */
template<class t_val, char op>
t_val VM::OpBinary(const t_val& val1, const t_val& val2)
{
	t_val result{};

	// int operators
	if constexpr(std::is_same_v<std::decay_t<t_int>, t_int>)
	{
		if constexpr(op == '&')
			result = val1 & val2;
		else if constexpr(op == '|')
			result = val1 | val2;
		else if constexpr(op == '^')
			result = val1 ^ val2;
		else if constexpr(op == '<')  // left shift
			result = val1 << val2;
		else if constexpr(op == '>')  // right shift
			result = val1 >> val2;
		else if constexpr(op == 'l')  // left rotation
			result = static_cast<t_int>(std::rotl<t_uint>(val1, static_cast<int>(val2)));
		else if constexpr(op == 'r')  // right rotation
			result = static_cast<t_int>(std::rotr<t_uint>(val1, static_cast<int>(val2)));
	}

	return result;
}


/**
 * binary operation
 */
template<char op>
void VM::OpBinary()
{
	t_data val2 = PopData();
	t_data val1 = PopData();

	if(val1.index() != val2.index())
	{
		std::ostringstream err;
		err << "Type mismatch in binary operation. "
			<< "Types: " << GetDataTypeName(val1.index())
			<< ", " << GetDataTypeName(val2.index()) << ".";
		throw std::runtime_error(err.str());
	}

	t_data result;

	if(val1.index() == m_intidx)
	{
		result = t_data{std::in_place_index<m_intidx>, OpBinary<t_int, op>(
			std::get<m_intidx>(val1), std::get<m_intidx>(val2))};
	}
	else
	{
		throw std::runtime_error("Invalid type in binary operation.");
	}

	PushData(result);
}


/**
 * comparison operation
 */
template<class t_val, OpCode op>
bool VM::OpComparison(const t_val& val1, const t_val& val2)
{
	bool result = false;

	// string comparison
	if constexpr(std::is_same_v<std::decay_t<t_val>, t_str>)
	{
		if constexpr(op == OpCode::EQU)
			result = (val1 == val2);
		else if constexpr(op == OpCode::NEQU)
			result = (val1 != val2);
	}

	// array comparison
	else if constexpr(std::is_same_v<std::decay_t<t_val>, t_vec_real>)
	{
		if constexpr(op == OpCode::EQU)
			result = m::equals(val1, val2, m_eps);
		else if constexpr(op == OpCode::NEQU)
			result = !m::equals(val1, val2, m_eps);
	}

	// integer / real comparison
	else
	{
		if constexpr(op == OpCode::GT)
			result = (val1 > val2);
		else if constexpr(op == OpCode::LT)
			result = (val1 < val2);
		else if constexpr(op == OpCode::GEQU)
			result = (val1 >= val2);
		else if constexpr(op == OpCode::LEQU)
			result = (val1 <= val2);
		else if constexpr(op == OpCode::EQU)
		{
			if constexpr(std::is_same_v<std::decay_t<t_val>, t_real>)
				result = (std::abs(val1 - val2) <= m_eps);
			else
				result = (val1 == val2);
		}
		else if constexpr(op == OpCode::NEQU)
		{
			if constexpr(std::is_same_v<std::decay_t<t_val>, t_real>)
				result = (std::abs(val1 - val2) > m_eps);
			else
				result = (val1 != val2);
		}
	}

	return result;
}


/**
 * comparison operation
 */
template<OpCode op>
void VM::OpComparison()
{
	t_data val2 = PopData();
	t_data val1 = PopData();

	if(val1.index() != val2.index())
	{
		std::ostringstream err;
		err << "Type mismatch in comparison operation. "
			<< "Types: " << GetDataTypeName(val1.index())
			<< ", " << GetDataTypeName(val2.index()) << ".";
		throw std::runtime_error(err.str());
	}

	bool result = false;

	if(val1.index() == m_realidx)
	{
		result = OpComparison<t_real, op>(
			std::get<m_realidx>(val1), std::get<m_realidx>(val2));
	}
	else if(val1.index() == m_intidx)
	{
		result = OpComparison<t_int, op>(
			std::get<m_intidx>(val1), std::get<m_intidx>(val2));
	}
	else if(val1.index() == m_stridx)
	{
		result = OpComparison<t_str, op>(
			std::get<m_stridx>(val1), std::get<m_stridx>(val2));
	}
	else if(val1.index() == m_realarridx)
	{
		result = OpComparison<t_vec_real, op>(
			std::get<m_realarridx>(val1), std::get<m_realarridx>(val2));
	}
	else
	{
		throw std::runtime_error("Invalid type in comparison operation.");
	}

	PushBool(result);
}


#endif
