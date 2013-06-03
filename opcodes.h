#ifndef OPCODES_H
#define OPCODES_H
#include <string>
#include <utility>

enum class Mnemonic {
	BRK = 1,  PHP = 2,  BPL = 3,  CLC = 4,  JSR = 5,  PLP = 6,
	SEC = 7,  RTI = 8,  PHA = 9,  JMP = 10, BVC = 11, CLI = 12,
	RTS = 13, PLA = 14, SEI = 15, DEY = 16, TYA = 17, SYA = 18,
	TAY = 19, CLV = 20, INY = 21, BNE = 22, CLD = 23, INX = 24,
	BEQ = 25, SED = 26, TXA = 27, SXA = 28, TAX = 29, TSX = 30,
	KIL = 31, NOP = 32, BIT = 33, STY = 34, LDY = 35, CPY = 36,
	CPX = 37, ASL = 38, ROL = 39, LSR = 40, ROR = 41, STX = 42,
	LDX = 43, DEC = 44, INC = 45, ORA = 46, AND = 47, EOR = 48,
	ADC = 49, STA = 50, LDA = 51, CMP = 52, SBC = 53, SLO = 54, TXS = 55,
	RLA = 56, SRE = 57, RRA = 58, SAX = 59, LAX = 60, DCP = 61, BMI = 62,
	ISB = 63, AAC = 64, ASR = 65, ARR = 66, XAA = 67, ATX = 68, BCC = 69,
	AXS = 70, AXA = 71, TAS = 72, LAR = 73, DEX = 74, BVS = 75, BCS = 76
};

enum class Addr_Mode {
	None = 0, Immediate = 1, Implied = 2, Relative = 3, Absolute = 4, 
	Absolute_X = 5, Absolute_Y = 6, Zero_Page = 7,
	Zero_Page_X = 8, Zero_Page_Y = 9, Indirect_X = 10, Indirect_Y = 11,
	Accumulator = 12, Indirect = 13
};

//Sorry for the mess, C++ hard D:
constexpr std::pair<Mnemonic, Addr_Mode> inst_map[] {
	#include "opcodes.txt"
};

struct Instruction {
	const Mnemonic mnemonic;
	const Addr_Mode mode;
	constexpr Instruction(Mnemonic mnemonic_, Addr_Mode mode_)
	 : mnemonic(mnemonic_), mode(mode_) {};
	constexpr Instruction(uint8_t opcode)
	 : Instruction(inst_map[opcode].first, inst_map[opcode].second) {};
};

std::string getString(Mnemonic mnemonic);
std::string getString(Addr_Mode mode);

#endif
