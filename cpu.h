#ifndef CPU_H
#define CPU_H
#include "opcodes.h"
#include <utility>

namespace CPU {

namespace FLAG {
	enum {
		CARRY       = 1,
		ZERO        = 2,
		INT_DISABLE = 4,
		DECIMAL     = 8,
		BREAK       = 16,
		DUMMY       = 32,
		OVERFLOW    = 64,
		NEGATIVE    = 128
	};
}
struct Registers {
	uint8_t P, A, X, Y, S;
	uint16_t PC;
	int cycle;
	int scanline;

	void flagOn(uint8_t flag) {
		P |= flag;
	}
	void flagOff(uint8_t flag) {
		P &= ~flag;
	}
	void setFlag(uint8_t flag, bool set) {
		if(set)
			flagOn(flag);
		else
			flagOff(flag);
	}

	bool getFlag(uint8_t flag) {
		return P & flag;
	} 
};

void tick();

/* This code is a huge mess */
void print_opcode_bytes(uint16_t addr);
void print_inst_operand(uint16_t addr);
void print_regs(const Registers& reg);

//Reads!
template <Addr_Mode mode> uint8_t read(Registers& reg);
template <> uint8_t read<Addr_Mode::Accumulator > (Registers& reg);
template <> uint8_t read<Addr_Mode::Implied     > (Registers& reg);
template <> uint8_t read<Addr_Mode::Immediate   > (Registers& reg);
template <> uint8_t read<Addr_Mode::Absolute    > (Registers& reg);
template <> uint8_t read<Addr_Mode::Zero_Page   > (Registers& reg);
template <> uint8_t read<Addr_Mode::Zero_Page_X > (Registers& reg);
template <> uint8_t read<Addr_Mode::Zero_Page_Y > (Registers& reg);
template <> uint8_t read<Addr_Mode::Absolute_X  > (Registers& reg);
template <> uint8_t read<Addr_Mode::Absolute_Y  > (Registers& reg);

//Writes!
template <Addr_Mode mode> void write(Registers& reg, uint8_t data);
template <> void write<Addr_Mode::Zero_Page  >(Registers& reg, uint8_t data);
template <> void write<Addr_Mode::Absolute   >(Registers& reg, uint8_t data);
template <> void write<Addr_Mode::Zero_Page_X>(Registers& reg, uint8_t data);
template <> void write<Addr_Mode::Zero_Page_Y>(Registers& reg, uint8_t data);
template <> void write<Addr_Mode::Absolute_X >(Registers& reg, uint8_t data);
template <> void write<Addr_Mode::Absolute_Y >(Registers& reg, uint8_t data);

//Read-Writes!
typedef uint8_t(*modifyOp)(Registers&, uint8_t); //function pointers sure are weird!
typedef std::pair<uint8_t, uint8_t> modPair;
template <Addr_Mode mode> modPair modify(Registers& reg, modifyOp op);
template <> modPair modify<Addr_Mode::Zero_Page  >(Registers& reg, modifyOp op);
template <> modPair modify<Addr_Mode::Absolute   >(Registers& reg, modifyOp op);
template <> modPair modify<Addr_Mode::Zero_Page_X>(Registers& reg, modifyOp op);
template <> modPair modify<Addr_Mode::Zero_Page_Y>(Registers& reg, modifyOp op);
template <> modPair modify<Addr_Mode::Absolute_X >(Registers& reg, modifyOp op);
template <> modPair modify<Addr_Mode::Absolute_Y >(Registers& reg, modifyOp op);

} //end namespace

#endif
