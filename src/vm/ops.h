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

	// casting from complex
	else if(data.index() == m_cplxidx)
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

	// casting from real array
	else if(data.index() == m_realarridx)
	{
		OpCastFromArray<t_to, t_vec_real>(data);
	}

	// casting from int array
	else if(data.index() == m_intarridx)
	{
		OpCastFromArray<t_to, t_vec_int>(data);
	}

	// casting from complex array
	else if(data.index() == m_cplxarridx)
	{
		OpCastFromArray<t_to, t_vec_cplx>(data);
	}
}


/**
 * cast from an array variable type
 */
template<class t_to, class t_vec>
void VM::OpCastFromArray(const t_data& data)
{
        using t_elem = typename t_vec::value_type;
        constexpr const std::size_t vec_idx = GetDataTypeIndex<t_vec>();

	if constexpr(std::is_same_v<std::decay_t<t_to>, t_vec>)
		return;  // don't need to cast to the same type

	const t_vec& val = std::get<vec_idx>(data);

	// convert to string
	if constexpr(std::is_same_v<std::decay_t<t_to>, t_str>)
	{
		std::ostringstream ostr;
		ostr.precision(m_prec);
		ostr << "[ ";
		for(std::size_t i = 0; i < val.size(); ++i)
		{
			t_elem elem = val[i];
			if(m::equals_0<t_elem>(elem, m_eps))
				elem = t_elem{};

			ostr << elem;
			if(i != val.size() - 1)
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
			<< GetDataTypeName(GetDataTypeIndex<t_to>()) << ".";
		throw std::runtime_error(msg.str());
	}
}


/**
 * cast to an array variable type
 */
template<class t_vec_to>
void VM::OpCastToArray(t_addr size)
{
	using t_elem = typename t_vec_to::value_type;
	constexpr const std::size_t vec_idx = GetDataTypeIndex<t_vec_to>();

	//using t_to = std::variant_alternative_t<toidx, t_data>;
	t_data data = TopData();

	// conversion error
	auto throw_err = [&data]()
	{
		std::ostringstream msg;
		msg << "Invalid cast from "
			<< GetDataTypeName(data.index()) << " to "
			<< GetDataTypeName(GetDataTypeIndex<t_vec_to>()) << ".";
		throw std::runtime_error(msg.str());
	};

	// casting from same array type
	if(data.index() == vec_idx)
		return;  // no action needed, TODO: check sizes

	// casting from real
	else if(data.index() == m_realidx)
	{
		t_real val = std::get<m_realidx>(data);
		PopData();

		// set every element of the array to the real value
		t_vec_to vec = m::create<t_vec_to>(size);
		for(t_addr i = 0; i < size; ++i)
			vec[i] = t_elem(val);
		PushData(t_data{std::in_place_index<vec_idx>, vec});
	}

	// casting from int
	else if(data.index() == m_intidx)
	{
		t_int val = std::get<m_intidx>(data);
		PopData();

		// set every element of the array to the int value
		t_vec_to vec = m::create<t_vec_to>(size);
		for(t_addr i = 0; i < size; ++i)
			vec[i] = t_elem(val);
		PushData(t_data{std::in_place_index<vec_idx>, vec});
	}

	// casting from complex scalar
	else if(data.index() == m_cplxidx)
	{
		// casting to complex vector
		if constexpr(std::is_same_v<std::decay_t<t_elem>, t_cplx>)
		{
			t_elem val = std::get<m_cplxidx>(data);
			PopData();

			// set every element of the array to the int value
			t_vec_to vec = m::create<t_vec_to>(size);
			for(t_addr i = 0; i < size; ++i)
				vec[i] = val;
			PushData(t_data{std::in_place_index<vec_idx>, vec});
		}
		else
		{
			throw_err();
		}
	}

	// invalid conversion
	else
	{
		throw_err();
	}
}


/**
 * same-type arithmetic operation
 */
template<class t_val, char op>
t_val VM::OpArithmeticSameType(const t_val& val1, const t_val& val2)
{
	t_val result{};

	// string operators
	if constexpr(std::is_same_v<std::decay_t<t_val>, t_str>)
	{
		if constexpr(op == '+')
			result = val1 + val2;
	}

	// array operators
	else if constexpr(std::is_same_v<std::decay_t<t_val>, t_vec_real> ||
		std::is_same_v<std::decay_t<t_val>, t_vec_int> ||
		std::is_same_v<std::decay_t<t_val>, t_vec_cplx>)
	{
		if constexpr(op == '+')
			result = val1 + val2;
		else if constexpr(op == '-')
			result = val1 - val2;
	}

	// int or real operators
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

	auto dot_prod = []<class t_vec>(const t_data& val1, const t_data& val2)
		-> std::pair<bool, t_data>
	{
		using t_elem = typename t_vec::value_type;
		constexpr const std::size_t vec_idx = GetDataTypeIndex<t_vec>();
		constexpr const std::size_t elem_type_idx = GetDataTypeIndex<t_elem>();

		if(val1.index() != vec_idx || val2.index() != vec_idx || op != '*')
			return std::make_pair(false, t_data{});

		const t_vec& vec1 = std::get<vec_idx>(val1);
		const t_vec& vec2 = std::get<vec_idx>(val2);

		t_elem dot = m::inner<t_vec>(vec1, vec2);
		return std::make_pair(true,
			t_data{std::in_place_index<elem_type_idx>, std::move(dot)});
	};

	auto scale_vec = []<class t_vec>(const t_data& val1, const t_data& val2)
		-> std::pair<bool, t_data>
	{
		using t_elem = typename t_vec::value_type;
		constexpr const std::size_t vec_idx = GetDataTypeIndex<t_vec>();
		constexpr const std::size_t elem_idx = GetDataTypeIndex<t_elem>();

		// vec * scalar
		if(val1.index() == vec_idx && val2.index() == elem_idx && op == '*')
		{
			using namespace m_ops;
			const t_vec& vec = std::get<vec_idx>(val1);
			const t_elem& s = std::get<elem_idx>(val2);

			t_data res = t_data{std::in_place_index<vec_idx>, s * vec};
			return std::make_pair(true, std::move(res));
		}

		// vec / scalar
		else if(val1.index() == vec_idx && val2.index() == elem_idx && op == '/')
		{
			using namespace m_ops;
			const t_vec& vec = std::get<vec_idx>(val1);
			const t_elem& s = std::get<elem_idx>(val2);

			t_data res = t_data{std::in_place_index<vec_idx>, vec / s};
			return std::make_pair(true, std::move(res));
		}

		// scalar * vec
		else if(val2.index() == vec_idx && val1.index() == elem_idx && op == '*')
		{
			using namespace m_ops;
			const t_vec& vec = std::get<vec_idx>(val2);
			const t_elem& s = std::get<elem_idx>(val1);

			t_data res = t_data{std::in_place_index<vec_idx>, s * vec};
			return std::make_pair(true, std::move(res));
		}

		return std::make_pair(false, t_data{});
	};

	// real vector dot product
	if(auto [ ok, res ] = dot_prod.template operator()<t_vec_real>(val1, val2); ok)
		result = std::move(res);

	// int vector dot product
	else if(auto [ ok, res ] = dot_prod.template operator()<t_vec_int>(val1, val2); ok)
		result = std::move(res);

	// complex vector dot product
	else if(auto [ ok, res ] = dot_prod.template operator()<t_vec_cplx>(val1, val2); ok)
		result = std::move(res);

	// scale real vector
	else if(auto [ ok, res ] = scale_vec.template operator()<t_vec_real>(val1, val2); ok)
		result = std::move(res);

	// scale int vector
	else if(auto [ ok, res ] = scale_vec.template operator()<t_vec_int>(val1, val2); ok)
		result = std::move(res);

	// scale complex vector
	else if(auto [ ok, res ] = scale_vec.template operator()<t_vec_cplx>(val1, val2); ok)
		result = std::move(res);

	// same-type operations
	else if(val1.index() == val2.index())
	{
		if(val1.index() == m_realidx)
		{
			result = t_data{std::in_place_index<m_realidx>,
				OpArithmeticSameType<t_real, op>(
					std::get<m_realidx>(val1), std::get<m_realidx>(val2))};
		}
		else if(val1.index() == m_intidx)
		{
			result = t_data{std::in_place_index<m_intidx>,
				OpArithmeticSameType<t_int, op>(
					std::get<m_intidx>(val1), std::get<m_intidx>(val2))};
		}
		else if(val1.index() == m_cplxidx)
		{
			result = t_data{std::in_place_index<m_cplxidx>,
				OpArithmeticSameType<t_cplx, op>(
					std::get<m_cplxidx>(val1), std::get<m_cplxidx>(val2))};
		}
		else if(val1.index() == m_stridx)
		{
			result = t_data{std::in_place_index<m_stridx>,
				OpArithmeticSameType<t_str, op>(
					std::get<m_stridx>(val1), std::get<m_stridx>(val2))};
		}
		else if(val1.index() == m_realarridx)
		{
			result = t_data{std::in_place_index<m_realarridx>,
				OpArithmeticSameType<t_vec_real, op>(
					std::get<m_realarridx>(val1), std::get<m_realarridx>(val2))};
		}
		else if(val1.index() == m_intarridx)
		{
			result = t_data{std::in_place_index<m_intarridx>,
				OpArithmeticSameType<t_vec_int, op>(
					std::get<m_intarridx>(val1), std::get<m_intarridx>(val2))};
		}
		else if(val1.index() == m_cplxarridx)
		{
			result = t_data{std::in_place_index<m_cplxarridx>,
				OpArithmeticSameType<t_vec_cplx, op>(
					std::get<m_cplxarridx>(val1), std::get<m_cplxarridx>(val2))};
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
 * same-type binary operation
 */
template<class t_val, char op>
t_val VM::OpBinarySameType(const t_val& val1, const t_val& val2)
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

	// same-type binary operations
	if(val1.index() == m_intidx)
	{
		result = t_data{std::in_place_index<m_intidx>,
			OpBinarySameType<t_int, op>(
				std::get<m_intidx>(val1), std::get<m_intidx>(val2))};
	}
	else
	{
		throw std::runtime_error("Invalid type in binary operation.");
	}

	PushData(result);
}


/**
 * same-type comparison operation
 */
template<class t_val, OpCode op>
bool VM::OpComparisonSameType(const t_val& val1, const t_val& val2)
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
	else if constexpr(std::is_same_v<std::decay_t<t_val>, t_vec_real> ||
		std::is_same_v<std::decay_t<t_val>, t_vec_int>)
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

	// same type comparison operations
	if(val1.index() == m_realidx)
	{
		result = OpComparisonSameType<t_real, op>(
			std::get<m_realidx>(val1), std::get<m_realidx>(val2));
	}
	else if(val1.index() == m_intidx)
	{
		result = OpComparisonSameType<t_int, op>(
			std::get<m_intidx>(val1), std::get<m_intidx>(val2));
	}
	else if(val1.index() == m_stridx)
	{
		result = OpComparisonSameType<t_str, op>(
			std::get<m_stridx>(val1), std::get<m_stridx>(val2));
	}
	else if(val1.index() == m_realarridx)
	{
		result = OpComparisonSameType<t_vec_real, op>(
			std::get<m_realarridx>(val1), std::get<m_realarridx>(val2));
	}
	else if(val1.index() == m_intarridx)
	{
		result = OpComparisonSameType<t_vec_int, op>(
			std::get<m_intarridx>(val1), std::get<m_intarridx>(val2));
	}
	else
	{
		throw std::runtime_error("Invalid type in comparison operation.");
	}

	PushBool(result);
}


#endif
