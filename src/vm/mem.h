/**
 * zero-address code vm -- memory operations
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 8-jun-2022
 * @license see 'LICENSE' file
 */

#ifndef __0ACVM_MEM_H__
#define __0ACVM_MEM_H__


#include <type_traits>
#include <vector>
#include <iostream>
#include <sstream>


/**
 * pop an array from the stack
 * an array consists of an t_addr giving the length
 * following by the array elements
 */
template<class t_vec>
t_vec VM::PopArray(bool raw_elems)
{
	using t_elem = typename t_vec::value_type;
	constexpr const t_addr elem_size = GetDataTypeSize<t_elem>();
	constexpr const std::size_t vec_idx = GetDataTypeIndex<t_vec>();
	constexpr const std::size_t elem_type_idx = GetDataTypeIndex<t_elem>();

	// raw real array and array size follow without descriptors
	if(raw_elems)
	{
		t_addr num_elems = PopRaw<t_addr, m_addrsize>();
		CheckMemoryBounds(m_sp, num_elems*elem_size);

		t_elem* begin = reinterpret_cast<t_elem*>(m_mem.get() + m_sp);
		t_vec vec(begin, num_elems);
		m_sp += num_elems*elem_size;

		if(m_zeropoppedvals)
		{
			constexpr bool is_pod = std::is_trivial_v<t_elem> && std::is_standard_layout_v<t_elem>;
			if constexpr(is_pod)
			{
				std::memset(begin, 0, num_elems*elem_size);
			}
			else if constexpr(!is_pod)
			{
				for(t_addr i = 0; i < num_elems; ++i)
					*(begin + i) = t_elem{};
			}
		}

		return vec;
	}

	// individual elements and array size with descriptor are on the stack
	else
	{
		t_addr num_elems = PopAddress();
		t_vec vec(num_elems);

		for(t_addr i = 0; i < num_elems; ++i)
		{
			t_data val = PopData();
			if(val.index() != elem_type_idx)
				throw std::runtime_error("Wrong element type for array.");

			t_elem elem = std::get<elem_type_idx>(val);
			vec[num_elems - i - 1] = elem;
		}

		if(m_debug)
		{
			using namespace m_ops;
			std::cout << "popped non-raw " << GetDataTypeName(vec_idx) << " "
				<< vec << "." << std::endl;
		}

		return vec;
	}
}


/**
 * get the array on top of the stack
 */
template<class t_vec>
t_vec VM::TopArray(typename VM::t_addr sp_offs) const
{
	using t_elem = typename t_vec::value_type;
	constexpr const t_addr elem_size = GetDataTypeSize<t_elem>();

	t_addr num_elems = TopRaw<t_addr, m_addrsize>(sp_offs);
	t_addr addr = m_sp + sp_offs + m_addrsize;

	CheckMemoryBounds(addr, num_elems*elem_size);
	const t_elem* begin = reinterpret_cast<t_elem*>(m_mem.get() + addr);
	return t_vec(begin, num_elems);
}


/**
 * push an array onto the stack
 */
template<class t_vec>
void VM::PushArray(const t_vec& vec, bool raw)
{
	using t_elem = typename t_vec::value_type;
	constexpr const std::size_t vec_idx = GetDataTypeIndex<t_vec>();
	constexpr const t_addr elem_size = GetDataTypeSize<t_elem>();

	t_addr num_elems = static_cast<t_addr>(vec.size());
	CheckMemoryBounds(m_sp, -num_elems*elem_size);

	m_sp -= num_elems*elem_size;
	t_elem* begin = reinterpret_cast<t_elem*>(m_mem.get() + m_sp);
	std::memcpy(begin, vec.data(), num_elems*elem_size);

	PushRaw<t_addr, m_addrsize>(num_elems);

	if(!raw)
	{
		// push descriptor
		PushRaw<t_byte, m_bytesize>(static_cast<t_byte>(
			GetArraySymbolType<t_elem>()));

		if(m_debug)
		{
			using namespace m_ops;
			std::cout << "pushed " << GetDataTypeName(vec_idx) << " "
				<< vec << "." << std::endl;
		}
	}
}


/**
 * read an array from a memory address
 */
template<class t_vec>
t_vec VM::ReadArrayRaw(typename VM::t_addr addr) const
{
	using t_elem = typename t_vec::value_type;
	constexpr const t_addr elem_size = GetDataTypeSize<t_elem>();

	t_addr num_elems = ReadMemRaw<t_addr>(addr);
	addr += m_addrsize;

	CheckMemoryBounds(addr, num_elems*elem_size);
	const t_elem *begin = reinterpret_cast<t_elem*>(&m_mem[addr]);

	return t_vec(begin, num_elems);
}


/**
 * write an array to a memory address
 */
template<class t_vec>
void VM::WriteArray(typename VM::t_addr addr, const t_vec& vec, bool raw)
{
	using t_elem = typename t_vec::value_type;
	constexpr const std::size_t vec_idx = GetDataTypeIndex<t_vec>();
	constexpr const t_addr elem_size = GetDataTypeSize<t_elem>();

	if(!raw)
	{
		if(m_debug)
		{
			using namespace m_ops;
			std::cout << "writing " << GetDataTypeName(vec_idx)
				<< " " << vec << " to address " << addr
				<< "." << std::endl;
		}

		// write descriptor prefix
		WriteMemRaw<t_byte>(addr, static_cast<t_byte>(
			GetArraySymbolType<t_elem>()));
		addr += m_bytesize;
	}

	t_addr num_elems = static_cast<t_addr>(vec.size());
	CheckMemoryBounds(addr, m_addrsize + num_elems*elem_size);

	// write array length
	WriteMemRaw<t_addr>(addr, num_elems);
	addr += m_addrsize;

	// write array
	t_elem* begin = reinterpret_cast<t_elem*>(&m_mem[addr]);
	std::memcpy(begin, vec.data(), num_elems*elem_size);
}


/**
 * read an array element from a given index and push it onto the stack
 */
template<class t_vec>
void VM::ReadArrayElem(const t_data& arr, typename VM::t_int idx)
{
	using t_elem = typename t_vec::value_type;
	constexpr const std::size_t vec_idx = GetDataTypeIndex<t_vec>();
	constexpr const std::size_t elem_idx = GetDataTypeIndex<t_elem>();

	// gets array element
	const t_vec& vec = std::get<vec_idx>(arr);
	idx = safe_array_index<t_int>(idx, vec.size());

	PushData(t_data{std::in_place_index<elem_idx>, vec[idx]});
}


/**
 * read an array element range from given indices
 * and push the new array onto the stack
 */
template<class t_vec>
void VM::ReadArrayElemRange(const t_data& arr,
	typename VM::t_int idx1, typename VM::t_int idx2)
{
	constexpr const std::size_t vec_idx = GetDataTypeIndex<t_vec>();

	// gets array range
	const t_vec& vec = std::get<vec_idx>(arr);
	idx1 = safe_array_index<t_int>(idx1, vec.size());
	idx2 = safe_array_index<t_int>(idx2, vec.size());

	t_int delta = (idx2 >= idx1 ? 1 : -1);
	idx2 += delta;

	t_vec newvec = m::zero<t_vec>(std::abs(idx2 - idx1));
	t_int new_idx = 0;
	for(t_int idx = idx1; idx != idx2; idx += delta)
		newvec[new_idx++] = vec[idx];
	PushData(t_data{std::in_place_index<vec_idx>, newvec});
}


/**
 * write an array element to a memory address
 */
template<class t_vec>
void VM::WriteArrayElem(typename VM::t_addr addr, const t_data& data,
	typename VM::t_int idx)
{
	using t_elem = typename t_vec::value_type;
	constexpr const t_addr elem_size = GetDataTypeSize<t_elem>();
	constexpr const std::size_t elem_idx = GetDataTypeIndex<t_elem>();

	if(data.index() != elem_idx)
	{
		throw std::runtime_error(
			"Array element has to be of scalar type.");
	}

	// get array length indicator
	t_addr veclen = ReadMemRaw<t_addr>(addr);
	addr += m_addrsize;

	// skip to element and overwrite it
	idx = safe_array_index<t_addr>(idx, veclen);
	addr += idx * elem_size;
	WriteMemRaw(addr, std::get<elem_idx>(data));
}


/**
 * write an array element range to a memory address
 */
template<class t_vec>
void VM::WriteArrayElemRange(typename VM::t_addr addr, const t_data& data,
	typename VM::t_int idx1, typename VM::t_int idx2)
{
	using t_elem = typename t_vec::value_type;
	constexpr const t_addr elem_size = GetDataTypeSize<t_elem>();
	constexpr const std::size_t vec_idx = GetDataTypeIndex<t_vec>();
	constexpr const std::size_t elem_idx = GetDataTypeIndex<t_elem>();

	const t_vec *rhsvec = nullptr;
	const t_elem *rhsreal = nullptr;

	if(data.index() == vec_idx)        // rhs is an array
		rhsvec = &std::get<vec_idx>(data);
	else if(data.index() == elem_idx)  // rhs is a scalar
		rhsreal = &std::get<elem_idx>(data);
	else
		throw std::runtime_error(
			"Array range has to be of array or scalar type.");

	// get array length indicator
	t_addr veclen = ReadMemRaw<t_addr>(addr);
	addr += m_addrsize;

	idx1 = safe_array_index<t_addr>(idx1, veclen);
	idx2 = safe_array_index<t_addr>(idx2, veclen);
	t_int delta = (idx2 >= idx1 ? 1 : -1);
	idx2 += delta;

	// skip to element range and overwrite it
	addr += idx1 * elem_size;
	t_int cur_idx = 0;
	for(t_int idx = idx1; idx != idx2; idx += delta)
	{
		t_elem elem{};

		if(rhsvec)
		{
			if(std::size_t(cur_idx) >= rhsvec->size())
				throw std::runtime_error("Array index out of bounds.");

			elem = (*rhsvec)[cur_idx++];
		}
		else if(rhsreal)
		{
			elem = *rhsreal;
		}

		WriteMemRaw(addr, elem);
		addr += elem_size * delta;
	}
}


/**
 * read a raw value from memory
 */
template<class t_val>
t_val VM::ReadMemRaw(typename VM::t_addr addr) const
{
	// string type
	if constexpr(std::is_same_v<std::decay_t<t_val>, t_str>)
	{
		t_addr len = ReadMemRaw<t_addr>(addr);
		addr += m_addrsize;

		CheckMemoryBounds(addr, len*m_charsize);
		const t_char* begin = reinterpret_cast<t_char*>(&m_mem[addr]);

		return t_str(begin, len);
	}

	// complex type
	else if constexpr(std::is_same_v<std::decay_t<t_val>, t_cplx>)
	{
		CheckMemoryBounds(addr, GetDataTypeSize<t_cplx>());
		const t_real* real = reinterpret_cast<t_real*>(
			&m_mem[addr]);
		const t_real* imag = reinterpret_cast<t_real*>(
			&m_mem[addr + GetDataTypeSize<t_real>()]);

		return t_cplx{*real, *imag};
	}

	// real array type
	else if constexpr(std::is_same_v<std::decay_t<t_val>, t_vec_real>)
	{
		return ReadArrayRaw<t_vec_real>(addr);
	}

	// int array type
	else if constexpr(std::is_same_v<std::decay_t<t_val>, t_vec_int>)
	{
		return ReadArrayRaw<t_vec_int>(addr);
	}

	// complex array type
	else if constexpr(std::is_same_v<std::decay_t<t_val>, t_vec_cplx>)
	{
		return ReadArrayRaw<t_vec_cplx>(addr);
	}

	// primitive types
	else
	{
		CheckMemoryBounds(addr, sizeof(t_val));
		t_val val = *reinterpret_cast<t_val*>(&m_mem[addr]);

		return val;
	}
}


/**
 * write a raw value to memory
 */
template<class t_val>
void VM::WriteMemRaw(typename VM::t_addr addr, const t_val& val)
{
	// string type
	if constexpr(std::is_same_v<std::decay_t<t_val>, t_str>)
	{
		t_addr len = static_cast<t_addr>(val.length());
		CheckMemoryBounds(addr, m_addrsize + len*m_charsize);

		// write string length
		WriteMemRaw<t_addr>(addr, len);
		addr += m_addrsize;

		// write string
		t_char* begin = reinterpret_cast<t_char*>(&m_mem[addr]);
		std::memcpy(begin, val.data(), len*m_charsize);
	}

	// complex type
	else if constexpr(std::is_same_v<std::decay_t<t_val>, t_cplx>)
	{
		CheckMemoryBounds(addr, GetDataTypeSize<t_cplx>());

		t_real* begin = reinterpret_cast<t_real*>(&m_mem[addr]);
		*(begin + 0) = val.real();
		*(begin + 1) = val.imag();
	}

	// real array type
	else if constexpr(std::is_same_v<std::decay_t<t_val>, t_vec_real>)
	{
		WriteArray(addr, val, true);
	}

	// primitive types
	else
	{
		CheckMemoryBounds(addr, sizeof(t_val));
		*reinterpret_cast<t_val*>(&m_mem[addr]) = val;
	}
}


/**
 * get the value on top of the stack
 */
template<class t_val, typename VM::t_addr valsize>
t_val VM::TopRaw(typename VM::t_addr sp_offs) const
{
	t_addr addr = m_sp + sp_offs;
	CheckMemoryBounds(addr, valsize);

	return *reinterpret_cast<t_val*>(m_mem.get() + addr);
}


/**
 * pop a raw value from the stack
 */
template<class t_val, typename VM::t_addr valsize>
t_val VM::PopRaw()
{
	CheckMemoryBounds(m_sp, valsize);

	t_val *valptr = reinterpret_cast<t_val*>(m_mem.get() + m_sp);
	t_val val = *valptr;

	if(m_zeropoppedvals)
		*valptr = 0;

	m_sp += valsize;	// stack grows to lower addresses

	return val;
}


/**
 * push a raw value onto the stack
 */
template<class t_val, typename VM::t_addr valsize>
void VM::PushRaw(const t_val& val)
{
	CheckMemoryBounds(m_sp, valsize);

	m_sp -= valsize;	// stack grows to lower addresses
	*reinterpret_cast<t_val*>(m_mem.get() + m_sp) = val;
}


#endif
