/**
 * asm generator and vm opcodes
 * @author Tobias Weber (orcid: 0000-0002-7230-1932)
 * @date 14-jun-2022
 * @license see 'LICENSE' file
 */

#ifndef __0ACVM_OPCODES_H__
#define __0ACVM_OPCODES_H__


#include "types.h"



enum class OpCode : t_vm_byte
{
	HALT        = 0x00,  // stop program
	NOP         = 0x01,  // no operation
	INVALID     = 0x02,  // invalid opcode

	// memory operations
	PUSH        = 0x10,  // push direct data
	WRMEM       = 0x11,  // write memory
	RDMEM       = 0x12,  // read memory

	// arithmetic operations
	USUB        = 0x20,  // unary -
	ADD         = 0x21,  // +
	SUB         = 0x22,  // -
	MUL         = 0x23,  // *
	DIV         = 0x24,  // /
	MOD         = 0x25,  // %
	POW         = 0x26,  // ^
	MATMUL      = 0x27,  // matrix multiplication

	// conversions
	TOR         = 0x31,  // cast to real
	TOI         = 0x32,  // cast to int
	TOC         = 0x33,  // cast to complex
	TOB         = 0x34,  // cast to bool
	TOS         = 0x35,  // cast to string

	// array conversions
	TOREALARR   = 0x41,  // cast to real array
	TOINTARR    = 0x42,  // cast to int array
	TOCPLXARR   = 0x43,  // cast to complex array

	// jumps
	JMP         = 0x50,  // unconditional jump
	JMPCND      = 0x51,  // conditional jump

	// logical operations
	AND         = 0x60,  // &&
	OR          = 0x61,  // ||
	XOR         = 0x62,  // ^
	NOT         = 0x63,  // !

	// comparisons
	GT          = 0x70,  // >
	LT          = 0x71,  // <
	GEQU        = 0x72,  // >=
	LEQU        = 0x73,  // <=
	EQU         = 0x74,  // ==
	NEQU        = 0x75,  // !=

	// function calls
	CALL        = 0x80,  // call function
	RET         = 0x81,  // return from function
	EXTCALL     = 0x82,  // call system function
	ADDFRAME    = 0x85,  // create stack frame
	REMFRAME    = 0x86,  // remove stack frame

	// binary operations
	BINAND      = 0x90,  // &
	BINOR       = 0x91,  // |
	BINXOR      = 0x92,  // ^
	BINNOT      = 0x93,  // ~
	SHL         = 0x94,  // <<
	SHR         = 0x95,  // >>
	ROTL        = 0x96,  // rotate left
	ROTR        = 0x97,  // rotate right

	// array operations
	MAKEREALARR = 0xa1,  // create a real array
	MAKEINTARR  = 0xa2,  // create an int array
	MAKECPLXARR = 0xa3,  // create a complex array
	MAKEQUATARR = 0xa4,  // create a quaternion array

	// array memory operations
	RDARR       = 0xb0,  // read element from an array type
	RDARRR      = 0xb1,  // read range from an array type
	WRARR       = 0xb4,  // write element to an array type
	WRARRR      = 0xb5,  // write range to an array type
};



/**
 * get a string representation of an opcode
 */
template<class t_str = const char*>
constexpr t_str get_vm_opcode_name(OpCode op)
{
	switch(op)
	{
		case OpCode::HALT:        return "halt";
		case OpCode::NOP:         return "nop";
		case OpCode::INVALID:     return "invalid";

		case OpCode::PUSH:        return "push";
		case OpCode::WRMEM:       return "wrmem";
		case OpCode::RDMEM:       return "rdmem";

		case OpCode::USUB:        return "usub";
		case OpCode::ADD:         return "add";
		case OpCode::SUB:         return "sub";
		case OpCode::MUL:         return "mul";
		case OpCode::DIV:         return "div";
		case OpCode::MOD:         return "mod";
		case OpCode::POW:         return "pow";
		case OpCode::MATMUL:      return "matmul";

		case OpCode::TOR:         return "tor";
		case OpCode::TOI:         return "toi";
		case OpCode::TOC:         return "toc";
		case OpCode::TOB:         return "tob";
		case OpCode::TOS:         return "tos";

		case OpCode::TOREALARR:   return "torealarr";
		case OpCode::TOINTARR:    return "tointarr";
		case OpCode::TOCPLXARR:   return "tocplxarr";

		case OpCode::JMP:         return "jmp";
		case OpCode::JMPCND:      return "jmpcnd";

		case OpCode::AND:         return "and";
		case OpCode::OR:          return "or";
		case OpCode::XOR:         return "xor";
		case OpCode::NOT:         return "not";

		case OpCode::GT:          return "gt";
		case OpCode::LT:          return "lt";
		case OpCode::GEQU:        return "gequ";
		case OpCode::LEQU:        return "lequ";
		case OpCode::EQU:         return "equ";
		case OpCode::NEQU:        return "nequ";

		case OpCode::CALL:        return "call";
		case OpCode::RET:         return "ret";
		case OpCode::EXTCALL:     return "extcall";
		case OpCode::ADDFRAME:    return "addframe";
		case OpCode::REMFRAME:    return "remframe";

		case OpCode::BINAND:      return "binand";
		case OpCode::BINOR:       return "binor";
		case OpCode::BINXOR:      return "binxor";
		case OpCode::BINNOT:      return "binnot";
		case OpCode::SHL:         return "shl";
		case OpCode::SHR:         return "shr";
		case OpCode::ROTL:        return "rotl";
		case OpCode::ROTR:        return "rotr";

		case OpCode::MAKEREALARR: return "makerealarr";
		case OpCode::MAKEINTARR:  return "makeintarr";
		case OpCode::MAKECPLXARR: return "makecplxarr";

		case OpCode::RDARR:       return "rdarr";
		case OpCode::RDARRR:      return "rdarrr";
		case OpCode::WRARR:       return "wrarr";
		case OpCode::WRARRR:      return "wrarrr";

		default:                  return "<unknown>";
	}
}


#endif
