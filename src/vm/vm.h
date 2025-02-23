/**
 * zero-address code vm
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 8-jun-2022
 * @license see 'LICENSE' file
 */

#ifndef __0ACVM_H__
#define __0ACVM_H__


#include <type_traits>
#include <memory>
#include <array>
#include <vector>
#include <optional>
#include <variant>
#include <iostream>
#include <sstream>
#include <bit>
#include <thread>
#include <chrono>
#include <atomic>
#include <limits>
#include <string>
#include <cstring>
#include <cmath>

#include "opcodes.h"
#include "helpers.h"


class VM
{
public:
	// data types
	using t_addr = ::t_vm_addr;
	using t_int = ::t_vm_int;
	using t_real = ::t_vm_real;
	using t_cplx = ::t_vm_cplx;
	using t_byte = ::t_vm_byte;
	using t_bool = ::t_vm_byte;

	using t_vec_real = ::t_vm_vec_real;

	using t_str = ::t_vm_str;

	using t_uint = typename std::make_unsigned<t_int>::type;
	using t_char = typename t_str::value_type;

	// variant of all data types
	using t_data = std::variant<
		std::monostate /*prevents default-construction of first type (t_real)*/,
		t_real, t_int, t_cplx, t_bool, t_addr,
		t_vec_real, t_str>;

	// use variant type indices and std::in_place_index instead of direct types
	// because two types might be identical (e.g. t_int and t_addr)
	static constexpr const std::size_t m_realidx = 1;
	static constexpr const std::size_t m_intidx  = 2;
	static constexpr const std::size_t m_cplxidx = 3;
	static constexpr const std::size_t m_boolidx = 4;
	static constexpr const std::size_t m_addridx = 5;
	static constexpr const std::size_t m_realarridx  = 6;
	static constexpr const std::size_t m_stridx  = 7;

	// data type sizes
	static constexpr const t_addr m_bytesize = sizeof(t_byte);
	static constexpr const t_addr m_addrsize = sizeof(t_addr);
	static constexpr const t_addr m_charsize = sizeof(t_char);

	// interrupts
	static constexpr const t_addr m_num_interrupts = 16;
	static constexpr const t_addr m_timer_interrupt = 0;


	/**
	 * helper function to get static data type indices into the t_data variant
	 */
	template<typename t_ty>
	static constexpr std::size_t GetDataTypeIndex()
	{
		if constexpr(std::is_same_v<std::decay_t<t_ty>, t_real>)
			return m_realidx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_int>)
			return m_intidx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_cplx>)
			return m_cplxidx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_bool>)
			return m_boolidx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_addr>)
			return m_addridx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_vec_real>)
			return m_realarridx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_str>)
			return m_stridx;
		else
			static_assert(false, "GetDataTypeIndex: Data type not yet implemented.");
		return 0;
	}


	/**
	 * helper function to get static data type sizes
	 */
	template<typename t_ty>
	static constexpr t_addr GetDataTypeSize()
	{
		if constexpr(std::is_same_v<std::decay_t<t_ty>, t_real>)
			return sizeof(t_real);
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_int>)
			return sizeof(t_int);
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_cplx>)
			return 2*sizeof(t_real);
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_bool>)
			return sizeof(t_bool);
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_addr>)
			return m_addrsize;
		else
			static_assert(false, "GetDataTypeSize: Data type not yet implemented.");
		return 0;
	}


public:
	VM(t_addr memsize = 0x1000);
	~VM();

	void SetDebug(bool b) { m_debug = b; }
	void SetDrawMemImages(bool b) { m_drawmemimages = b; }
	void SetChecks(bool b) { m_checks = b; }
	void SetZeroPoppedVals(bool b) { m_zeropoppedvals = b; }

	static const char* GetDataTypeName(std::size_t type_idx);
	static const char* GetDataTypeName(const t_data& dat);

	void Reset();
	bool Run();

	void SetMem(t_addr addr, t_byte data);
	void SetMem(t_addr addr, const t_byte* data, std::size_t size, bool is_code = false);
	void SetMem(t_addr addr, const std::string& data, bool is_code = false);

	t_addr GetSP() const { return m_sp; }
	t_addr GetBP() const { return m_bp; }
	t_addr GetGBP() const { return m_gbp; }
	t_addr GetIP() const { return m_ip; }

	void SetSP(t_addr sp) { m_sp = sp; }
	void SetBP(t_addr bp) { m_bp = bp; }
	void SetGBP(t_addr gbp) { m_gbp = gbp; }
	void SetIP(t_addr ip) { m_ip = ip; }

	//get top data from the stack
	t_data TopData() const;

	//pop data from the stack
	t_data PopData();

	//signals an interrupt
	void RequestInterrupt(t_addr num);

	//visualises vm memory utilisation
	void DrawMemoryImage();


protected:
	//return the size of the held data
	t_addr GetDataSize(const t_data& data) const;

	//call external function
	t_data CallExternal(const t_str& func_name);

	//pop an address from the stack
	t_addr PopAddress();

	// push an address to stack
	void PushAddress(t_addr addr, VMType ty = VMType::ADDR_MEM);

	// pop a bool from the stack
	bool PopBool();

	// push a bool to the stack
	void PushBool(bool val);

	// pop a complex number from the stack
	t_cplx PopComplex();

	// get the complex number on top of the stack
	t_cplx TopComplex(t_addr sp_offs = 0) const;

	// push a complex number to the stack
	void PushComplex(const t_cplx& val);

	// pop a string from the stack
	t_str PopString();

	// get the string on top of the stack
	t_str TopString(t_addr sp_offs = 0) const;

	// push a string to the stack
	void PushString(const t_str& str);

	// push data onto the stack
	void PushData(const t_data& data, VMType ty = VMType::UNKNOWN, bool err_on_unknown = true);

	// read the data type prefix from data in memory
	VMType ReadMemType(t_addr addr);

	// read data from memory
	std::tuple<VMType, t_data> ReadMemData(t_addr addr);

	// write data to memory
	void WriteMemData(t_addr addr, const t_data& data);


	/**
	 * pop a vector from the stack
	 * a vector consists of an t_addr giving the length
	 * following by the vector elements
	 */
	template<class t_vec = t_vec_real>
	t_vec PopVector(bool raw_elems = true)
	{
		using t_elem = typename t_vec::value_type;
		constexpr const t_addr elem_size = GetDataTypeSize<t_elem>();
		constexpr const std::size_t elem_type_idx = GetDataTypeIndex<t_elem>();

		// raw real array and vector size follow without descriptors
		if(raw_elems)
		{
			t_addr num_elems = PopRaw<t_addr, m_addrsize>();
			CheckMemoryBounds(m_sp, num_elems*elem_size);

			t_elem* begin = reinterpret_cast<t_elem*>(m_mem.get() + m_sp);
			t_vec vec(begin, num_elems);
			m_sp += num_elems*elem_size;

			if(m_zeropoppedvals)
				std::memset(begin, 0, num_elems*elem_size);

			return vec;
		}

		// individual elements and vector size with descriptor are on the stack
		else
		{
			t_addr num_elems = PopAddress();
			t_vec vec(num_elems);

			for(t_addr i = 0; i < num_elems; ++i)
			{
				t_data val = PopData();
				if(val.index() != elem_type_idx)
					throw std::runtime_error("Wrong element type for vector.");

				t_elem elem = std::get<elem_type_idx>(val);
				vec[num_elems - i - 1] = elem;
			}

			if(m_debug)
			{
				using namespace m_ops;
				std::cout << "popped non-raw vector " << vec << "." << std::endl;
			}

			return vec;
		}
	}


	/**
	 * get the vector on top of the stack
	 */
	template<class t_vec = t_vec_real>
	t_vec TopVector(t_addr sp_offs = 0) const
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
	 * push a vector onto the stack
	 */
	template<class t_vec = t_vec_real>
	void PushVector(const t_vec& vec)
	{
		using t_elem = typename t_vec::value_type;
		constexpr const t_addr elem_size = GetDataTypeSize<t_elem>();

		t_addr num_elems = static_cast<t_addr>(vec.size());
		CheckMemoryBounds(m_sp, -num_elems*elem_size);

		m_sp -= num_elems*elem_size;
		t_elem* begin = reinterpret_cast<t_elem*>(m_mem.get() + m_sp);
		std::memcpy(begin, vec.data(), num_elems*elem_size);

		PushRaw<t_addr, m_addrsize>(num_elems);
	}


	/**
	 * read a vector from a memory address
	 */
	template<class t_vec = t_vec_real>
	t_vec ReadVectorRaw(t_addr addr) const
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
	 * write a vector to a memory address
	 */
	template<class t_vec = t_vec_real>
	void WriteVectorRaw(t_addr addr, const t_vec& vec)
	{
		using t_elem = typename t_vec::value_type;
		constexpr const t_addr elem_size = GetDataTypeSize<t_elem>();

		t_addr num_elems = static_cast<t_addr>(vec.size());
		CheckMemoryBounds(addr, m_addrsize + num_elems*elem_size);

		// write vector length
		WriteMemRaw<t_addr>(addr, num_elems);
		addr += m_addrsize;

		// write vector
		t_elem* begin = reinterpret_cast<t_elem*>(&m_mem[addr]);
		std::memcpy(begin, vec.data(), num_elems*elem_size);
	}


	/**
	 * read a vector element from a given index and push it onto the stack
	 */
	template<class t_vec = t_vec_real>
	void ReadVectorElem(const t_data& arr, t_int idx = 0)
	{
		using t_elem = typename t_vec::value_type;
		constexpr const std::size_t vec_idx = GetDataTypeIndex<t_vec>();
		constexpr const std::size_t elem_idx = GetDataTypeIndex<t_elem>();

		// gets vector element
		const t_vec& vec = std::get<vec_idx>(arr);
		idx = safe_array_index<t_int>(idx, vec.size());

		PushData(t_data{std::in_place_index<elem_idx>, vec[idx]});
	}


	/**
	 * read a vector element range from given indices
	 * and push the new vector onto the stack
	 */
	template<class t_vec = t_vec_real>
	void ReadVectorElemRange(const t_data& arr, t_int idx1 = 0, t_int idx2 = 0)
	{
		constexpr const std::size_t vec_idx = GetDataTypeIndex<t_vec>();

		// gets vector range
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
	 * write a vector element to a memory address
	 */
	template<class t_vec = t_vec_real>
	void WriteVectorElem(t_addr addr, const t_data& data, t_int idx = 0)
	{
		using t_elem = typename t_vec::value_type;
		constexpr const t_addr elem_size = GetDataTypeSize<t_elem>();
		constexpr const std::size_t elem_idx = GetDataTypeIndex<t_elem>();

		if(data.index() != elem_idx)
		{
			throw std::runtime_error(
				"Vector element has to be of scalar type.");
		}

		// get vector length indicator
		t_addr veclen = ReadMemRaw<t_addr>(addr);
		addr += m_addrsize;

		// skip to element and overwrite it
		idx = safe_array_index<t_addr>(idx, veclen);
		addr += idx * elem_size;
		WriteMemRaw(addr, std::get<elem_idx>(data));
	}


	/**
	 * write a vector element range to a memory address
	 */
	template<class t_vec = t_vec_real>
	void WriteVectorElemRange(t_addr addr, const t_data& data,
		t_int idx1 = 0, t_int idx2 = 0)
	{
		using t_elem = typename t_vec::value_type;
		constexpr const t_addr elem_size = GetDataTypeSize<t_elem>();
		constexpr const std::size_t vec_idx = GetDataTypeIndex<t_vec>();
		constexpr const std::size_t elem_idx = GetDataTypeIndex<t_elem>();

		const t_vec *rhsvec = nullptr;
		const t_elem *rhsreal = nullptr;

		if(data.index() == vec_idx)        // rhs is a vector
			rhsvec = &std::get<vec_idx>(data);
		else if(data.index() == elem_idx)  // rhs is a scalar
			rhsreal = &std::get<elem_idx>(data);
		else
			throw std::runtime_error(
				"Vector range has to be of vector or scalar type.");

		// get vector length indicator
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
				{
					throw std::runtime_error(
						"Vector index out of bounds.");
				}

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
	t_val ReadMemRaw(t_addr addr) const
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

		// real vector type
		else if constexpr(std::is_same_v<std::decay_t<t_val>, t_vec_real>)
		{
			return ReadVectorRaw<t_vec_real>(addr);
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
	void WriteMemRaw(t_addr addr, const t_val& val)
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

		// real vector type
		else if constexpr(std::is_same_v<std::decay_t<t_val>, t_vec_real>)
		{
			WriteVectorRaw(addr, val);
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
	template<class t_val, t_addr valsize = sizeof(t_val)>
	t_val TopRaw(t_addr sp_offs = 0) const
	{
		t_addr addr = m_sp + sp_offs;
		CheckMemoryBounds(addr, valsize);

		return *reinterpret_cast<t_val*>(m_mem.get() + addr);
	}


	/**
	 * pop a raw value from the stack
	 */
	template<class t_val, t_addr valsize = sizeof(t_val)>
	t_val PopRaw()
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
	template<class t_val, t_addr valsize = sizeof(t_val)>
	void PushRaw(const t_val& val)
	{
		CheckMemoryBounds(m_sp, valsize);

		m_sp -= valsize;	// stack grows to lower addresses
		*reinterpret_cast<t_val*>(m_mem.get() + m_sp) = val;
	}


	/**
	 * cast from one variable type to the other
	 */
	template<std::size_t toidx>
	void OpCast()
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

		// casting from vector
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
				msg << "Invalid cast from vector to "
					<< GetDataTypeName(toidx) << ".";
				throw std::runtime_error(msg.str());
			}
		}
	}


	/**
	 * cast to an array variable type
	 */
	template<std::size_t toidx>
	void OpArrayCast(t_addr size)
	{
		//using t_to = std::variant_alternative_t<toidx, t_data>;
		t_data data = TopData();

		// casting to vector
		if constexpr(toidx == m_realarridx)
		{
			// casting from vector
			if(data.index() == m_realarridx)
				return;  // no action needed, TODO: check sizes

			// casting from real
			else if(data.index() == m_realidx)
			{
				t_real val = std::get<m_realidx>(data);
				PopData();

				// set every element of the vector to the real value
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

				// set every element of the vector to the int value
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
	t_val OpArithmetic(const t_val& val1, const t_val& val2)
	{
		t_val result{};

		// string operators
		if constexpr(std::is_same_v<std::decay_t<t_val>, t_str>)
		{
			if constexpr(op == '+')
				result = val1 + val2;
		}

		// vector operators
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
	void OpArithmetic()
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

		// scale vector
		else if(val1.index() == m_realarridx && val2.index() == m_realidx && op == '*')
		{
			using namespace m_ops;
			const t_vec_real& vec = std::get<m_realarridx>(val1);
			const t_real s = std::get<m_realidx>(val2);
			result = t_data{std::in_place_index<m_realarridx>, s * vec};
		}

		// scale vector
		else if(val1.index() == m_realarridx && val2.index() == m_realidx && op == '/')
		{
			using namespace m_ops;
			const t_vec_real& vec = std::get<m_realarridx>(val1);
			const t_real s = std::get<m_realidx>(val2);
			result = t_data{std::in_place_index<m_realarridx>, vec / s};
		}

		// scale vector
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
	void OpLogical()
	{
		bool val2 = PopBool();
		bool val1 = PopBool();

		bool result = 0;
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
	t_val OpBinary(const t_val& val1, const t_val& val2)
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
	void OpBinary()
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
	bool OpComparison(const t_val& val1, const t_val& val2)
	{
		bool result = 0;

		// string comparison
		if constexpr(std::is_same_v<std::decay_t<t_val>, t_str>)
		{
			if constexpr(op == OpCode::EQU)
				result = (val1 == val2);
			else if constexpr(op == OpCode::NEQU)
				result = (val1 != val2);
		}

		// vector comparison
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
	void OpComparison()
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

		bool result;

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


	// sets the address of an interrupt service routine
	void SetISR(t_addr num, t_addr addr);

	void StartTimer();
	void StopTimer();


private:
	void CheckMemoryBounds(t_addr addr, t_addr size = 1) const;
	void CheckPointerBounds() const;
	void UpdateCodeRange(t_addr begin, t_addr end);

	void TimerFunc();


private:
	bool m_debug{false};               // write debug messages
	bool m_checks{true};               // do memory boundary checks
	bool m_drawmemimages{false};       // write memory dump images
	bool m_zeropoppedvals{false};      // zero memory of popped values
	t_real m_eps{std::numeric_limits<t_real>::epsilon()};
	t_int m_prec{6};

	std::unique_ptr<t_byte[]> m_mem{}; // ram
	t_addr m_code_range[2]{-1, -1};    // address range where the code resides

	// registers
	t_addr m_ip{};                     // instruction pointer
	t_addr m_sp{};                     // stack pointer
	t_addr m_bp{};                     // base pointer for local variables
	t_addr m_gbp{};                    // global base pointer for global variables

	// memory sizes and ranges
	t_addr m_memsize = 0x1000;         // total memory size

	// signals interrupt requests
	std::array<std::atomic_bool, m_num_interrupts> m_irqs{};
	// addresses of the interrupt service routines
	std::array<std::optional<t_addr>, m_num_interrupts> m_isrs{};

	std::thread m_timer_thread{};
	bool m_timer_running{false};
	std::chrono::milliseconds m_timer_ticks{250};
};


#endif
