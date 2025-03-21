/**
 * zero-address code vm, main execution loop
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 8-jun-2022
 * @license see 'LICENSE' file
 */

#include "vm.h"
#include "ops.h"
#include "mem.h"

#include <iostream>


bool VM::Run()
{
	bool running = true;
	std::size_t num_ops = 0;

	while(running)
	{
		CheckPointerBounds();
		if(m_drawmemimages)
			DrawMemoryImage();

		OpCode op{OpCode::INVALID};
		bool irq_active = false;

		// tests for interrupt requests
		for(t_addr irq = 0; irq < m_num_interrupts; ++irq)
		{
			if(!m_irqs[irq])
				continue;

			m_irqs[irq] = false;
			if(!m_isrs[irq])
				continue;

			irq_active = true;

			// call interrupt service routine
			PushAddress(*m_isrs[irq], VMType::ADDR_MEM);
			op = OpCode::CALL;

			// TODO: add specialised ICALL and IRET instructions
			// in case of additional registers that might need saving
			break;
		}

		if(!irq_active)
		{
			// fetch instruction
			t_byte _op = m_mem[m_ip++];
			op = static_cast<OpCode>(_op);
		}

		if(m_debug)
		{
			std::cout << "*** [" << num_ops << "] read instruction"
				<< " at ip = " << t_int(m_ip)
				<< ", sp = " << t_int(m_sp)
				<< ", bp = " << t_int(m_bp)
				<< ", gbp = " << t_int(m_gbp)
				<< ", opcode: " << std::hex
				<< static_cast<std::size_t>(op)
				<< " (" << get_vm_opcode_name(op) << ")"
				<< std::dec << ". ***" << std::endl;
		}

		// run instruction (as this is a zero-address machine without
		// general-purpose registers and only one direct data instruction
		// (push), we don't need an explicit decode step before)
		switch(op)
		{
			case OpCode::HALT:
			{
				running = false;
				break;
			}

			case OpCode::NOP:
			{
				break;
			}

			// ----------------------------------------------------
			// memory instructions
			// ----------------------------------------------------
			case OpCode::PUSH:  // push direct data onto stack
			{
				auto [ty, val] = ReadMemData(m_ip);
				m_ip += GetDataSize(val) + m_bytesize;
				PushData(val, ty);
				break;
			}

			case OpCode::WRMEM:
			{
				// variable address
				t_addr addr = PopAddress();

				// pop data and write it to memory
				t_data val = PopData();
				WriteMemData(addr, val);
				break;
			}

			case OpCode::RDMEM:
			{
				// variable address
				t_addr addr = PopAddress();

				// read and push data from memory
				auto [ty, val] = ReadMemData(addr);
				PushData(val, ty);
				break;
			}
			// ----------------------------------------------------

			// ----------------------------------------------------
			// array instructions
			// ----------------------------------------------------
			case OpCode::RDARR:  // read array element
			{
				t_int idx = std::get<m_intidx>(PopData());
				t_data arr = PopData();

				if(arr.index() == m_realarridx)
				{
					ReadArrayElem<t_vec_real>(arr, idx);
				}
				else if(arr.index() == m_intarridx)
				{
					ReadArrayElem<t_vec_int>(arr, idx);
				}
				else if(arr.index() == m_cplxarridx)
				{
					ReadArrayElem<t_vec_cplx>(arr, idx);
				}
				else if(arr.index() == m_quatarridx)
				{
					ReadArrayElem<t_vec_quat>(arr, idx);
				}
				else if(arr.index() == m_stridx)
				{
					// gets string element as substring
					const t_str& str = std::get<m_stridx>(arr);
					idx = safe_array_index<t_int>(idx, str.length());

					t_str newstr;
					newstr += str[idx];
					PushData(t_data{std::in_place_index<m_stridx>, newstr});
				}
				else
				{
					throw std::runtime_error("Cannot index non-array type.");
				}

				break;
			}

			case OpCode::RDARRR:  // read a range of array elements
			{
				t_int idx2 = std::get<m_intidx>(PopData());
				t_int idx1 = std::get<m_intidx>(PopData());
				t_data arr = PopData();

				if(arr.index() == m_realarridx)
				{
					ReadArrayElemRange<t_vec_real>(arr, idx1, idx2);
				}
				else if(arr.index() == m_intarridx)
				{
					ReadArrayElemRange<t_vec_int>(arr, idx1, idx2);
				}
				else if(arr.index() == m_cplxarridx)
				{
					ReadArrayElemRange<t_vec_cplx>(arr, idx1, idx2);
				}
				else if(arr.index() == m_quatarridx)
				{
					ReadArrayElemRange<t_vec_quat>(arr, idx1, idx2);
				}
				else if(arr.index() == m_stridx)
				{
					// gets string element as substring
					const t_str& str = std::get<m_stridx>(arr);
					idx1 = safe_array_index<t_int>(idx1, str.length());
					idx2 = safe_array_index<t_int>(idx2, str.length());

					t_int delta = (idx2 >= idx1 ? 1 : -1);
					idx2 += delta;

					t_str newstr;
					for(t_int idx=idx1; idx!=idx2; idx+=delta)
						newstr += str[idx];
					PushData(t_data{std::in_place_index<m_stridx>, newstr});
				}
				else
				{
					throw std::runtime_error("Cannot index non-array type.");
				}

				break;
			}

			case OpCode::WRARR:  // write an array element
			{
				t_int idx = std::get<m_intidx>(PopData());

				t_data data = PopData();
				t_addr addr = PopAddress();

				// get variable data type
				VMType ty = ReadMemType(addr);
				// skip type descriptor byte
				addr += m_bytesize;

				if(ty == VMType::REALARR)
					WriteArrayElem<t_vec_real>(addr, data, idx);
				else if(ty == VMType::INTARR)
					WriteArrayElem<t_vec_int>(addr, data, idx);
				else if(ty == VMType::CPLXARR)
					WriteArrayElem<t_vec_cplx>(addr, data, idx);
				else if(ty == VMType::QUATARR)
					WriteArrayElem<t_vec_quat>(addr, data, idx);
				else
					throw std::runtime_error("Cannot index non-array type.");

				break;
			}

			case OpCode::WRARRR:  // write a range of array elements
			{
				t_int idx2 = std::get<m_intidx>(PopData());
				t_int idx1 = std::get<m_intidx>(PopData());

				t_data data = PopData();
				t_addr addr = PopAddress();

				// get variable data type
				VMType ty = ReadMemType(addr);
				// skip type descriptor byte
				addr += m_bytesize;

				// lhs variable is a real array
				if(ty == VMType::REALARR)
				{
					WriteArrayElemRange<t_vec_real>(addr, data, idx1, idx2);
				}

				// lhs variable is an int array
				else if(ty == VMType::INTARR)
				{
					WriteArrayElemRange<t_vec_int>(addr, data, idx1, idx2);
				}

				// lhs variable is a complex array
				else if(ty == VMType::CPLXARR)
				{
					WriteArrayElemRange<t_vec_cplx>(addr, data, idx1, idx2);
				}

				// lhs variable is a quaternion array
				else if(ty == VMType::QUATARR)
				{
					WriteArrayElemRange<t_vec_quat>(addr, data, idx1, idx2);
				}

				// lhs variable is a string
				else if(ty == VMType::STR)
				{
					if(data.index() != m_stridx)
					{
						throw std::runtime_error(
							"String range has to be of string type.");
					}

					const t_str& rhsstr = std::get<m_stridx>(data);;

					// get array length indicator
					t_addr strlen = ReadMemRaw<t_addr>(addr);
					addr += m_addrsize;

					idx1 = safe_array_index<t_addr>(idx1, strlen);
					idx2 = safe_array_index<t_addr>(idx2, strlen);
					t_int delta = (idx2 >= idx1 ? 1 : -1);
					idx2 += delta;

					// skip to element range and overwrite it
					addr += idx1 * m_charsize;
					t_int cur_idx = 0;
					for(t_int idx = idx1; idx != idx2; idx += delta)
					{
						t_char elem{};

						if(std::size_t(cur_idx) >= rhsstr.length())
						{
							throw std::runtime_error(
								"String index out of bounds.");
						}

						elem = rhsstr[cur_idx++];

						WriteMemRaw(addr, elem);
						addr += m_charsize * delta;
					}
				}
				else
				{
					throw std::runtime_error("Cannot index non-array type.");
				}

				break;
			}

			case OpCode::MAKEREALARR:  // create a real array out of the elements on the stack
			{
				t_vec_real vec = PopArray<t_vec_real>(false);
				PushData(t_data{std::in_place_index<m_realarridx>, vec});
				break;
			}

			case OpCode::MAKEINTARR:  // create an int array out of the elements on the stack
			{
				t_vec_int vec = PopArray<t_vec_int>(false);
				PushData(t_data{std::in_place_index<m_intarridx>, vec});
				break;
			}

			case OpCode::MAKECPLXARR:  // create a complex array out of the elements on the stack
			{
				t_vec_cplx vec = PopArray<t_vec_cplx>(false);
				PushData(t_data{std::in_place_index<m_cplxarridx>, vec});
				break;
			}

			case OpCode::MAKEQUATARR:  // create a quaternion array out of the elements on the stack
			{
				t_vec_quat vec = PopArray<t_vec_quat>(false);
				PushData(t_data{std::in_place_index<m_quatarridx>, vec});
				break;
			}
			// ----------------------------------------------------

			// ----------------------------------------------------
			// arithmetic instructions
			// ----------------------------------------------------
			case OpCode::USUB:
			{
				t_data val = PopData();
				t_data result;

				if(val.index() == m_realidx)
				{
					result = t_data{std::in_place_index<m_realidx>,
						-std::get<m_realidx>(val)};
				}
				else if(val.index() == m_intidx)
				{
					result = t_data{std::in_place_index<m_intidx>,
						-std::get<m_intidx>(val)};
				}
				else if(val.index() == m_cplxidx)
				{
					result = t_data{std::in_place_index<m_cplxidx>,
						-std::get<m_cplxidx>(val)};
				}
				else if(val.index() == m_quatidx)
				{
					result = t_data{std::in_place_index<m_quatidx>,
						-std::get<m_quatidx>(val)};
				}
				else if(val.index() == m_realarridx)
				{
					using namespace m_ops;
					result = t_data{std::in_place_index<m_realarridx>,
						-std::get<m_realarridx>(val)};
				}
				else if(val.index() == m_intarridx)
				{
					using namespace m_ops;
					result = t_data{std::in_place_index<m_intarridx>,
						-std::get<m_intarridx>(val)};
				}
				else if(val.index() == m_cplxarridx)
				{
					using namespace m_ops;
					result = t_data{std::in_place_index<m_cplxarridx>,
						-std::get<m_cplxarridx>(val)};
				}
				else if(val.index() == m_quatarridx)
				{
					using namespace m_ops;
					result = t_data{std::in_place_index<m_quatarridx>,
						-std::get<m_quatarridx>(val)};
				}
				else
				{
					throw std::runtime_error(
						"Type mismatch in arithmetic operation.");
				}

				PushData(result);
				break;
			}

			case OpCode::ADD:
			{
				OpArithmetic<'+'>();
				break;
			}

			case OpCode::SUB:
			{
				OpArithmetic<'-'>();
				break;
			}

			case OpCode::MUL:
			{
				OpArithmetic<'*'>();
				break;
			}

			case OpCode::DIV:
			{
				OpArithmetic<'/'>();
				break;
			}

			case OpCode::MOD:
			{
				OpArithmetic<'%'>();
				break;
			}

			case OpCode::POW:
			{
				OpArithmetic<'^'>();
				break;
			}

			case OpCode::MATMUL:
			{
				OpMatrixMultiplication();
				break;
			}
			// ----------------------------------------------------

			// ----------------------------------------------------
			// logical instructions
			// ----------------------------------------------------
			case OpCode::AND:
			{
				OpLogical<'&'>();
				break;
			}

			case OpCode::OR:
			{
				OpLogical<'|'>();
				break;
			}

			case OpCode::XOR:
			{
				OpLogical<'^'>();
				break;
			}

			case OpCode::NOT:
			{
				// pop old value
				bool boolval = PopBool();

				// push new value
				PushBool(!boolval);
				break;
			}

			case OpCode::GT:
			{
				OpComparison<OpCode::GT>();
				break;
			}

			case OpCode::LT:
			{
				OpComparison<OpCode::LT>();
				break;
			}

			case OpCode::GEQU:
			{
				OpComparison<OpCode::GEQU>();
				break;
			}

			case OpCode::LEQU:
			{
				OpComparison<OpCode::LEQU>();
				break;
			}

			case OpCode::EQU:
			{
				OpComparison<OpCode::EQU>();
				break;
			}

			case OpCode::NEQU:
			{
				OpComparison<OpCode::NEQU>();
				break;
			}
			// ----------------------------------------------------

			// ----------------------------------------------------
			// binary instructions
			// ----------------------------------------------------
			case OpCode::BINAND:
			{
				OpBinary<'&'>();
				break;
			}

			case OpCode::BINOR:
			{
				OpBinary<'|'>();
				break;
			}

			case OpCode::BINXOR:
			{
				OpBinary<'^'>();
				break;
			}

			case OpCode::BINNOT:
			{
				t_data val = PopData();
				if(val.index() == m_intidx)
				{
					t_int newval = ~std::get<m_intidx>(val);
					PushData(t_data{std::in_place_index<m_intidx>, newval});
				}
				else
				{
					throw std::runtime_error("Invalid data type for binary not.");
				}

				break;
			}

			case OpCode::SHL:
			{
				OpBinary<'<'>();
				break;
			}

			case OpCode::SHR:
			{
				OpBinary<'>'>();
				break;
			}

			case OpCode::ROTL:
			{
				OpBinary<'l'>();
				break;
			}

			case OpCode::ROTR:
			{
				OpBinary<'r'>();
				break;
			}
			// ----------------------------------------------------

			// ----------------------------------------------------
			// type casts
			// ----------------------------------------------------
			case OpCode::TOR: // converts value to t_real
			{
				OpCast<m_realidx>();
				break;
			}

			case OpCode::TOI: // converts value to t_int
			{
				OpCast<m_intidx>();
				break;
			}

			case OpCode::TOC: // converts value to t_cplx
			{
				OpCast<m_cplxidx>();
				break;
			}

			case OpCode::TOQ: // converts value to t_quat
			{
				OpCast<m_quatidx>();
				break;
			}

			case OpCode::TOB: // converts value to t_bool
			{
				OpCast<m_boolidx>();
				break;
			}

			case OpCode::TOS: // converts value to t_str
			{
				OpCast<m_stridx>();
				break;
			}

			case OpCode::TOREALARR: // converts value to t_vec_real
			{
				t_addr vec_size = PopAddress();
				OpCastToArray<t_vec_real>(vec_size);
				break;
			}

			case OpCode::TOINTARR: // converts value to t_vec_int
			{
				t_addr vec_size = PopAddress();
				OpCastToArray<t_vec_int>(vec_size);
				break;
			}

			case OpCode::TOCPLXARR: // converts value to t_vec_cplx
			{
				t_addr vec_size = PopAddress();
				OpCastToArray<t_vec_cplx>(vec_size);
				break;
			}

			case OpCode::TOQUATARR: // converts value to t_vec_quat
			{
				t_addr vec_size = PopAddress();
				OpCastToArray<t_vec_quat>(vec_size);
				break;
			}
			// ----------------------------------------------------

			// ----------------------------------------------------
			// jumps and function calls
			// ----------------------------------------------------
			case OpCode::JMP: // jump to direct address
			{
				// get address from stack and set ip
				m_ip = PopAddress();
				break;
			}

			case OpCode::JMPCND: // conditional jump to direct address
			{
				// get address from stack
				t_addr addr = PopAddress();

				// get boolean condition result from stack
				bool boolcond = PopBool();

				if(m_debug)
				{
					if(!boolcond)
						std::cout << "no ";
					std::cout << "conditional jump to address " << addr
						<< "." << std::endl;
				}

				// set instruction pointer
				if(boolcond)
					m_ip = addr;
				break;
			}

			/**
			 * stack frame for functions:
			 *
			 *  --------------------
			 * |  local var n       |  <-- m_sp
			 *  --------------------      |
			 * |      ...           |     |
			 *  --------------------      |
			 * |  local var 2       |     |  framesize
			 *  --------------------      |
			 * |  local var 1       |     |
			 *  --------------------      |
			 * |  old m_bp          |  <-- m_bp (= previous m_sp)
			 *  --------------------
			 * |  old m_ip for ret  |
			 *  --------------------
			 * |  func. arg 1       |
			 *  --------------------
			 * |  func. arg 2       |
			 *  --------------------
			 * |  ...               |
			 *  --------------------
			 * |  func. arg n       |
			 *  --------------------
			 */
			case OpCode::CALL: // function call
			{
				// get return address and frame size
				t_addr funcaddr = PopAddress();
				t_int framesize = std::get<m_intidx>(PopData());

				// save instruction and base pointer and
				// set up the function's stack frame for local variables
				PushAddress(m_ip, VMType::ADDR_MEM);
				PushAddress(m_bp, VMType::ADDR_MEM);

				if(m_debug)
				{
					std::cout << "saved base pointer " << m_bp
						<< "." << std::endl;
				}
				m_bp = m_sp;
				m_sp -= framesize;

				// jump to function
				m_ip = funcaddr;
				if(m_debug)
				{
					std::cout << "calling function " << funcaddr
						<< "." << std::endl;
				}
				break;
			}

			case OpCode::RET: // return from function
			{
				// get number of function arguments and frame size
				t_int num_args = std::get<m_intidx>(PopData());
				t_int framesize = std::get<m_intidx>(PopData());

				// if there are still values on the stack, use then as return values
				std::vector<t_data> retvals;
				while(m_sp + framesize < m_bp)
					retvals.push_back(PopData());

				// zero the stack frame
				if(m_zeropoppedvals)
					std::memset(m_mem.get() + m_sp, 0, (m_bp - m_sp)*m_bytesize);

				// remove the function's stack frame
				m_sp = m_bp;

				m_bp = PopAddress();
				m_ip = PopAddress();  // jump back

				if(m_debug)
				{
					std::cout << "restored base pointer " << m_bp
						<< "." << std::endl;
				}

				// remove function arguments from stack
				for(t_int arg = 0; arg < num_args; ++arg)
					PopData();

				for(const t_data& retval : retvals)
					PushData(retval, VMType::UNKNOWN, false);
				break;
			}

			case OpCode::EXTCALL: // external function call
			{
				// get function name
				const t_str/*&*/ funcname = std::get<m_stridx>(PopData());

				t_data retval = CallExternal(funcname);
				PushData(retval, VMType::UNKNOWN, false);
				break;
			}

			case OpCode::ADDFRAME: // create a stack frame
			{
				t_int framesize = std::get<m_intidx>(PopData());
				m_sp -= framesize;

				if(m_debug)
				{
					std::cout << "created stack frame of size "
						<< framesize << "." << std::endl;
				}
				break;
			}

			case OpCode::REMFRAME: // remove a stack frame
			{
				t_int framesize = std::get<m_intidx>(PopData());

				// zero the stack frame
				if(m_zeropoppedvals)
					std::memset(m_mem.get() + m_sp, 0, framesize*m_bytesize);

				m_sp += framesize;

				if(m_debug)
				{
					std::cout << "removed stack frame of size "
						<< framesize << "." << std::endl;
				}
				break;
			}
			// ----------------------------------------------------

			default:
			{
				std::cerr << "Error: Invalid instruction " << std::hex
					<< static_cast<t_addr>(op) << std::dec
					<< std::endl;
				return false;
			}
		}  // switch(op)
		++num_ops;

		// wrap around
		if(m_ip >= m_memsize)
		{
			m_ip %= m_memsize;

			if(m_debug)
			{
				std::cout << "ip wrapped around memory limit."
					<< std::endl;
			}
		}
	}  // while(running)

	if(m_debug)
	{
		std::cout << "Ran " << num_ops << " instructions." << std::endl;
	}

	return true;
}
