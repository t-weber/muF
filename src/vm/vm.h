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
#include "common/helpers.h"



class VM
{
public:
	// data types
	using t_real = ::t_vm_real;
	using t_int = ::t_vm_int;
	using t_cplx = ::t_vm_cplx;
	using t_quat = ::t_vm_quat;

	using t_vec_real = ::t_vm_vec_real;
	using t_vec_int = ::t_vm_vec_int;
	using t_vec_cplx = ::t_vm_vec_cplx;
	using t_vec_quat = ::t_vm_vec_quat;

	using t_addr = ::t_vm_addr;
	using t_byte = ::t_vm_byte;
	using t_bool = ::t_vm_byte;
	using t_str = ::t_vm_str;

	using t_uint = typename std::make_unsigned<t_int>::type;
	using t_char = typename t_str::value_type;

	// variant of all data types
	using t_data = std::variant<
		std::monostate /*prevents default-construction of first type (t_real)*/,
		t_real /*1*/, t_int /*2*/, t_cplx /*3*/, t_bool /*4*/, t_addr /*5*/,
		t_vec_real /*6*/, t_vec_int /*7*/, t_vec_cplx /*8*/, t_str /*9*/,
		t_quat /*10*/, t_vec_quat /*11*/>;

	// use variant type indices and std::in_place_index instead of direct types
	// because two types might be identical (e.g. t_int and t_addr)
	static constexpr const std::size_t m_realidx = 1;
	static constexpr const std::size_t m_intidx  = 2;
	static constexpr const std::size_t m_cplxidx = 3;
	static constexpr const std::size_t m_boolidx = 4;
	static constexpr const std::size_t m_addridx = 5;
	static constexpr const std::size_t m_realarridx = 6;
	static constexpr const std::size_t m_intarridx = 7;
	static constexpr const std::size_t m_cplxarridx = 8;
	static constexpr const std::size_t m_stridx = 9;
	static constexpr const std::size_t m_quatidx = 10;
	static constexpr const std::size_t m_quatarridx = 11;

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
		static_assert(std::is_same_v<std::decay_t<t_ty>, t_real> ||
			std::is_same_v<std::decay_t<t_ty>, t_int> ||
			std::is_same_v<std::decay_t<t_ty>, t_cplx> ||
			std::is_same_v<std::decay_t<t_ty>, t_quat> ||
			std::is_same_v<std::decay_t<t_ty>, t_bool> ||
			std::is_same_v<std::decay_t<t_ty>, t_addr> ||
			std::is_same_v<std::decay_t<t_ty>, t_vec_real> ||
			std::is_same_v<std::decay_t<t_ty>, t_vec_int> ||
			std::is_same_v<std::decay_t<t_ty>, t_vec_cplx> ||
			std::is_same_v<std::decay_t<t_ty>, t_vec_quat> ||
			std::is_same_v<std::decay_t<t_ty>, t_str>,
			"GetDataTypeIndex: Data type not yet implemented.");

		if constexpr(std::is_same_v<std::decay_t<t_ty>, t_real>)
			return m_realidx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_int>)
			return m_intidx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_cplx>)
			return m_cplxidx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_quat>)
			return m_quatidx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_bool>)
			return m_boolidx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_addr>)
			return m_addridx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_vec_real>)
			return m_realarridx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_vec_int>)
			return m_intarridx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_vec_cplx>)
			return m_cplxarridx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_vec_quat>)
			return m_quatarridx;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_str>)
			return m_stridx;

		return 0;
	}


	/**
	 * helper function to get static data type sizes
	 */
	template<typename t_ty>
	static constexpr t_addr GetDataTypeSize()
	{
		static_assert(std::is_same_v<std::decay_t<t_ty>, t_real> ||
			std::is_same_v<std::decay_t<t_ty>, t_int> ||
			std::is_same_v<std::decay_t<t_ty>, t_cplx> ||
			std::is_same_v<std::decay_t<t_ty>, t_quat> ||
			std::is_same_v<std::decay_t<t_ty>, t_bool> ||
			std::is_same_v<std::decay_t<t_ty>, t_addr>,
			"GetDataTypeSize: Data type not yet implemented.");

		if constexpr(std::is_same_v<std::decay_t<t_ty>, t_real>)
			return sizeof(t_real);
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_int>)
			return sizeof(t_int);
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_cplx>)
			return 2*sizeof(t_real);
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_quat>)
			return 4*sizeof(t_real);
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_bool>)
			return sizeof(t_bool);
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_addr>)
			return m_addrsize;

		return 0;
	}


	/**
	 * get the symbol type of an array
	 */
	template<typename t_ty>
	static constexpr VMType GetArraySymbolType()
	{
		static_assert(std::is_same_v<std::decay_t<t_ty>, t_real> ||
			std::is_same_v<std::decay_t<t_ty>, t_int> ||
			std::is_same_v<std::decay_t<t_ty>, t_cplx> ||
			std::is_same_v<std::decay_t<t_ty>, t_quat>,
			"GetArraySymbolType: Data type not yet implemented.");

		if constexpr(std::is_same_v<std::decay_t<t_ty>, t_real>)
			return VMType::REALARR;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_int>)
			return VMType::INTARR;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_cplx>)
			return VMType::CPLXARR;
		else if constexpr(std::is_same_v<std::decay_t<t_ty>, t_quat>)
			return VMType::QUATARR;

		return VMType::UNKNOWN;
	}


	//template<std::size_t type_idx>
	static constexpr const char* GetDataTypeName(std::size_t type_idx)
	{
		switch(type_idx)
		{
			case m_realidx: return get_vm_type_name(VMType::REAL);
			case m_intidx: return get_vm_type_name(VMType::INT);
			case m_cplxidx: return get_vm_type_name(VMType::CPLX);
			case m_boolidx: return get_vm_type_name(VMType::BOOL);
			case m_stridx: return get_vm_type_name(VMType::STR);
			case m_addridx: return "address";

			case m_realarridx: return get_vm_type_name(VMType::REALARR);
			case m_intarridx: return get_vm_type_name(VMType::INTARR);
			case m_cplxarridx: return get_vm_type_name(VMType::CPLXARR);

			default: return "unknown";
		}
	}


	static constexpr const char* GetDataTypeName(const t_data& dat)
	{
		return GetDataTypeName(dat.index());
	}


public:
	VM(t_addr memsize = 0x1000);
	~VM();

	void SetDebug(bool b) { m_debug = b; }
	void SetDrawMemImages(bool b) { m_drawmemimages = b; }
	void SetChecks(bool b) { m_checks = b; }
	void SetZeroPoppedVals(bool b) { m_zeropoppedvals = b; }

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

	// sets the address of an interrupt service routine
	void SetISR(t_addr num, t_addr addr);

	void StartTimer();
	void StopTimer();

	// --------------------------------------------------------------------
	// memory/stack operations
	// --------------------------------------------------------------------
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

	// pop a quaternion from the stack
	t_quat PopQuaternion();

	// get the complex number on top of the stack
	t_cplx TopComplex(t_addr sp_offs = 0) const;

	// get the quaternion on top of the stack
	t_quat TopQuaternion(t_addr sp_offs = 0) const;

	// push a complex number to the stack
	void PushComplex(const t_cplx& val, bool raw = true);

	// push a quaternion to the stack
	void PushQuaternion(const t_quat& val, bool raw = true);

	// pop a string from the stack
	t_str PopString();

	// get the string on top of the stack
	t_str TopString(t_addr sp_offs = 0) const;

	// push a string to the stack
	void PushString(const t_str& str, bool raw = true);

	// push data onto the stack
	void PushData(const t_data& data, VMType ty = VMType::UNKNOWN, bool err_on_unknown = true);

	// read the data type prefix from data in memory
	VMType ReadMemType(t_addr addr);

	// read data from memory
	std::tuple<VMType, t_data> ReadMemData(t_addr addr);

	// write data to memory
	void WriteMemData(t_addr addr, const t_data& data);
	// --------------------------------------------------------------------

	// --------------------------------------------------------------------
	// array memory/stack operations
	// --------------------------------------------------------------------
	/**
	 * pop an array from the stack
	 * an array consists of an t_addr giving the length
	 * following by the array elements
	 */
	template<class t_vec = t_vec_real> t_vec PopArray(bool raw_elems = true);

	// get the array on top of the stack
	template<class t_vec = t_vec_real> t_vec TopArray(t_addr sp_offs = 0) const;

	// push an array onto the stack
	template<class t_vec = t_vec_real> void PushArray(const t_vec& vec, bool raw = true);

	// read an array from a memory address
	template<class t_vec = t_vec_real>
	t_vec ReadArrayRaw(t_addr addr) const;

	// write an array to a memory address
	template<class t_vec = t_vec_real>
	void WriteArray(t_addr addr, const t_vec& vec, bool raw = true);

	// read an array element from a given index and push it onto the stack
	template<class t_vec = t_vec_real>
	void ReadArrayElem(const t_data& arr, t_int idx = 0);

	/**
	 * read an array element range from given indices
	 * and push the new array onto the stack
	 */
	template<class t_vec = t_vec_real>
	void ReadArrayElemRange(const t_data& arr, t_int idx1 = 0, t_int idx2 = 0);

	// write an array element to a memory address
	template<class t_vec = t_vec_real>
	void WriteArrayElem(t_addr addr, const t_data& data, t_int idx = 0);

	// write an array element range to a memory address
	template<class t_vec = t_vec_real>
	void WriteArrayElemRange(t_addr addr, const t_data& data,
		t_int idx1 = 0, t_int idx2 = 0);

	// --------------------------------------------------------------------
	// raw memory/stack operations
	// --------------------------------------------------------------------
	// read a raw value from memory
	template<class t_val> t_val ReadMemRaw(t_addr addr) const;

	// write a raw value to memory
	template<class t_val>
	void WriteMemRaw(t_addr addr, const t_val& val);

	// get the value on top of the stack
	template<class t_val, t_addr valsize = sizeof(t_val)>
	t_val TopRaw(t_addr sp_offs = 0) const;

	// pop a raw value from the stack
	template<class t_val, t_addr valsize = sizeof(t_val)>
	t_val PopRaw();

	// push a raw value onto the stack
	template<class t_val, t_addr valsize = sizeof(t_val)>
	void PushRaw(const t_val& val);
	// --------------------------------------------------------------------

	// --------------------------------------------------------------------
	// operators
	// --------------------------------------------------------------------
	// cast from one variable type to the other
	template<std::size_t toidx> void OpCast();

	// cast from an array variable type
	template<class t_to, class t_vec> void OpCastFromArray(const t_data& data);

	// cast to an array variable type
	template<class t_vec_to> void OpCastToArray(t_addr size);

	// same-type arithmetic operation
	template<class t_val, char op>
	t_val OpArithmeticSameType(const t_val& val1, const t_val& val2);

	// arithmetic operation
	template<char op> void OpArithmetic();

	// matrix multiplication
	void OpMatrixMultiplication();

	// logical operation
	template<char op> void OpLogical();

	// same-type binary operation
	template<class t_val, char op>
	t_val OpBinarySameType(const t_val& val1, const t_val& val2);

	// binary operation
	template<char op> void OpBinary();

	// same-type comparison operation
	template<class t_val, OpCode op>
	bool OpComparisonSameType(const t_val& val1, const t_val& val2);

	// comparison operation
	template<OpCode op> void OpComparison();
	// --------------------------------------------------------------------


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
