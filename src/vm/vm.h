/**
 * zero-address code vm
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 8-jun-2022
 * @license see 'LICENSE.GPL' file
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
	using t_byte = ::t_vm_byte;
	using t_bool = ::t_vm_byte;

	using t_str = ::t_vm_str;
	using t_vec = ::t_vm_vec;
	using t_mat = ::t_vm_mat;

	using t_uint = typename std::make_unsigned<t_int>::type;
	using t_char = typename t_str::value_type;

	// variant of all data types
	using t_data = std::variant<
		std::monostate /*prevents default-construction of first type (t_real)*/,
		t_real, t_int, t_bool, t_addr, t_str, t_vec, t_mat>;

	// use variant type indices and std::in_place_index instead of direct types
	// because two types might be identical (e.g. t_int and t_addr)
	static constexpr const std::size_t m_realidx = 1;
	static constexpr const std::size_t m_intidx  = 2;
	static constexpr const std::size_t m_boolidx = 3;
	static constexpr const std::size_t m_addridx = 4;
	static constexpr const std::size_t m_stridx  = 5;
	static constexpr const std::size_t m_vecidx  = 6;
	static constexpr const std::size_t m_matidx  = 7;

	// data type sizes
	static constexpr const t_addr m_bytesize = sizeof(t_byte);
	static constexpr const t_addr m_addrsize = sizeof(t_addr);
	static constexpr const t_addr m_realsize = sizeof(t_real);
	static constexpr const t_addr m_intsize = sizeof(t_int);
	static constexpr const t_addr m_boolsize = sizeof(t_bool);
	static constexpr const t_addr m_charsize = sizeof(t_char);

	static constexpr const t_addr m_num_interrupts = 16;
	static constexpr const t_addr m_timer_interrupt = 0;


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

	// pop a string from the stack
	t_str PopString();

	// get the string on top of the stack
	t_str TopString(t_addr sp_offs = 0) const;

	// push a string to the stack
	void PushString(const t_str& str);

	// pop a vector from the stack
	t_vec PopVector(bool raw_elems = true);

	// get the vector on top of the stack
	t_vec TopVector(t_addr sp_offs = 0) const;

	// push a vector to the stack
	void PushVector(const t_vec& vec);

	// pop a matrix from the stack
	t_mat PopMatrix(bool raw_elems = true);

	// get the matrix on top of the stack
	t_mat TopMatrix(t_addr sp_offs = 0) const;

	// push a matrix to the stack
	void PushMatrix(const t_mat& vec);

	// push data onto the stack
	void PushData(const t_data& data, VMType ty = VMType::UNKNOWN, bool err_on_unknown = true);

	// read the data type prefix from data in memory
	VMType ReadMemType(t_addr addr);

	// read data from memory
	std::tuple<VMType, t_data> ReadMemData(t_addr addr);

	// write data to memory
	void WriteMemData(t_addr addr, const t_data& data);


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

			t_str str(begin, len);
			return str;
		}

		// vector type
		else if constexpr(std::is_same_v<std::decay_t<t_val>, t_vec>)
		{
			t_addr num_elems = ReadMemRaw<t_addr>(addr);
			addr += m_addrsize;

			CheckMemoryBounds(addr, num_elems*m_realsize);
			const t_real* begin = reinterpret_cast<t_real*>(&m_mem[addr]);

			t_vec vec(begin, num_elems);
			return vec;
		}

		// matrix type
		else if constexpr(std::is_same_v<std::decay_t<t_val>, t_mat>)
		{
			t_addr num_elems_1 = ReadMemRaw<t_addr>(addr);
			addr += m_addrsize;
			t_addr num_elems_2 = ReadMemRaw<t_addr>(addr);
			addr += m_addrsize;

			CheckMemoryBounds(addr, num_elems_1*num_elems_2*m_realsize);
			const t_real* begin = reinterpret_cast<t_real*>(&m_mem[addr]);

			t_mat mat(begin, num_elems_1, num_elems_2);
			return mat;
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

		// vector type
		else if constexpr(std::is_same_v<std::decay_t<t_val>, t_vec>)
		{
			t_addr num_elems = static_cast<t_addr>(val.size());
			CheckMemoryBounds(addr, m_addrsize + num_elems*m_realsize);

			// write vector length
			WriteMemRaw<t_addr>(addr, num_elems);
			addr += m_addrsize;

			// write vector
			t_real* begin = reinterpret_cast<t_real*>(&m_mem[addr]);
			std::memcpy(begin, val.data(), num_elems*m_realsize);
		}

		// matrix type
		else if constexpr(std::is_same_v<std::decay_t<t_val>, t_mat>)
		{
			t_addr num_elems_1 = static_cast<t_addr>(val.size1());
			t_addr num_elems_2 = static_cast<t_addr>(val.size2());
			CheckMemoryBounds(addr, m_addrsize + num_elems_1*num_elems_2*m_realsize);

			// write matrix lengths
			WriteMemRaw<t_addr>(addr, num_elems_1);
			addr += m_addrsize;
			WriteMemRaw<t_addr>(addr, num_elems_2);
			addr += m_addrsize;

			// write matrix
			t_real* begin = reinterpret_cast<t_real*>(&m_mem[addr]);
			std::memcpy(begin, val.data(), num_elems_1*num_elems_2*m_realsize);
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
		else if(data.index() == m_vecidx)
		{
			if constexpr(std::is_same_v<std::decay_t<t_to>, t_vec>)
				return;  // don't need to cast to the same type

			const t_vec& val = std::get<m_vecidx>(data);

			// convert to string
			if constexpr(std::is_same_v<std::decay_t<t_to>, t_str>)
			{
				std::ostringstream ostr;
				ostr.precision(m_prec);
				ostr << "[ ";
				for(std::size_t i=0; i<val.size(); ++i)
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

		// casting from matrix
		else if(data.index() == m_matidx)
		{
			if constexpr(std::is_same_v<std::decay_t<t_to>, t_mat>)
				return;  // don't need to cast to the same type

			const t_mat& val = std::get<m_matidx>(data);

			// convert to string
			if constexpr(std::is_same_v<std::decay_t<t_to>, t_str>)
			{
				std::ostringstream ostr;
				ostr.precision(m_prec);
				ostr << "[ ";
				for(std::size_t i=0; i<val.size1(); ++i)
				{
					for(std::size_t j=0; j<val.size2(); ++j)
					{
						t_real elem = val(i, j);
						if(m::equals_0<t_real>(elem, m_eps))
							elem = t_real(0);

						ostr << elem;
						if(j != val.size2()-1)
							ostr << ", ";
					}
						if(i != val.size1()-1)
							ostr << "; ";
				}
				ostr << " ]";

				PopData();
				PushData(t_data{std::in_place_index<m_stridx>, ostr.str()});
			}
			else
			{
				std::ostringstream msg;
				msg << "Invalid cast from matrix to "
					<< GetDataTypeName(toidx) << ".";
				throw std::runtime_error(msg.str());
			}
		}
	}


	/**
	 * cast to an array variable type (matrix or vector)
	 */
	template<std::size_t toidx>
	void OpArrayCast(t_addr size1, t_addr size2 = 0)
	{
		//using t_to = std::variant_alternative_t<toidx, t_data>;
		t_data data = TopData();

		// casting to vector
		if constexpr(toidx == m_vecidx)
		{
			// casting from vector
			if(data.index() == m_vecidx)
				return;  // no action needed, TODO: check sizes

			// casting from real
			else if(data.index() == m_realidx)
			{
				t_real val = std::get<m_realidx>(data);
				PopData();

				// set every element of the vector to the real value
				t_vec vec = m::create<t_vec>(size1);
				for(t_addr i=0; i<size1; ++i)
					vec[i] = val;
				PushData(t_data{std::in_place_index<m_vecidx>, vec});
			}

			// casting from int
			else if(data.index() == m_intidx)
			{
				t_real val = std::get<m_intidx>(data);
				PopData();

				// set every element of the vector to the int value
				t_vec vec = m::create<t_vec>(size1);
				for(t_addr i=0; i<size1; ++i)
					vec[i] = t_real(val);
				PushData(t_data{std::in_place_index<m_vecidx>, vec});
			}

			// casting from matrix
			else if(data.index() == m_matidx)
			{
				const t_mat& val = std::get<m_matidx>(data);
				PopData();

				// flatten the matrix
				t_vec vec = m::create<t_vec>(size1);
				for(t_addr i=0; i<size1; ++i)
					vec[i] = val(i/val.size2(), i%val.size2());
				PushData(t_data{std::in_place_index<m_vecidx>, vec});
			}
		}

		// casting to matrix
		else if constexpr(toidx == m_matidx)
		{
			// casting from matrix
			if(data.index() == m_matidx)
				return;  // no action needed, TODO: check sizes

			// casting from real
			else if(data.index() == m_realidx)
			{
				t_real val = std::get<m_realidx>(data);
				PopData();

				// set every element of the matrix to the real value
				t_mat mat = m::create<t_mat>(size1, size2);
				for(t_addr i=0; i<size1; ++i)
					for(t_addr j=0; j<size2; ++j)
						mat(i, j) = val;
				PushData(t_data{std::in_place_index<m_matidx>, mat});
			}

			// casting from int
			else if(data.index() == m_intidx)
			{
				t_real val = std::get<m_intidx>(data);
				PopData();

				// set every element of the matrix to the int value
				t_mat mat = m::create<t_mat>(size1, size2);
				for(t_addr i=0; i<size1; ++i)
					for(t_addr j=0; j<size2; ++j)
						mat(i, j) = t_real(val);
				PushData(t_data{std::in_place_index<m_matidx>, mat});
			}

			// casting from vector
			else if(data.index() == m_vecidx)
			{
				const t_vec& val = std::get<m_vecidx>(data);
				PopData();

				t_mat mat = m::create<t_mat>(size1, size2);
				for(t_addr i=0; i<size1; ++i)
					for(t_addr j=0; j<size2; ++j)
						mat(i, j) = val[i*size2 + j];
				PushData(t_data{std::in_place_index<m_matidx>, mat});
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
		else if constexpr(std::is_same_v<std::decay_t<t_val>, t_vec>)
		{
			if constexpr(op == '+')
				result = val1 + val2;
			else if constexpr(op == '-')
				result = val1 - val2;
		}

		// matrix operators
		else if constexpr(std::is_same_v<std::decay_t<t_val>, t_mat>)
		{
			if constexpr(op == '+')
				result = val1 + val2;
			else if constexpr(op == '-')
				result = val1 - val2;
			else if constexpr(op == '*')
				result = val1 * val2;
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
			else if constexpr(op == '^' && std::is_floating_point_v<t_val>)
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

		// matrix-vector product
		if(val1.index() == m_matidx && val2.index() == m_vecidx && op == '*')
		{
			using namespace m_ops;
			const t_mat& mat = std::get<m_matidx>(val1);
			const t_vec& vec = std::get<m_vecidx>(val2);
			result = t_data{std::in_place_index<m_vecidx>, mat*vec};
		}

		// matrix power
		else if(val1.index() == m_matidx && val2.index() == m_realidx && op == '^')
		{
			const t_mat& mat = std::get<m_matidx>(val1);
			t_int pow = static_cast<t_int>(std::get<m_realidx>(val2));
			auto [matpow, ok] = m::pow<t_mat, t_vec, t_int>(mat, pow);
			if(!ok)
				throw std::runtime_error("Matrix power could not be calculated.");
			result = t_data{std::in_place_index<m_matidx>, matpow};
		}

		// dot product
		else if(val1.index() == m_vecidx && val2.index() == m_vecidx && op == '*')
		{
			const t_vec& vec1 = std::get<m_vecidx>(val1);
			const t_vec& vec2 = std::get<m_vecidx>(val2);
			t_real dot = m::inner<t_vec>(vec1, vec2);
			result = t_data{std::in_place_index<m_realidx>, dot};
		}

		// scale vector
		else if(val1.index() == m_vecidx && val2.index() == m_realidx && op == '*')
		{
			using namespace m_ops;
			const t_vec& vec = std::get<m_vecidx>(val1);
			const t_real s = std::get<m_realidx>(val2);
			result = t_data{std::in_place_index<m_vecidx>, s * vec};
		}

		// scale vector
		else if(val1.index() == m_vecidx && val2.index() == m_realidx && op == '/')
		{
			using namespace m_ops;
			const t_vec& vec = std::get<m_vecidx>(val1);
			const t_real s = std::get<m_realidx>(val2);
			result = t_data{std::in_place_index<m_vecidx>, vec / s};
		}

		// scale vector
		else if(val2.index() == m_vecidx && val1.index() == m_realidx && op == '*')
		{
			using namespace m_ops;
			const t_vec& vec = std::get<m_vecidx>(val2);
			const t_real s = std::get<m_realidx>(val1);
			result = t_data{std::in_place_index<m_vecidx>, s * vec};
		}

		// scale matrix
		else if(val1.index() == m_matidx && val2.index() == m_realidx && op == '*')
		{
			using namespace m_ops;
			const t_mat& mat = std::get<m_matidx>(val1);
			const t_real s = std::get<m_realidx>(val2);
			result = t_data{std::in_place_index<m_matidx>, s * mat};
		}

		// scale matrix
		else if(val1.index() == m_matidx && val2.index() == m_realidx && op == '/')
		{
			using namespace m_ops;
			const t_mat& mat = std::get<m_matidx>(val1);
			const t_real s = std::get<m_realidx>(val2);
			result = t_data{std::in_place_index<m_matidx>, mat / s};
		}

		// scale matrix
		else if(val2.index() == m_matidx && val1.index() == m_realidx && op == '*')
		{
			using namespace m_ops;
			const t_mat& mat = std::get<m_matidx>(val2);
			const t_real s = std::get<m_realidx>(val1);
			result = t_data{std::in_place_index<m_matidx>, s * mat};
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
			else if(val1.index() == m_stridx)
			{
				result = t_data{std::in_place_index<m_stridx>, OpArithmetic<t_str, op>(
					std::get<m_stridx>(val1), std::get<m_stridx>(val2))};
			}
			else if(val1.index() == m_vecidx)
			{
				result = t_data{std::in_place_index<m_vecidx>, OpArithmetic<t_vec, op>(
					std::get<m_vecidx>(val1), std::get<m_vecidx>(val2))};
			}
			else if(val1.index() == m_matidx)
			{
				result = t_data{std::in_place_index<m_matidx>, OpArithmetic<t_mat, op>(
					std::get<m_matidx>(val1), std::get<m_matidx>(val2))};
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
		// might also use PopData and PushData in case ints
		// should also be allowed in boolean expressions
		t_bool val2 = PopRaw<t_bool, m_boolsize>();
		t_bool val1 = PopRaw<t_bool, m_boolsize>();

		t_bool result = 0;

		if constexpr(op == '&')
			result = val1 && val2;
		else if constexpr(op == '|')
			result = val1 || val2;
		else if constexpr(op == '^')
			result = val1 ^ val2;

		PushRaw<t_bool, m_boolsize>(result);
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
	t_bool OpComparison(const t_val& val1, const t_val& val2)
	{
		t_bool result = 0;

		// string comparison
		if constexpr(std::is_same_v<std::decay_t<t_val>, t_str>)
		{
			if constexpr(op == OpCode::EQU)
				result = (val1 == val2);
			else if constexpr(op == OpCode::NEQU)
				result = (val1 != val2);
		}

		// vector or matrix comparison
		else if constexpr(std::is_same_v<std::decay_t<t_val>, t_vec> ||
			std::is_same_v<std::decay_t<t_val>, t_mat>)
		{
			if constexpr(op == OpCode::EQU)
				result = m::equals(val1, val2, m_eps);
			else if constexpr(op == OpCode::NEQU)
				result = !m::equals(val1, val2, m_eps);
		}

		// integer /  real comparison
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

		t_bool result;

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
		else if(val1.index() == m_vecidx)
		{
			result = OpComparison<t_vec, op>(
				std::get<m_vecidx>(val1), std::get<m_vecidx>(val2));
		}
		else if(val1.index() == m_matidx)
		{
			result = OpComparison<t_mat, op>(
				std::get<m_matidx>(val1), std::get<m_matidx>(val2));
		}
		else
		{
			throw std::runtime_error("Invalid type in comparison operation.");
		}

		PushRaw<t_bool, m_boolsize>(result);
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
	t_addr m_gbp{};                    // gloabl base pointer for global variables

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
