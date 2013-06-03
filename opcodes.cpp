#include <map>
#include <iostream>
#include "opcodes.h"

std::string getString(Mnemonic mnemonic) {
	//std::cout << "abandon hope, all ye who enter here." << std::endl;
	auto m = std::map<Mnemonic, std::string> {
		{Mnemonic::DEX, "DEX"},{Mnemonic::AXS, "AXS"},{Mnemonic::AAC, "AAC"},
		{Mnemonic::BVS, "BVS"},{Mnemonic::DEY, "DEY"},{Mnemonic::RTI, "RTI"},
		{Mnemonic::BPL, "BPL"},{Mnemonic::CPX, "CPX"},{Mnemonic::EOR, "EOR"},
		{Mnemonic::AXA, "AXA"},{Mnemonic::TXA, "TXA"},{Mnemonic::TSX, "TSX"},
		{Mnemonic::ISB, "ISB"},{Mnemonic::DEC, "DEC"},{Mnemonic::ATX, "ATX"},
		{Mnemonic::CMP, "CMP"},{Mnemonic::ASR, "ASR"},{Mnemonic::LDA, "LDA"},
		{Mnemonic::BEQ, "BEQ"},{Mnemonic::ROL, "ROL"},{Mnemonic::TAS, "TAS"},
		{Mnemonic::CLI, "CLI"},{Mnemonic::JMP, "JMP"},{Mnemonic::TAX, "TAX"},
		{Mnemonic::TAY, "TAY"},{Mnemonic::RRA, "RRA"},{Mnemonic::STY, "STY"},
		{Mnemonic::SAX, "SAX"},{Mnemonic::LSR, "LSR"},{Mnemonic::RTS, "RTS"},
		{Mnemonic::BRK, "BRK"},{Mnemonic::SEC, "SEC"},{Mnemonic::ROR, "ROR"},
		{Mnemonic::LDX, "LDX"},{Mnemonic::DCP, "DCP"},{Mnemonic::ASL, "ASL"},
		{Mnemonic::STA, "STA"},{Mnemonic::SRE, "SRE"},{Mnemonic::CLD, "CLD"},
		{Mnemonic::SYA, "SYA"},{Mnemonic::CLC, "CLC"},{Mnemonic::BIT, "BIT"},
		{Mnemonic::SLO, "SLO"},{Mnemonic::ADC, "ADC"},{Mnemonic::XAA, "XAA"},
		{Mnemonic::CLV, "CLV"},{Mnemonic::STX, "STX"},{Mnemonic::LDY, "LDY"},
		{Mnemonic::BNE, "BNE"},{Mnemonic::INY, "INY"},{Mnemonic::AND, "AND"},
		{Mnemonic::JSR, "JSR"},{Mnemonic::ARR, "ARR"},{Mnemonic::PHP, "PHP"},
		{Mnemonic::INX, "INX"},{Mnemonic::SXA, "SXA"},{Mnemonic::CPY, "CPY"},
		{Mnemonic::PHA, "PHA"},{Mnemonic::PLP, "PLP"},{Mnemonic::RLA, "RLA"},
		{Mnemonic::TYA, "TYA"},{Mnemonic::BVC, "BVC"},{Mnemonic::SBC, "SBC"},
		{Mnemonic::LAX, "LAX"},{Mnemonic::SEI, "SEI"},{Mnemonic::PLA, "PLA"},
		{Mnemonic::KIL, "KIL"},{Mnemonic::SED, "SED"},{Mnemonic::INC, "INC"},
		{Mnemonic::NOP, "NOP"},{Mnemonic::LAR, "LAR"},{Mnemonic::ORA, "ORA"},
		{Mnemonic::BCS, "BCS"},{Mnemonic::BCC, "BCC"},{Mnemonic::BCC, "BMI"},
		{Mnemonic::TXS, "TXS"},{Mnemonic::BMI, "BMI"}
	};
	return m[mnemonic];
}

std::string getString(Addr_Mode mode) {
	return std::map<Addr_Mode, std::string> {
		{Addr_Mode::None,        "none       "},
		{Addr_Mode::Immediate,   "immediate  "},
		{Addr_Mode::Implied,     "implied    "},
		{Addr_Mode::Relative,    "relative   "},
		{Addr_Mode::Absolute,    "absolute   "},
		{Addr_Mode::Absolute_X,  "absolute_x "},
		{Addr_Mode::Absolute_Y,  "absolute_y "},
		{Addr_Mode::Indirect,    "indirect   "},
		{Addr_Mode::Zero_Page,   "zero_page  "},
		{Addr_Mode::Zero_Page_X, "zero_page_x"},
		{Addr_Mode::Zero_Page_Y, "zero_page_y"},
		{Addr_Mode::Indirect_X,  "indirect_x "},
		{Addr_Mode::Indirect_Y,  "indirect_y "},
		{Addr_Mode::Accumulator, "accumulator"}
	}[mode];
}

/*
constexpr Instruction::Instruction(Mnemonic mnemonic_, Addr_Mode mode_)
 : mnemonic(mnemonic_), mode(mode_) {};

constexpr Instruction inst_map[] {
	#include "opcodes.txt"
};*/

//constexpr Instruction::Instruction(uint8_t opcode)
// : mnemonic(inst_map[opcode].mnemonic), mode(inst_map[opcode].mode) {};



