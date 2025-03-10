/**
 * zero-address code generator
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 10-july-2022
 * @license see 'LICENSE' file
 */

#include "codegen.h"

#include <sstream>



//#define START_FUNC "start"  // start with the given entry-point
#define START_FUNC ""         // no start function => directly start in global scope


Codegen::Codegen(SymTab* syms, std::ostream* ostr)
	: m_syms{syms}, m_ostr{ostr}
{
	// dummy symbol for real constants
	m_real_const = new Symbol();
	m_real_const->ty = SymbolType::REAL;
	m_real_const->is_tmp = true;
	m_real_const->name = "<real>";

	// dummy symbol for int constants
	m_int_const = new Symbol();
	m_int_const->ty = SymbolType::INT;
	m_int_const->is_tmp = true;
	m_int_const->name = "<integer>";

	// dummy symbol for complex constants
	m_cplx_const = new Symbol();
	m_cplx_const->ty = SymbolType::CPLX;
	m_cplx_const->is_tmp = true;
	m_cplx_const->name = "<complex>";

	// dummy symbol for bool constants
	m_bool_const = new Symbol();
	m_bool_const->ty = SymbolType::BOOL;
	m_bool_const->is_tmp = true;
	m_bool_const->name = "<bool>";

	// dummy symbol for string constants
	m_str_const = new Symbol();
	m_str_const->ty = SymbolType::STRING;
	m_str_const->is_tmp = true;
	m_str_const->name = "<string>";

	// dummy symbol for real array constants
	m_real_array_const = new Symbol();
	m_real_array_const->ty = SymbolType::REAL_ARRAY;
	m_real_array_const->is_tmp = true;
	m_real_array_const->name = "<array_real>";

	// dummy symbol for int array constants
	m_int_array_const = new Symbol();
	m_int_array_const->ty = SymbolType::INT_ARRAY;
	m_int_array_const->is_tmp = true;
	m_int_array_const->name = "<array_integer>";

	// dummy symbol for complex array constants
	m_cplx_array_const = new Symbol();
	m_cplx_array_const->ty = SymbolType::CPLX_ARRAY;
	m_cplx_array_const->is_tmp = true;
	m_cplx_array_const->name = "<array_complex>";
}


Codegen::~Codegen()
{
	for(Symbol** sym : {
		&m_real_const, &m_int_const, &m_cplx_const,
		&m_real_array_const, &m_int_array_const, &m_cplx_array_const,
		&m_bool_const, &m_str_const, })
	{
		delete *sym;
		*sym = nullptr;
	}
}


/**
 * insert start-up code
 */
void Codegen::Start()
{
	// create global stack frame
	t_vm_int global_framesize = static_cast<t_vm_int>(GetStackFrameSize(nullptr));
	if(global_framesize > 0)
	{
		if(m_debug)
		{
			std::cout << "Global stack frame size: "
				<< global_framesize << " bytes."
				<< std::endl;
		}
		m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
		m_ostr->put(static_cast<t_vm_byte>(VMType::INT));
		m_ostr->write(reinterpret_cast<const char*>(&global_framesize), vm_type_size<VMType::INT, false>);
		m_ostr->put(static_cast<t_vm_byte>(OpCode::ADDFRAME));
	}

	const t_str funcname = START_FUNC;

	// no start function given
	if(funcname == "")
		return;

	// call a start function if given
	t_astret func = GetSym(funcname);
	if(!func)
		throw std::runtime_error("Start function is not in symbol table.");

	// create stack frame
	t_vm_int framesize = static_cast<t_vm_int>(GetStackFrameSize(func));
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	m_ostr->put(static_cast<t_vm_byte>(VMType::INT));
	m_ostr->write(reinterpret_cast<const char*>(&framesize), vm_type_size<VMType::INT, false>);
	m_ostr->put(static_cast<t_vm_byte>(OpCode::ADDFRAME));

	// push relative function address
	t_vm_addr func_addr = 0;  // to be filled in later
	m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
	m_ostr->put(static_cast<t_vm_byte>(VMType::ADDR_IP));
	// already skipped over address and jmp instruction
	std::streampos addr_pos = m_ostr->tellp();
	t_vm_addr to_skip = static_cast<t_vm_addr>(func_addr - addr_pos);
	to_skip -= vm_type_size<VMType::ADDR_IP, true>;
	m_ostr->write(reinterpret_cast<const char*>(&to_skip), vm_type_size<VMType::ADDR_IP, false>);

	// call the start function
	m_ostr->put(static_cast<t_vm_byte>(OpCode::CALL));

	// function address not yet known
	m_func_comefroms.emplace_back(
		std::make_tuple(funcname, addr_pos, 0, nullptr));

	// add a halt instruction
	m_ostr->put(static_cast<t_vm_byte>(OpCode::HALT));
}


/**
 * insert missing addresses and finalising code
 */
std::streampos Codegen::Finish()
{
	// remove global stack frame
	t_vm_int global_framesize = static_cast<t_vm_int>(GetStackFrameSize(nullptr));
	if(global_framesize > 0)
	{
		m_ostr->put(static_cast<t_vm_byte>(OpCode::PUSH));
		m_ostr->put(static_cast<t_vm_byte>(VMType::INT));
		m_ostr->write(reinterpret_cast<const char*>(&global_framesize), vm_type_size<VMType::INT, false>);
		m_ostr->put(static_cast<t_vm_byte>(OpCode::REMFRAME));
	}

	// add a final halt instruction
	m_ostr->put(static_cast<t_vm_byte>(OpCode::HALT));


	// write constants block
	std::streampos consttab_pos = m_ostr->tellp();
	if(auto [constsize, constbytes] = m_consttab.GetBytes(); constsize && constbytes)
	{
		m_ostr->write((char*)constbytes.get(), constsize);
	}

	// patch in the addresses of the constants
	for(auto [addr_pos, const_addr] : m_const_addrs)
	{
		// add address offset to constants table
		t_vm_addr addr = const_addr + consttab_pos;

		// write new address
		m_ostr->seekp(addr_pos);
		m_ostr->write(reinterpret_cast<const char*>(&addr),
			vm_type_size<VMType::ADDR_MEM, false>);
        }

	// patch in function addresses
	for(const auto& [func_name, pos, num_args, call_ast] : m_func_comefroms)
	{
		t_astret sym = GetSym(func_name);
		if(!sym)
			throw std::runtime_error("Tried to call unknown function \"" + func_name + "\".");
		if(!sym->addr)
			throw std::runtime_error("Function address for \"" + func_name + "\" not known.");

		t_vm_int func_num_args = static_cast<t_vm_int>(sym->argty.size());
		if(num_args != func_num_args)
		{
			std::ostringstream msg;
			msg << "Function \"" << func_name << "\" takes " << func_num_args
				<< " arguments, but " << num_args << " were given.";
			throw std::runtime_error(msg.str());
		}

		m_ostr->seekp(pos);

		// write relative function address
		t_vm_addr to_skip = static_cast<t_vm_addr>(*sym->addr - pos);
		// already skipped over address and jmp instruction
		to_skip -= vm_type_size<VMType::ADDR_IP, true>;
		m_ostr->write(reinterpret_cast<const char*>(&to_skip), vm_type_size<VMType::ADDR_IP, false>);
	}

	// patch in the jump addresses
	for(const auto& [ label, goto_pos ] : m_goto_comefroms)
	{
		auto iter_label = m_labels.find(label);
		if(iter_label == m_labels.end())
		{
			std::ostringstream msg;
			msg << "Label \"" << label << "\" not found.";
			throw std::runtime_error(msg.str());
		}

		std::streampos label_pos = iter_label->second;
		t_vm_addr to_skip = label_pos - goto_pos;

		// already skipped over address and jmp instruction
		to_skip -= vm_type_size<VMType::ADDR_IP, true>;
		m_ostr->seekp(goto_pos);
		m_ostr->write(reinterpret_cast<const char*>(&to_skip),
			vm_type_size<VMType::ADDR_IP, false>);
	}

	m_goto_comefroms.clear();
	m_labels.clear();

	// seek to end of stream
	m_ostr->seekp(0, std::ios_base::end);
	return m_ostr->tellp();
}
