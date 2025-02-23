/**
 * zero-address code vm
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 8-jun-2022
 * @license see 'LICENSE' file
 */

#include "vm.h"

#include <iostream>
#include <sstream>
#include <cstring>


VM::VM(t_addr memsize) : m_memsize{memsize}
{
	m_mem.reset(new t_byte[m_memsize]);
	Reset();
}


VM::~VM()
{
	StopTimer();
}


void VM::StartTimer()
{
	if(!m_timer_running)
	{
		m_timer_running = true;
		m_timer_thread = std::thread(&VM::TimerFunc, this);
	}
}


void VM::StopTimer()
{
	m_timer_running = false;
	if(m_timer_thread.joinable())
		m_timer_thread.join();
}


/**
 * function for timer thread
 */
void VM::TimerFunc()
{
	while(m_timer_running)
	{
		std::this_thread::sleep_for(m_timer_ticks);
		RequestInterrupt(m_timer_interrupt);
	}
}


/**
 * signals an interrupt
 */
void VM::RequestInterrupt(t_addr num)
{
	m_irqs[num] = true;
}


/**
 * sets the address of an interrupt service routine
 */
void VM::SetISR(t_addr num, t_addr addr)
{
	m_isrs[num] = addr;

	if(m_debug)
		std::cout << "Set isr " << num << " to address " << addr << "." << std::endl;
}


/**
 * pop an address from the stack
 * an address consists of the index of an register
 * holding the base address and an offset address
 */
VM::t_addr VM::PopAddress()
{
	// get register/type info from stack
	t_byte regval = PopRaw<t_byte, m_bytesize>();

	// get address from stack
	t_addr addr = PopRaw<t_addr, m_addrsize>();
	VMType thereg = static_cast<VMType>(regval);

	if(m_debug)
	{
		std::cout << "popped address " << t_int(addr)
			<< " of type " << t_int(regval)
			<< " (" << get_vm_type_name(thereg) << ")"
			<< "." << std::endl;
	}

	// get absolute address using base address from register
	switch(thereg)
	{
		case VMType::ADDR_MEM: break;
		case VMType::ADDR_IP: addr += m_ip; break;
		case VMType::ADDR_SP: addr += m_sp; break;
		case VMType::ADDR_BP: addr += m_bp; break;
		case VMType::ADDR_GBP: addr += m_gbp; break;
		default: throw std::runtime_error("Unknown address base register."); break;
	}

	return addr;
}


/**
 * push an address to stack
 */
void VM::PushAddress(t_addr addr, VMType ty)
{
	PushRaw<t_addr, m_addrsize>(addr);
	PushRaw<t_byte, m_bytesize>(static_cast<t_byte>(ty));
}


/**
 * pop a bool from the stack
 */
bool VM::PopBool()
{
	t_data dat = PopData();
	bool val = false;

	if(dat.index() == m_boolidx)
		val = std::get<m_boolidx>(dat) != 0;
	else if(dat.index() == m_intidx)
		val = std::get<m_intidx>(dat) != 0;
	else
		throw std::runtime_error("Data type not convertible to bool.");

	return val;
}


/**
 * push a bool to the stack
 */
void VM::PushBool(bool val)
{
	t_bool dat = static_cast<t_bool>(val);
	PushData(t_data{std::in_place_index<m_boolidx>, dat});
}


/**
 * pop a string from the stack
 * a string consists of an t_addr giving the length
 * following by the string (without 0-termination)
 */
VM::t_str VM::PopString()
{
	t_addr len = PopRaw<t_addr, m_addrsize>();
	CheckMemoryBounds(m_sp, len*m_charsize);

	t_char* begin = reinterpret_cast<t_char*>(m_mem.get() + m_sp);
	t_str str(begin, len);
	m_sp += len*m_charsize;

	if(m_zeropoppedvals)
		std::memset(begin, 0, len*m_bytesize);

	return str;
}


/**
 * get a string from the top of the stack
 */
VM::t_str VM::TopString(t_addr sp_offs) const
{
	t_addr len = TopRaw<t_addr, m_addrsize>(sp_offs);
	t_addr addr = m_sp + sp_offs + m_addrsize;

	CheckMemoryBounds(addr, len*m_charsize);
	t_char* begin = reinterpret_cast<t_char*>(m_mem.get() + addr);
	t_str str(begin, len);

	return str;
}


/**
 * push a string to the stack
 */
void VM::PushString(const VM::t_str& str)
{
	t_addr len = static_cast<t_addr>(str.length());
	CheckMemoryBounds(m_sp, -len*m_charsize);

	m_sp -= len*m_charsize;
	t_char* begin = reinterpret_cast<t_char*>(m_mem.get() + m_sp);
	std::memcpy(begin, str.data(), len*m_charsize);

	PushRaw<t_addr, m_addrsize>(len);
}


/**
 * pop a complex number from the stack
 */
VM::t_cplx VM::PopComplex()
{
	CheckMemoryBounds(m_sp, GetDataTypeSize<t_cplx>());

	t_real* begin = reinterpret_cast<t_real*>(m_mem.get() + m_sp);
	t_cplx cplx{*begin, *(begin + 1)};

	m_sp += GetDataTypeSize<t_cplx>();

	if(m_zeropoppedvals)
		std::memset(begin, 0, GetDataTypeSize<t_cplx>());

	return cplx;
}


/**
 * get a complex number from the top of the stack
 */
VM::t_cplx VM::TopComplex(t_addr sp_offs) const
{
	t_addr addr = m_sp + sp_offs;

	CheckMemoryBounds(addr, GetDataTypeSize<t_cplx>());
	const t_real* begin = reinterpret_cast<t_real*>(m_mem.get() + addr);

	return t_cplx{*begin, *(begin + 1)};
}


/**
 * push a complex number to the stack
 */
void VM::PushComplex(const VM::t_cplx& cplx)
{
	CheckMemoryBounds(m_sp, -GetDataTypeSize<t_cplx>());

	m_sp -= GetDataTypeSize<t_cplx>();
	t_real* begin = reinterpret_cast<t_real*>(m_mem.get() + m_sp);

	*(begin + 0) = cplx.real();
	*(begin + 1) = cplx.imag();
}


/**
 * get top data from the stack, which is prefixed
 * with a type descriptor byte
 */
VM::t_data VM::TopData() const
{
	// get data type info from stack
	t_byte tyval = TopRaw<t_byte, m_bytesize>();
	VMType ty = static_cast<VMType>(tyval);

	t_data dat;

	switch(ty)
	{
		case VMType::REAL:
		{
			dat = t_data{std::in_place_index<m_realidx>,
				TopRaw<t_real, GetDataTypeSize<t_real>()>(m_bytesize)};
			break;
		}

		case VMType::INT:
		{
			dat = t_data{std::in_place_index<m_intidx>,
				TopRaw<t_int, GetDataTypeSize<t_real>()>(m_bytesize)};
			break;
		}

		case VMType::CPLX:
		{
			dat = t_data{std::in_place_index<m_cplxidx>,
				TopComplex(m_bytesize)};
			break;
		}

		case VMType::BOOL:
		{
			dat = t_data{std::in_place_index<m_boolidx>,
				TopRaw<t_bool, GetDataTypeSize<t_bool>()>(m_bytesize)};
			break;
		}

		case VMType::ADDR_MEM:
		case VMType::ADDR_IP:
		case VMType::ADDR_SP:
		case VMType::ADDR_BP:
		case VMType::ADDR_GBP:
		{
			dat = t_data{std::in_place_index<m_addridx>,
				TopRaw<t_addr, m_addrsize>(m_bytesize)};
			break;
		}

		case VMType::STR:
		{
			dat = t_data{std::in_place_index<m_stridx>,
				TopString(m_bytesize)};
			break;
		}

		case VMType::REALARR:
		{
			dat = t_data{std::in_place_index<m_realarridx>,
				TopVector(m_bytesize)};
				break;
		}

		default:
		{
			std::ostringstream msg;
			msg << "Top: Data type " << (int)tyval
				<< " (" << get_vm_type_name(ty) << ")"
				<< " not yet implemented.";
			throw std::runtime_error(msg.str());
			break;
		}
	}

	return dat;
}


/**
 * pop data from the stack, which is prefixed
 * with a type descriptor byte
 */
VM::t_data VM::PopData()
{
	// get data type info from stack
	t_byte tyval = PopRaw<t_byte, m_bytesize>();
	VMType ty = static_cast<VMType>(tyval);

	t_data dat;

	switch(ty)
	{
		case VMType::REAL:
		{
			dat = t_data{std::in_place_index<m_realidx>,
				PopRaw<t_real, GetDataTypeSize<t_real>()>()};
			if(m_debug)
			{
				std::cout << "popped real " << std::get<m_realidx>(dat)
					<< "." << std::endl;
			}
			break;
		}

		case VMType::INT:
		{
			dat = t_data{std::in_place_index<m_intidx>,
				PopRaw<t_int, GetDataTypeSize<t_int>()>()};
			if(m_debug)
			{
				std::cout << "popped integer " << std::get<m_intidx>(dat)
					<< "." << std::endl;
			}
			break;
		}

		case VMType::CPLX:
		{
			dat = t_data{std::in_place_index<m_cplxidx>, PopComplex()};
			if(m_debug)
			{
				std::cout << "popped complex " << std::get<m_cplxidx>(dat)
					<< "." << std::endl;
			}
			break;
		}


		case VMType::BOOL:
		{
			dat = t_data{std::in_place_index<m_boolidx>,
				PopRaw<t_bool, GetDataTypeSize<t_bool>()>()};
			if(m_debug)
			{
				std::cout << "popped bool " << std::boolalpha
					<< (std::get<m_boolidx>(dat) != 0)
					<< "." << std::endl;
			}
			break;
		}

		case VMType::ADDR_MEM:
		case VMType::ADDR_IP:
		case VMType::ADDR_SP:
		case VMType::ADDR_BP:
		case VMType::ADDR_GBP:
		{
			dat = t_data{std::in_place_index<m_addridx>,
				PopRaw<t_addr, m_addrsize>()};
			if(m_debug)
			{
				std::cout << "popped address " << std::get<m_addridx>(dat)
					<< "." << std::endl;
			}
			break;
		}

		case VMType::STR:
		{
			dat = t_data{std::in_place_index<m_stridx>, PopString()};
			if(m_debug)
			{
				std::cout << "popped string \"" << std::get<m_stridx>(dat)
					<< "\"." << std::endl;
			}
			break;
		}

		case VMType::REALARR:
		{
			dat = t_data{std::in_place_index<m_realarridx>, PopVector<t_vec_real>()};
			if(m_debug)
			{
				using namespace m_ops;
				std::cout << "popped vector " << std::get<m_realarridx>(dat)
					<< "." << std::endl;
			}
			break;
		}

		default:
		{
			std::ostringstream msg;
			msg << "Pop: Data type " << (int)tyval
				<< " (" << get_vm_type_name(ty) << ")"
				<< " not yet implemented.";
			throw std::runtime_error(msg.str());
			break;
		}
	}

	return dat;
}


/**
 * push the raw data followed by a data type descriptor
 */
void VM::PushData(const VM::t_data& data, VMType ty, bool err_on_unknown)
{
	// real data
	if(data.index() == m_realidx)
	{
		// push the actual data
		PushRaw<t_real, GetDataTypeSize<t_real>()>(std::get<m_realidx>(data));

		// push descriptor
		PushRaw<t_byte, m_bytesize>(static_cast<t_byte>(VMType::REAL));

		if(m_debug)
		{
			std::cout << "pushed real "
				<< std::get<m_realidx>(data) << "."
				<< std::endl;
		}
	}

	// integer data
	else if(data.index() == m_intidx)
	{
		// push the actual data
		PushRaw<t_int, GetDataTypeSize<t_int>()>(std::get<m_intidx>(data));

		// push descriptor
		PushRaw<t_byte, m_bytesize>(static_cast<t_byte>(VMType::INT));

		if(m_debug)
		{
			std::cout << "pushed integer "
				<< std::get<m_intidx>(data) << "."
				<< std::endl;
		}
	}

	// complex data
	else if(data.index() == m_cplxidx)
	{
		// push the actual complex number
		PushComplex(std::get<m_cplxidx>(data));

		// push descriptor
		PushRaw<t_byte, m_bytesize>(static_cast<t_byte>(VMType::CPLX));

		if(m_debug)
		{
			std::cout << "pushed complex "
				<< std::get<m_cplxidx>(data) << "."
				<< std::endl;
		}
	}

	// bool data
	else if(data.index() == m_boolidx)
	{
		// push the actual data
		PushRaw<t_bool, GetDataTypeSize<t_bool>()>(std::get<m_boolidx>(data));

		// push descriptor
		PushRaw<t_byte, m_bytesize>(static_cast<t_byte>(VMType::BOOL));

		if(m_debug)
		{
			std::cout << "pushed bool " << std::boolalpha
				<< (std::get<m_boolidx>(data) != 0) << "."
				<< std::endl;
		}
	}

	// address data
	else if(data.index() == m_addridx)
	{
		// push the actual address
		PushRaw<t_addr, m_addrsize>(std::get<m_addridx>(data));

		// push descriptor
		PushRaw<t_byte, m_bytesize>(static_cast<t_byte>(ty));

		if(m_debug)
		{
			std::cout << "pushed address "
				<< std::get<m_addridx>(data) << "."
				<< std::endl;
		}
	}

	// string data
	else if(data.index() == m_stridx)
	{
		// push the actual string
		PushString(std::get<m_stridx>(data));

		// push descriptor
		PushRaw<t_byte, m_bytesize>(static_cast<t_byte>(VMType::STR));

		if(m_debug)
		{
			std::cout << "pushed string \""
				<< std::get<m_stridx>(data) << "\"."
				<< std::endl;
		}
	}

	// vector data
	else if(data.index() == m_realarridx)
	{
		// push the actual vector
		PushVector(std::get<m_realarridx>(data));

		// push descriptor
		PushRaw<t_byte, m_bytesize>(static_cast<t_byte>(VMType::REALARR));

		if(m_debug)
		{
			using namespace m_ops;
			std::cout << "pushed vector "
				<< std::get<m_realarridx>(data) << "."
				<< std::endl;
		}
	}

	// unknown data
	else if(err_on_unknown)
	{
		std::ostringstream msg;
		msg << "Push: Data type " << (int)ty
			<< " (" << get_vm_type_name(ty) << ")"
			<< " not yet implemented.";
		throw std::runtime_error(msg.str());
	}
}


/**
 * read the data type prefix from data in memory
 */
VMType VM::ReadMemType(VM::t_addr addr)
{
	// get data type info from memory
	t_byte tyval = ReadMemRaw<t_byte>(addr);
	return static_cast<VMType>(tyval);
}


/**
 * read type-prefixed data from memory
 */
std::tuple<VMType, VM::t_data> VM::ReadMemData(VM::t_addr addr)
{
	// get data type info from memory
	VMType ty = ReadMemType(addr);
	addr += m_bytesize;

	t_data dat;

	switch(ty)
	{
		case VMType::REAL:
		{
			t_real val = ReadMemRaw<t_real>(addr);
			dat = t_data{std::in_place_index<m_realidx>, val};

			if(m_debug)
			{
				std::cout << "read real " << val
					<< " from address " << (addr - 1)
					<< "." << std::endl;
			}
			break;
		}

		case VMType::INT:
		{
			t_int val = ReadMemRaw<t_int>(addr);
			dat = t_data{std::in_place_index<m_intidx>, val};

			if(m_debug)
			{
				std::cout << "read integer " << val
					<< " from address " << (addr - 1)
					<< "." << std::endl;
			}
			break;
		}

		case VMType::CPLX:
		{
			t_cplx val = ReadMemRaw<t_cplx>(addr);
			dat = t_data{std::in_place_index<m_cplxidx>, val};

			if(m_debug)
			{
				std::cout << "read complex " << val
					<< " from address " << (addr - 1)
					<< "." << std::endl;
			}
			break;
		}

		case VMType::BOOL:
		{
			t_bool val = ReadMemRaw<t_bool>(addr);
			dat = t_data{std::in_place_index<m_boolidx>, val};

			if(m_debug)
			{
				std::cout << "read bool " << std::boolalpha << (val != 0)
					<< " from address " << (addr - 1)
					<< "." << std::endl;
			}
			break;
		}

		case VMType::ADDR_MEM:
		case VMType::ADDR_IP:
		case VMType::ADDR_SP:
		case VMType::ADDR_BP:
		case VMType::ADDR_GBP:
		{
			t_addr val = ReadMemRaw<t_addr>(addr);
			dat = t_data{std::in_place_index<m_addridx>, val};

			if(m_debug)
			{
				std::cout << "read address " << t_int(val)
					<< " from address " << t_int(addr - 1)
					<< "." << std::endl;
			}
			break;
		}

		case VMType::STR:
		{
			t_str str = ReadMemRaw<t_str>(addr);
			dat = t_data{std::in_place_index<m_stridx>, str};

			if(m_debug)
			{
				std::cout << "read string \"" << str
					<< "\" from address " << (addr - 1)
					<< "." << std::endl;
			}
			break;
		}

		case VMType::REALARR:
		{
			t_vec_real vec = ReadMemRaw<t_vec_real>(addr);
			dat = t_data{std::in_place_index<m_realarridx>, vec};

			if(m_debug)
			{
				using namespace m_ops;
				std::cout << "read vector \"" << vec
					<< "\" from address " << (addr - 1)
					<< "." << std::endl;
			}
			break;
		}

		default:
		{
			std::ostringstream msg;
			msg << "ReadMem at address " << (addr - 1)
				<< ": Data type " << (int)ty
				<< " (" << get_vm_type_name(ty) << ")"
				<< " not yet implemented.";
			throw std::runtime_error(msg.str());
			break;
		}
	}

	return std::make_tuple(ty, dat);
}


/**
 * write type-prefixed data to memory
 */
void VM::WriteMemData(VM::t_addr addr, const VM::t_data& data)
{
	// real type
	if(data.index() == m_realidx)
	{
		if(m_debug)
		{
			std::cout << "writing real value "
				<< std::get<m_realidx>(data)
				<< " to address " << addr
				<< "." << std::endl;
		}

		// write descriptor prefix
		WriteMemRaw<t_byte>(addr, static_cast<t_byte>(VMType::REAL));
		addr += m_bytesize;

		// write the actual data
		WriteMemRaw<t_real>(addr, std::get<m_realidx>(data));
	}

	// integer type
	else if(data.index() == m_intidx)
	{
		if(m_debug)
		{
			std::cout << "writing integer value "
				<< std::get<m_intidx>(data)
				<< " to address " << addr
				<< "." << std::endl;
		}

		// write descriptor prefix
		WriteMemRaw<t_byte>(addr, static_cast<t_byte>(VMType::INT));
		addr += m_bytesize;

		// write the actual data
		WriteMemRaw<t_int>(addr, std::get<m_intidx>(data));
	}

	// complex type
	else if(data.index() == m_cplxidx)
	{
		if(m_debug)
		{
			std::cout << "writing complex "
				<< std::get<m_cplxidx>(data)
				<< " to address " << addr
				<< "." << std::endl;
		}

		// write descriptor prefix
		WriteMemRaw<t_byte>(addr, static_cast<t_byte>(VMType::CPLX));
		addr += m_bytesize;

		// write the actual data
		WriteMemRaw<t_cplx>(addr, std::get<m_cplxidx>(data));
	}

	// bool type
	else if(data.index() == m_boolidx)
	{
		if(m_debug)
		{
			std::cout << "writing bool value " << std::boolalpha
				<< (std::get<m_boolidx>(data) != 0)
				<< " to address " << addr
				<< "." << std::endl;
		}

		// write descriptor prefix
		WriteMemRaw<t_byte>(addr, static_cast<t_byte>(VMType::BOOL));
		addr += m_bytesize;

		// write the actual data
		WriteMemRaw<t_bool>(addr, std::get<m_boolidx>(data));
	}

	// address type
	/*else if(data.index() == m_addridx)
	{
		if(m_debug)
		{
			std::cout << "writing address value "
				<< std::get<m_addridx>(data)
				<< " to address " << addr
				<< std::endl;
		}

		// write descriptor prefix
		WriteMemRaw<t_byte>(addr, static_cast<t_byte>(ty));
		addr += m_bytesize;

		// write the actual data
		WriteMemRaw<t_int>(addr, std::get<m_addridx>(data));
	}*/

	// string type
	else if(data.index() == m_stridx)
	{
		if(m_debug)
		{
			std::cout << "writing string \""
				<< std::get<m_stridx>(data)
				<< "\" to address " << addr
				<< "." << std::endl;
		}

		// write descriptor prefix
		WriteMemRaw<t_byte>(addr, static_cast<t_byte>(VMType::STR));
		addr += m_bytesize;

		// write the actual data
		WriteMemRaw<t_str>(addr, std::get<m_stridx>(data));
	}

	// vector type
	else if(data.index() == m_realarridx)
	{
		if(m_debug)
		{
			using namespace m_ops;
			std::cout << "writing vector \""
				<< std::get<m_realarridx>(data)
				<< "\" to address " << addr
				<< "." << std::endl;
		}

		// write descriptor prefix
		WriteMemRaw<t_byte>(addr, static_cast<t_byte>(VMType::REALARR));
		addr += m_bytesize;

		// write the actual data
		WriteMemRaw<t_vec_real>(addr, std::get<m_realarridx>(data));
	}

	// unknown type
	else
	{
		throw std::runtime_error("WriteMem: Data type not yet implemented.");
	}
}


/**
 * helper function to get (possibly dynamic) data type sizes
 */
VM::t_addr VM::GetDataSize(const t_data& data) const
{
	if(data.index() == m_realidx)
		return GetDataTypeSize<t_real>();
	else if(data.index() == m_intidx)
		return GetDataTypeSize<t_int>();
	else if(data.index() == m_cplxidx)
		return GetDataTypeSize<t_cplx>();
	else if(data.index() == m_boolidx)
		return GetDataTypeSize<t_bool>();
	else if(data.index() == m_addridx)
		return m_addrsize;
	else if(data.index() == m_stridx)
		return m_addrsize + std::get<m_stridx>(data).length();

	throw std::runtime_error("GetDataSize: Data type not yet implemented.");
	return 0;
}


void VM::Reset()
{
	m_ip = 0;
	m_sp = m_memsize;
	m_bp = m_memsize;
	m_gbp = m_memsize;
	// padding of max. data type size to avoid writing beyond memory size
	m_sp -= sizeof(t_data) + 1;

	std::memset(m_mem.get(), static_cast<t_byte>(OpCode::HALT), m_memsize*m_bytesize);
	m_code_range[0] = m_code_range[1] = -1;
}


/**
 * sets or updates the range of memory where executable code resides
 */
void VM::UpdateCodeRange(t_addr begin, t_addr end)
{
	if(m_code_range[0] < 0 || m_code_range[1] < 0)
	{
		// set range
		m_code_range[0] = begin;
		m_code_range[1] = end;
	}
	else
	{
		// update range
		m_code_range[0] = std::min(m_code_range[0], begin);
		m_code_range[1] = std::max(m_code_range[1], end);
	}
}


void VM::SetMem(t_addr addr, VM::t_byte data)
{
	CheckMemoryBounds(addr, m_bytesize);

	m_mem[addr % m_memsize] = data;
}


void VM::SetMem(t_addr addr, const t_str& data, bool is_code)
{
	if(is_code)
		UpdateCodeRange(addr, addr + data.size());

	for(std::size_t i = 0; i < data.size(); ++i)
		SetMem(addr + t_addr(i), static_cast<t_byte>(data[i]));
}


void VM::SetMem(t_addr addr, const VM::t_byte* data, std::size_t size, bool is_code)
{
	if(is_code)
		UpdateCodeRange(addr, addr + size);

	for(std::size_t i = 0; i < size; ++i)
		SetMem(addr + t_addr(i), data[i]);
}


const char* VM::GetDataTypeName(std::size_t type_idx)
{
	switch(type_idx)
	{
		case m_realidx: return "real";
		case m_intidx: return "integer";
		case m_cplxidx: return "complex";
		case m_boolidx: return "bool";

		case m_addridx: return "address";

		case m_realarridx: return "array_real";

		case m_stridx: return "string";

		default: return "unknown";
	}
}


const char* VM::GetDataTypeName(const t_data& dat)
{
	return GetDataTypeName(dat.index());
}


void VM::CheckMemoryBounds(t_addr addr, t_addr size) const
{
	if(!m_checks)
		return;

	t_addr new_addr = addr + size;
	if(new_addr > m_memsize || new_addr < 0 || addr < 0)
	{
		std::ostringstream msg;
		msg << "Attempted memory access out of bounds: "
			<< addr << " + " << size << " = " << new_addr
			<< " > " << m_memsize << ".";
		throw std::runtime_error(msg.str());
	}
}


void VM::CheckPointerBounds() const
{
	if(!m_checks)
		return;

	// check code range?
	bool chk_c = (m_code_range[0] >= 0 && m_code_range[1] >= 0);

	if(m_ip > m_memsize || m_ip < 0 || (chk_c && (m_ip < m_code_range[0] || m_ip >= m_code_range[1])))
	{
		std::ostringstream msg;
		msg << "Instruction pointer " << t_int(m_ip) << " is out of memory bounds.";
		throw std::runtime_error(msg.str());
	}
	if(m_sp > m_memsize || m_sp < 0 || (chk_c && m_sp >= m_code_range[0] && m_sp < m_code_range[1]))
	{
		std::ostringstream msg;
		msg << "Stack pointer " << t_int(m_sp) << " is out of memory bounds.";
		throw std::runtime_error(msg.str());
	}
	if(m_bp > m_memsize || m_bp < 0 || (chk_c && m_bp >= m_code_range[0] && m_bp < m_code_range[1]))
	{
		std::ostringstream msg;
		msg << "Base pointer " << t_int(m_bp) << " is out of memory bounds.";
		throw std::runtime_error(msg.str());
	}
	if(m_gbp > m_memsize || m_gbp < 0 || (chk_c && m_gbp >= m_code_range[0] && m_gbp < m_code_range[1]))
	{
		std::ostringstream msg;
		msg << "Global base pointer " << t_int(m_gbp) << " is out of memory bounds.";
		throw std::runtime_error(msg.str());
	}
}
