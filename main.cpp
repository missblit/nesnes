#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <array>
#include <utility>
#include <map>
#include <cassert>
#include "boost/preprocessor/iteration/local.hpp"
#include "opcodes.h"
#include "cpu.h"
#include "instructions.h"
#include "memory.h"


typedef unsigned char byte;

typedef std::array<byte, 16384> prg_rom_block;
typedef std::array<byte, 8192>  chr_rom_block;
struct ines_header {
	std::array<byte,4> magic_number;   // 0 - 3
	byte               prg_rom_blocks; // 4 
	byte               chr_rom_blocks; // 5
	byte               flags_6;        // 6
	byte               flags_7;        // 7
	byte               prg_ram_blocks; // 8
	byte               flags_9;        // 9
	byte               flags_10;       // 10
	std::array<byte,5> unused;         // 11 - 15
	bool has_magic_number() {
		return    magic_number[0] == 'N'
		       && magic_number[1] == 'E'
		       && magic_number[2] == 'S'
		       && magic_number[3] == 0x1A;
	}	
	byte mapper_num() {
		return (flags_7 & 0xF0 << 4) | flags_6 & 0xF0;
	}
};

struct ines_file {
	ines_header header;
	//ines_trainer trainer;
	std::vector<prg_rom_block> prg_rom;
	std::vector<chr_rom_block> chr_rom;

	std::istream& read(std::istream& in);
};

std::istream& ines_file::read(std::istream& in) {
	in.read( (char*)&header, sizeof(header) );
	if(!header.has_magic_number()) {
		in.setstate(std::ios::failbit);
		return in;
	}

	prg_rom.resize(header.prg_rom_blocks);
	in.read( (char*)prg_rom.data(), sizeof(prg_rom_block)*prg_rom.size());
	chr_rom.resize(header.chr_rom_blocks);
	in.read( (char*)chr_rom.data(), sizeof(chr_rom_block)*prg_rom.size());

	//std::cout << "prg_rom_blocks: " << int(header.prg_rom_blocks) << "\n";
	//std::cout << "chr_rom_blocks: " << int(header.chr_rom_blocks) << "\n";
	//std::cout << "mapper number: " << int(header.mapper_num()) << "\n";
	return in;
}

int main(int argc, char **argv) {
	using namespace std;
	assert(argc >= 2);
	ifstream romFile(argv[1], ios::in | ios::binary);
	ines_file f;
	f.read(romFile);

	//Load ROM into 0xC000 for now
	//TODO: add memory map and such
	/*vector<byte> */mem.resize(0x10000);
	int addr = 0xC000;
	for(prg_rom_block& b : f.prg_rom) {
		copy(b.begin(), b.end(), mem.data()+addr);
		addr += sizeof(prg_rom_block);
	}

	//initialize PC, TODO: use interrupt vector, initialize NES properly
	CPU::Registers reg;
	reg.PC = 0xC000;
	reg.A = 0;
	reg.X = 0;
	reg.Y = 0;
	reg.P = 0x24;
	reg.S = 0xFD;
	reg.cycle = 0;
	reg.scanline = 241;


	int count = 0;
    /* And here's the main emulation loop, fetches opcode and runs it */
	for(;;) {
		count++;
		const auto inst = Instruction(mem[reg.PC]);
		cout << uppercase << setfill('0') << setw(4) << hex
		     << int(reg.PC) << "  ";
		CPU::print_opcode_bytes(reg.PC);
		cout << getString(inst.mnemonic) << " ";
		CPU::print_inst_operand(reg.PC);
		print_regs(reg);
		//if(reg.PC == 0xF080) assert(false);
		cout << " " << count;
		cout << "\n";
		/* This switch statement has 256 cases, generated through pp black magic
           One for each opcode, that just calls the appropriate opcode function
         */
		switch(CPU::read<Addr_Mode::Immediate>(reg)) {
			#define BOOST_PP_LOCAL_LIMITS (0, 255)
			#define BOOST_PP_LOCAL_MACRO(n) \
			case n: \
				Operation<Instruction(n).mnemonic,Instruction(n).mode>()(reg); \
				break;
			#include BOOST_PP_LOCAL_ITERATE()
		}
		
	}
}
	
