/**
 * zero-address code vm, main execution loop
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 8-jun-2022
 * @license see 'LICENSE.GPL' file
 */

#include "vm.h"
#include <iostream>


bool VM::Run()
{
	bool running = true;
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
			t_byte _op = m_mem[m_ip++];
			op = static_cast<OpCode>(_op);
		}

		if(m_debug)
		{
			std::cout << "*** read instruction at ip = " << t_int(m_ip)
				<< ", sp = " << t_int(m_sp)
				<< ", bp = " << t_int(m_bp)
				<< ", opcode: " << std::hex
				<< static_cast<std::size_t>(op)
				<< " (" << get_vm_opcode_name(op) << ")"
				<< std::dec << ". ***" << std::endl;
		}

		// run instruction
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

			// push direct data onto stack
			case OpCode::PUSH:
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

			case OpCode::RDARR1D:
			{
				t_int idx = std::get<m_intidx>(PopData());
				t_data arr = PopData();

				if(arr.index() == m_vecidx)
				{
					// gets vector element
					const t_vec& vec = std::get<m_vecidx>(arr);
					idx = safe_array_index<t_int>(idx, vec.size());

					PushData(t_data{std::in_place_index<m_realidx>, vec[idx]});
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
				else if(arr.index() == m_matidx)
				{
					// gets matrix column
					const t_mat& mat = std::get<m_matidx>(arr);
					idx = safe_array_index<t_int>(idx, mat.size2());

					t_vec col = m::zero<t_vec>(mat.size1());
					for(std::size_t i=0; i<mat.size1(); ++i)
						col[i] = mat(i, idx);
					PushData(t_data{std::in_place_index<m_vecidx>, col});
				}
				else
				{
					throw std::runtime_error("Cannot index non-array type.");
				}

				break;
			}

			case OpCode::RDARR1DR:
			{
				t_int idx2 = std::get<m_intidx>(PopData());
				t_int idx1 = std::get<m_intidx>(PopData());
				t_data arr = PopData();

				if(arr.index() == m_vecidx)
				{
					// gets vector range
					const t_vec& vec = std::get<m_vecidx>(arr);
					idx1 = safe_array_index<t_int>(idx1, vec.size());
					idx2 = safe_array_index<t_int>(idx2, vec.size());

					t_int delta = (idx2 >= idx1 ? 1 : -1);
					idx2 += delta;

					t_vec newvec = m::zero<t_vec>(std::abs(idx2 - idx1));
					t_int new_idx = 0;
					for(t_int idx=idx1; idx!=idx2; idx+=delta)
						newvec[new_idx++] = vec[idx];
					PushData(t_data{std::in_place_index<m_vecidx>, newvec});
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
				else if(arr.index() == m_matidx)
				{
					// gets matrix columns
					const t_mat& mat = std::get<m_matidx>(arr);
					idx1 = safe_array_index<t_int>(idx1, mat.size2());
					idx2 = safe_array_index<t_int>(idx2, mat.size2());

					t_int delta = (idx2 >= idx1 ? 1 : -1);
					idx2 += delta;

					t_mat cols = m::zero<t_mat, std::size_t>(mat.size1(), idx2 - idx1);
					for(t_int idx=idx1; idx!=idx2; idx += delta)
						for(std::size_t i=0; i<mat.size1(); i += delta)
							cols(i, idx) = mat(i, idx);
					PushData(t_data{std::in_place_index<m_matidx>, cols});
				}
				else
				{
					throw std::runtime_error("Cannot index non-array type.");
				}

				break;
			}

			case OpCode::RDARR2D:
			{
				t_int idx2 = std::get<m_intidx>(PopData());
				t_int idx1 = std::get<m_intidx>(PopData());
				t_data arr = PopData();

				if(arr.index() == m_matidx)
				{
					// gets matrix element
					const t_mat& mat = std::get<m_matidx>(arr);
					idx1 = safe_array_index<t_int>(idx1, mat.size2());
					idx2 = safe_array_index<t_int>(idx2, mat.size2()) + 1;

					PushData(t_data{std::in_place_index<m_realidx>, mat(idx1, idx2)});
				}
				else
				{
					throw std::runtime_error("Cannot double-index non-matrix type.");
				}

				break;
			}

			case OpCode::RDARR2DR:
			{
				t_int idx4 = std::get<m_intidx>(PopData());
				t_int idx3 = std::get<m_intidx>(PopData());
				t_int idx2 = std::get<m_intidx>(PopData());
				t_int idx1 = std::get<m_intidx>(PopData());
				t_data arr = PopData();

				if(arr.index() == m_matidx)
				{
					// gets matrix range
					const t_mat& mat = std::get<m_matidx>(arr);
					idx1 = safe_array_index<t_int>(idx1, mat.size1());
					idx2 = safe_array_index<t_int>(idx2, mat.size1());
					idx3 = safe_array_index<t_int>(idx3, mat.size2());
					idx4 = safe_array_index<t_int>(idx4, mat.size2());

					t_int delta1 = (idx2 >= idx1 ? 1 : -1);
					t_int delta2 = (idx4 >= idx3 ? 1 : -1);

					idx2 += delta1;
					idx4 += delta2;

					t_mat newmat = m::create<t_mat>(
						std::abs(idx2-idx1), std::abs(idx4-idx3));

					t_int new_i = 0;
					for(t_int i=idx1; i!=idx2; i+=delta1)
					{
						t_int new_j = 0;
						for(t_int j=idx3; j!=idx4; j+=delta2)
							newmat(new_i, new_j++) = mat(i, j);
						++new_i;
					}

					PushData(t_data{std::in_place_index<m_matidx>, newmat});
				}
				else
				{
					throw std::runtime_error("Cannot double-index non-matrix type.");
				}

				break;
			}

			case OpCode::WRARR1D:
			{
				t_int idx = std::get<m_intidx>(PopData());

				t_data data = PopData();
				t_addr addr = PopAddress();

				// get variable data type
				VMType ty = ReadMemType(addr);
				// skip type descriptor byte
				addr += m_bytesize;

				if(ty == VMType::VEC)
				{
					if(data.index() != m_realidx)
					{
						throw std::runtime_error(
							"Vector element has to be of scalar type.");
					}

					// get vector length indicator
					t_addr veclen = ReadMemRaw<t_addr>(addr);
					addr += m_addrsize;

					idx = safe_array_index<t_addr>(idx, veclen);

					// skip to element and overwrite it
					addr += idx * m_realsize;
					WriteMemRaw(addr, std::get<m_realidx>(data));
				}
				else
				{
					throw std::runtime_error("Cannot index non-array type.");
				}

				break;
			}

			case OpCode::WRARR2D:
			{
				t_int idx2 = std::get<m_intidx>(PopData());
				t_int idx1 = std::get<m_intidx>(PopData());

				t_data data = PopData();
				t_addr addr = PopAddress();

				// get variable data type
				VMType ty = ReadMemType(addr);
				// skip type descriptor byte
				addr += m_bytesize;

				if(ty == VMType::MAT)
				{
					if(data.index() != m_realidx)
					{
						throw std::runtime_error(
							"Matrix element has to be of scalar type.");
					}

					// get matrix length indicators
					t_addr num_rows = ReadMemRaw<t_addr>(addr);
					addr += m_addrsize;
					t_addr num_cols = ReadMemRaw<t_addr>(addr);
					addr += m_addrsize;

					idx1 = safe_array_index<t_addr>(idx1, num_rows);
					idx2 = safe_array_index<t_addr>(idx2, num_cols);

					// skip to element and overwrite it
					addr += (idx1*num_cols + idx2) * m_realsize;
					//std::cout << ReadMemRaw<t_real>(addr) << std::endl;
					WriteMemRaw(addr, std::get<m_realidx>(data));
				}
				else
				{
					throw std::runtime_error("Cannot double-index non-matrix type.");
				}

				break;
			}

			case OpCode::WRARR1DR:
			{
				t_int idx2 = std::get<m_intidx>(PopData());
				t_int idx1 = std::get<m_intidx>(PopData());

				t_data data = PopData();
				t_addr addr = PopAddress();

				// get variable data type
				VMType ty = ReadMemType(addr);
				// skip type descriptor byte
				addr += m_bytesize;

				// lhs variable is a vector
				if(ty == VMType::VEC)
				{
					const t_vec* rhsvec = nullptr;
					const t_real* rhsreal = nullptr;

					// rhs is a vector
					if(data.index() == m_vecidx)
					{
						rhsvec = &std::get<m_vecidx>(data);
					}
					// rhs is a scalar
					else if(data.index() == m_realidx)
					{
						rhsreal = &std::get<m_realidx>(data);
					}
					else
					{
						throw std::runtime_error(
							"Vector range has to be of vector or scalar type.");
					}

					// get vector length indicator
					t_addr veclen = ReadMemRaw<t_addr>(addr);
					addr += m_addrsize;

					idx1 = safe_array_index<t_addr>(idx1, veclen);
					idx2 = safe_array_index<t_addr>(idx2, veclen);
					t_int delta = (idx2 >= idx1 ? 1 : -1);
					idx2 += delta;

					// skip to element range and overwrite it
					addr += idx1 * m_realsize;
					t_int cur_idx = 0;
					for(t_int idx=idx1; idx!=idx2; idx+=delta)
					{
						t_real elem{};

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
						addr += m_realsize * delta;
					}
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

					// get vector length indicator
					t_addr strlen = ReadMemRaw<t_addr>(addr);
					addr += m_addrsize;

					idx1 = safe_array_index<t_addr>(idx1, strlen);
					idx2 = safe_array_index<t_addr>(idx2, strlen);
					t_int delta = (idx2 >= idx1 ? 1 : -1);
					idx2 += delta;

					// skip to element range and overwrite it
					addr += idx1 * m_charsize;
					t_int cur_idx = 0;
					for(t_int idx=idx1; idx!=idx2; idx+=delta)
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

			case OpCode::WRARR2DR:
			{
				t_int idx4 = std::get<m_intidx>(PopData());
				t_int idx3 = std::get<m_intidx>(PopData());
				t_int idx2 = std::get<m_intidx>(PopData());
				t_int idx1 = std::get<m_intidx>(PopData());

				t_data rhsdata = PopData();
				t_addr addr = PopAddress();

				// get variable data type
				VMType ty = ReadMemType(addr);
				// skip type descriptor byte
				addr += m_bytesize;

				// assign to matrix
				if(ty == VMType::MAT)
				{
					// get matrix length indicators
					t_addr num_rows = ReadMemRaw<t_addr>(addr);
					addr += m_addrsize;
					t_addr num_cols = ReadMemRaw<t_addr>(addr);
					addr += m_addrsize;

					idx1 = safe_array_index<t_addr>(idx1, num_rows);
					idx2 = safe_array_index<t_addr>(idx2, num_rows);
					idx3 = safe_array_index<t_addr>(idx3, num_cols);
					idx4 = safe_array_index<t_addr>(idx4, num_cols);

					t_int delta1 = (idx2 >= idx1 ? 1 : -1);
					t_int delta2 = (idx4 >= idx3 ? 1 : -1);

					idx2 += delta1;
					idx4 += delta2;

					// assign from scalar
					if(rhsdata.index() == m_realidx)
					{
						t_real rhsreal = std::get<m_realidx>(rhsdata);
						for(t_int i=idx1; i!=idx2; i+=delta1)
						{
							for(t_int j=idx3; j!=idx4; j+=delta2)
							{
								t_int elem_idx = i*num_cols + j;
								WriteMemRaw(addr + elem_idx*m_realsize, rhsreal);
							}
						}
					}

					// assign from vector
					else if(rhsdata.index() == m_vecidx)
					{
						const t_vec& rhsvec = std::get<m_vecidx>(rhsdata);

						t_addr vecidx = 0;
						for(t_int i=idx1; i!=idx2; i+=delta1)
						{
							for(t_int j=idx3; j!=idx4; j+=delta2)
							{
								if(std::size_t(vecidx) >= rhsvec.size())
								{
									throw std::runtime_error(
										"Vector index out of bounds.");
								}

								t_int elem_idx = i*num_cols + j;
								t_real elem = rhsvec[vecidx++];
								WriteMemRaw(addr + elem_idx*m_realsize, elem);
							}
						}
					}

					// assign from matrix
					else if(rhsdata.index() == m_matidx)
					{
						const t_mat& rhsmat = std::get<m_matidx>(rhsdata);

						t_int i_rhs = 0;
						for(t_int i=idx1; i!=idx2; i+=delta1)
						{
							t_int j_rhs = 0;
							for(t_int j=idx3; j!=idx4; j+=delta2)
							{
								if(std::size_t(i_rhs) >= rhsmat.size1() ||
									std::size_t(j_rhs) >= rhsmat.size2())
								{
									throw std::runtime_error(
										"Matrix index out of bounds.");
								}

								t_int elem_idx = i*num_cols + j;
								t_real elem = rhsmat(i_rhs, j_rhs);
								WriteMemRaw(addr + elem_idx*m_realsize, elem);
								++j_rhs;
							}
							++i_rhs;
						}
					}

					else
					{
						throw std::runtime_error(
							"Invalid matrix range assignment.");
					}
				}
				else
				{
					throw std::runtime_error("Cannot index non-array type.");
				}

				break;
			}

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
				else if(val.index() == m_vecidx)
				{
					using namespace m_ops;
					result = t_data{std::in_place_index<m_vecidx>,
						-std::get<m_vecidx>(val)};
				}
				else if(val.index() == m_matidx)
				{
					using namespace m_ops;
					result = t_data{std::in_place_index<m_matidx>,
						-std::get<m_matidx>(val)};
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
				// might also use PopData and PushData in case ints
				// should also be allowed in boolean expressions
				t_bool val = PopRaw<t_bool, m_boolsize>();
				PushRaw<t_bool, m_boolsize>(!val);
				break;
			}

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

			case OpCode::TOI: // converts value to t_int
			{
				OpCast<m_intidx>();
				break;
			}

			case OpCode::TOF: // converts value to t_real
			{
				OpCast<m_realidx>();
				break;
			}

			case OpCode::TOS: // converts value to t_str
			{
				OpCast<m_stridx>();
				break;
			}

			case OpCode::TOV: // converts value to t_vec
			{
				t_addr vec_size = PopAddress();
				OpArrayCast<m_vecidx>(vec_size);
				break;
			}

			case OpCode::TOM: // converts value to t_mat
			{
				t_addr size1 = PopAddress();
				t_addr size2 = PopAddress();
				OpArrayCast<m_matidx>(size1, size2);
				break;
			}

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
				t_bool cond = PopRaw<t_bool, m_boolsize>();

				// set instruction pointer
				if(cond)
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
					std::cout << "saved base pointer "
						<< m_bp << "."
						<< std::endl;
				}
				m_bp = m_sp;
				m_sp -= framesize;

				// jump to function
				m_ip = funcaddr;
				if(m_debug)
				{
					std::cout << "calling function "
						<< funcaddr << "."
						<< std::endl;
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
					std::memset(m_mem.get()+m_sp, 0, (m_bp-m_sp)*m_bytesize);

				// remove the function's stack frame
				m_sp = m_bp;

				m_bp = PopAddress();
				m_ip = PopAddress();  // jump back

				if(m_debug)
				{
					std::cout << "restored base pointer "
						<< m_bp << "."
						<< std::endl;
				}

				// remove function arguments from stack
				for(t_int arg=0; arg<num_args; ++arg)
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

			case OpCode::MAKEVEC:
			{
				t_vec vec = PopVector(false);
				PushData(t_data{std::in_place_index<m_vecidx>, vec});
				break;
			}

			case OpCode::MAKEMAT:
			{
				t_mat mat = PopMatrix(false);
				PushData(t_data{std::in_place_index<m_matidx>, mat});
				break;
			}

			default:
			{
				std::cerr << "Error: Invalid instruction " << std::hex
					<< static_cast<t_addr>(op) << std::dec
					<< std::endl;
				return false;
			}
		}

		// wrap around
		if(m_ip > m_memsize)
			m_ip %= m_memsize;
	}

	return true;
}
