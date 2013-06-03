typedef Addr_Mode AM;
typedef Mnemonic  Mn;
 
/*template <Mnemonic mnemonic, Addr_Mode mode>
class Operation {
	public:	inline void operator()(CPU::Registers& reg) {
		std::cout << "Unimplemented instruction: ";
		std::cout << getString(mnemonic) << " ";
		std::cout << getString(mode) << "\n";
		assert(false);
	};
};*/

template <AM mode>
class Operation<Mnemonic::SEI, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		CPU::read<mode>(reg); //dummy CPU::read
		reg.flagOn(CPU::FLAG::INT_DISABLE);
	}
};

template <AM mode>
class Operation<Mnemonic::SED, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		CPU::read<mode>(reg); //dummy CPU::read
		reg.flagOn(CPU::FLAG::DECIMAL);
	}
};

template <AM mode>
class Operation<Mnemonic::CLD, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		CPU::read<mode>(reg); //dummy CPU::read
		reg.flagOff(CPU::FLAG::DECIMAL);
	}
};

template <AM mode>
class Operation<Mnemonic::CLV, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		CPU::read<mode>(reg); //dummy CPU::read
		reg.flagOff(CPU::FLAG::OVERFLOW);
	}
};

template <>
class Operation<Mnemonic::JMP, Addr_Mode::Absolute> {
	public: inline void operator()(CPU::Registers& reg) {
		auto low = mem[reg.PC++];                  CPU::tick();
		uint16_t res = low | (mem[reg.PC++] << 8);
		reg.PC = res;                              CPU::tick();
	}
};

template <>
class Operation<Mnemonic::JMP, Addr_Mode::Indirect> {
	public: inline void operator()(CPU::Registers& reg) {
		uint8_t low         = mem[reg.PC++];            CPU::tick();
		uint8_t high        = mem[reg.PC++];            CPU::tick();
		uint8_t target_low  = mem[low++ | (high << 8)]; CPU::tick();
		uint8_t target_high = mem[low   | (high << 8)];
		reg.PC = target_low | (target_high << 8);       CPU::tick(); 
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::LDX, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		reg.X = CPU::read<mode>(reg);
		reg.setFlag(CPU::FLAG::ZERO, !reg.X);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.X & 0x80);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::LDY, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		reg.Y = CPU::read<mode>(reg);
		reg.setFlag(CPU::FLAG::ZERO, !reg.Y);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.Y & 0x80);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::STX, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		CPU::write<mode>(reg, reg.X);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::STY, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		CPU::write<mode>(reg, reg.Y);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::JSR, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto low = mem[reg.PC++];                CPU::tick();
		                                         CPU::tick();
		mem[0x100 | reg.S--] = reg.PC >> 8;      CPU::tick();
		mem[0x100 | reg.S--] = reg.PC;           CPU::tick(); 
		uint16_t res = low | (mem[reg.PC] << 8);
		reg.PC = res;                            CPU::tick();
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::RTS, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		mem[reg.PC];                         CPU::tick();
		reg.S++;	                         CPU::tick();
		reg.PC  =  mem[0x100 | reg.S++];     CPU::tick();
		reg.PC |= (mem[0x100 | reg.S] << 8); CPU::tick();
		reg.PC++;                            CPU::tick();
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::PHP, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		mem[reg.PC];                                               CPU::tick();
		mem[0x100 | reg.S--] =  reg.P | CPU::FLAG::BREAK | CPU::FLAG::DUMMY; CPU::tick();
		
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::PLP, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		mem[reg.PC];                                               CPU::tick();
		reg.S++;                                                   CPU::tick();
		reg.P = (mem[0x100 | reg.S] | CPU::FLAG::DUMMY) & ~CPU::FLAG::BREAK; CPU::tick();
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::PHA, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		mem[reg.PC];                   CPU::tick();
		mem[0x100 | reg.S--] =  reg.A; CPU::tick();
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::PLA, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		mem[reg.PC];                     CPU::tick();
		reg.S++;                         CPU::tick();
		reg.A = mem[0x100 | reg.S];      CPU::tick();
		reg.setFlag(CPU::FLAG::ZERO, !reg.A);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.A & 0x80);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::AND, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		reg.A &= CPU::read<mode>(reg);
		reg.setFlag(CPU::FLAG::ZERO, !reg.A);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.A & 0x80);	
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::ORA, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		reg.A |= CPU::read<mode>(reg);
		reg.setFlag(CPU::FLAG::ZERO, !reg.A);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.A & 0x80);	
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::EOR, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		reg.A ^= CPU::read<mode>(reg);
		reg.setFlag(CPU::FLAG::ZERO, !reg.A);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.A & 0x80);	
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::ADC, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		uint8_t data = CPU::read<mode>(reg);
		uint16_t sum = reg.A + data + reg.getFlag(CPU::FLAG::CARRY);
		reg.setFlag(CPU::FLAG::CARRY, sum & 0x100); //0
		reg.setFlag(CPU::FLAG::NEGATIVE, sum & 0x80); //1	 
		reg.setFlag(CPU::FLAG::OVERFLOW, (reg.A ^ sum) & (data ^ sum) & 0x80);
		reg.setFlag(CPU::FLAG::ZERO, !(sum & 0xFF));
		reg.A = sum;
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::SBC, mode> {
	public: inline void operator()(CPU::Registers& reg) {

		using std::cout;
		using std::hex;
		uint8_t data = CPU::read<mode>(reg) + !reg.getFlag(CPU::FLAG::CARRY);
		uint8_t sum = reg.A - data;
		reg.setFlag(CPU::FLAG::CARRY, !(sum & 0x80));
		reg.setFlag(CPU::FLAG::NEGATIVE, sum & 0x80);
		reg.setFlag(CPU::FLAG::OVERFLOW, ((reg.A ^ sum) & 0x80) &&
		                            ((data & 0x80) == (sum & 0x80)));
		reg.setFlag(CPU::FLAG::ZERO, !(sum & 0xFF));
		reg.A = sum;
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::INY, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		CPU::read<mode>(reg);
		reg.Y++;
		reg.setFlag(CPU::FLAG::ZERO, !reg.Y);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.Y & 0x80);		
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::INX, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		CPU::read<mode>(reg); //dummy CPU::read
		reg.X++;
		reg.setFlag(CPU::FLAG::ZERO, !reg.X);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.X & 0x80);		
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::INC, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto res = CPU::modify<mode>(reg, [](CPU::Registers&, uint8_t a)
		-> uint8_t { return a+1; });
		reg.setFlag(CPU::FLAG::ZERO, !res.second);
		reg.setFlag(CPU::FLAG::NEGATIVE, res.second & 0x80);		
	}
};
	
template <Addr_Mode mode>
class Operation<Mnemonic::DEY, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		CPU::read<mode>(reg); //dummy CPU::read
		reg.Y--;
		reg.setFlag(CPU::FLAG::ZERO, !reg.Y);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.Y & 0x80);		
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::DEX, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		//dummy CPU::read
		CPU::read<mode>(reg);
		reg.X--;
		reg.setFlag(CPU::FLAG::ZERO, !reg.X);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.X & 0x80);		
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::DEC, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto res = CPU::modify<mode>(reg, [](CPU::Registers&, uint8_t a)
		-> uint8_t { return a-1; });
		reg.setFlag(CPU::FLAG::ZERO, !res.second);
		reg.setFlag(CPU::FLAG::NEGATIVE, res.second & 0x80);		
	}
};
	
template <Addr_Mode mode>
class Operation<Mnemonic::CMP, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		uint8_t data = CPU::read<mode>(reg);
		reg.setFlag(CPU::FLAG::CARRY,    reg.A >= data);
		reg.setFlag(CPU::FLAG::ZERO,     reg.A == data);
		reg.setFlag(CPU::FLAG::NEGATIVE, (reg.A - data) & 0x80);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::CPY, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		uint8_t data = CPU::read<mode>(reg);
		reg.setFlag(CPU::FLAG::CARRY,    reg.Y >= data);
		reg.setFlag(CPU::FLAG::ZERO,     reg.Y == data);
		reg.setFlag(CPU::FLAG::NEGATIVE, (reg.Y - data) & 0x80);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::CPX, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		uint8_t data = CPU::read<mode>(reg);
		reg.setFlag(CPU::FLAG::CARRY,    reg.X >= data);
		reg.setFlag(CPU::FLAG::ZERO,     reg.X == data);
		reg.setFlag(CPU::FLAG::NEGATIVE, (reg.X - data) & 0x80);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::NOP, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto dummy = CPU::read<mode>(reg);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::SEC, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		reg.flagOn(CPU::FLAG::CARRY);
		CPU::tick();
	}
};

void branch_on_flag(CPU::Registers& reg, uint8_t flag, bool branch_if_set) {
	int8_t delta = CPU::read<AM::Immediate>(reg); 
	bool shouldBranch = branch_if_set ?  reg.getFlag(flag)
	                                  : !reg.getFlag(flag);
	if(!shouldBranch)
		return;

	//calculate new address
	auto dummy = mem[reg.PC]; //spurious memory access
	uint16_t pc_high = reg.PC & 0xFF00;
	reg.PC += delta;
	CPU::tick();
	
	//no page flip
	if(pc_high == (reg.PC & 0xFF00)) {
		return;
	}
	//page flip - extra cycle
	auto dummy2 = mem[reg.PC]; //spurious memory access
	CPU::tick();
	return;

}

template <Addr_Mode mode>
class Operation<Mnemonic::BCS, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		return branch_on_flag(reg, CPU::FLAG::CARRY, true);

	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::BVS, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		return branch_on_flag(reg, CPU::FLAG::OVERFLOW, true);

	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::BVC, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		return branch_on_flag(reg, CPU::FLAG::OVERFLOW, false);

	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::BPL, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		return branch_on_flag(reg, CPU::FLAG::NEGATIVE, false);

	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::BMI, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		return branch_on_flag(reg, CPU::FLAG::NEGATIVE, true);

	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::BCC, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		return branch_on_flag(reg, CPU::FLAG::CARRY, false);

	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::BEQ, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		return branch_on_flag(reg, CPU::FLAG::ZERO, true);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::BNE, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		return branch_on_flag(reg, CPU::FLAG::ZERO, false);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::CLC, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		reg.flagOff(CPU::FLAG::CARRY);
		CPU::tick();
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::LDA, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		reg.A = CPU::read<mode>(reg);
		//std::cout << "Loading: " << std::hex << int(reg.A) << "\n";
		reg.setFlag(CPU::FLAG::ZERO, !reg.A);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.A & 0x80);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::STA, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		CPU::write<mode>(reg, reg.A);		
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::BIT, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto mem = CPU::read<mode>(reg);
		auto anded = mem & reg.A;
		reg.setFlag(CPU::FLAG::ZERO, !anded);
		reg.setFlag(CPU::FLAG::OVERFLOW, mem & 0x40);
		reg.setFlag(CPU::FLAG::NEGATIVE, mem & 0x80);			
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::TAY, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto mem = CPU::read<mode>(reg); //dummy CPU::read
		reg.Y = reg.A;
		reg.setFlag(CPU::FLAG::ZERO, !reg.Y);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.Y & 0x80);			
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::TYA, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto mem = CPU::read<mode>(reg); //dummy CPU::read
		reg.A = reg.Y;
		reg.setFlag(CPU::FLAG::ZERO, !reg.A);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.A & 0x80);			
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::TAX, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto mem = CPU::read<mode>(reg); //dummy CPU::read
		reg.X = reg.A;
		reg.setFlag(CPU::FLAG::ZERO, !reg.X);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.X & 0x80);			
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::TXA, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto mem = CPU::read<mode>(reg); //dummy CPU::read
		reg.A = reg.X;
		reg.setFlag(CPU::FLAG::ZERO, !reg.A);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.A & 0x80);			
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::TSX, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto mem = CPU::read<mode>(reg); //dummy CPU::read
		reg.X = reg.S;
		reg.setFlag(CPU::FLAG::ZERO, !reg.X);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.X & 0x80);			
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::TXS, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto mem = CPU::read<mode>(reg); //dummy CPU::read
		reg.S = reg.X;		
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::RTI, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		CPU::read<mode>(reg);
		reg.S++; CPU::tick();
		reg.P = (mem[0x100 | reg.S++] | CPU::FLAG::DUMMY) & ~CPU::FLAG::BREAK;
		CPU::tick();
		reg.PC = mem[0x100  | reg.S++];     CPU::tick();
		reg.PC |= mem[0x100 | reg.S] << 8; CPU::tick();			
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::LSR, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto data = CPU::modify<mode>(reg, [](CPU::Registers&, uint8_t a)
		 -> uint8_t { return a>>1; });
		reg.setFlag(CPU::FLAG::CARRY, data.first & 0x01);
		reg.flagOff(CPU::FLAG::NEGATIVE);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::ASL, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto data = CPU::modify<mode>(reg, [](CPU::Registers&, uint8_t a)
		-> uint8_t { return a<<1; });
		reg.setFlag(CPU::FLAG::CARRY, data.first & 0x80);
		reg.setFlag(CPU::FLAG::NEGATIVE, data.second & 0x80);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::ROR, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto data = CPU::modify<mode>(reg, [](CPU::Registers& reg, uint8_t a)
		-> uint8_t { return (a >> 1) | reg.getFlag(CPU::FLAG::CARRY) << 7; });
		reg.setFlag(CPU::FLAG::CARRY, data.first & 0x01);
		reg.setFlag(CPU::FLAG::NEGATIVE, data.second & 0x80);
	}
};


template <Addr_Mode mode>
class Operation<Mnemonic::ROL, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto data = CPU::modify<mode>(reg, [](CPU::Registers& reg, uint8_t a)
		-> uint8_t { return (a << 1) | reg.getFlag(CPU::FLAG::CARRY); });
		reg.setFlag(CPU::FLAG::CARRY, data.first & 0x80);
		reg.setFlag(CPU::FLAG::NEGATIVE, data.second & 0x80);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::LAX, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		reg.X = CPU::read<mode>(reg);
		reg.A = reg.X;
		reg.setFlag(CPU::FLAG::ZERO, !reg.X);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.X & 0x80);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::SAX, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		CPU::write<mode>(reg, reg.A & reg.X);		
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::DCP, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto res = CPU::modify<mode>(reg, [](CPU::Registers&, uint8_t a)
		-> uint8_t { return a-1; });
		reg.setFlag(CPU::FLAG::CARRY,    reg.A >= res.second);
		reg.setFlag(CPU::FLAG::ZERO,     reg.A == res.second);
		reg.setFlag(CPU::FLAG::NEGATIVE, (reg.A - res.second) & 0x80);	
	}
};


template <Addr_Mode mode>
class Operation<Mnemonic::ISB, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		using namespace std;
		auto res = CPU::modify<mode>(reg, [](CPU::Registers&, uint8_t a)
		-> uint8_t { return a+1; });
		uint8_t data = res.second + !reg.getFlag(CPU::FLAG::CARRY);
		uint8_t sum = reg.A - data;
		reg.setFlag(CPU::FLAG::CARRY, (sum & 0x80)); //seemingly backwards, but makes nestest happy
		reg.setFlag(CPU::FLAG::NEGATIVE, sum & 0x80);
		reg.setFlag(CPU::FLAG::OVERFLOW, ((reg.A ^ sum) & 0x80) &&
		                            ((data & 0x80) == (sum & 0x80)));
		reg.setFlag(CPU::FLAG::ZERO, !(sum & 0xFF));
		reg.A = sum;		
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::SLO, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto data = CPU::modify<mode>(reg, [](CPU::Registers&, uint8_t a)
		-> uint8_t { return a<<1; });
		reg.A |= data.second;
		reg.setFlag(CPU::FLAG::CARRY, data.first & 0x80);
		reg.setFlag(CPU::FLAG::ZERO, !reg.A);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.A & 0x80);
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::RLA, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto data = CPU::modify<mode>(reg, [](CPU::Registers& reg, uint8_t a)
		-> uint8_t { return (a << 1) | reg.getFlag(CPU::FLAG::CARRY); });
		reg.A &= data.second;
		reg.setFlag(CPU::FLAG::CARRY, data.first & 0x80);
		reg.setFlag(CPU::FLAG::ZERO, !reg.A);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.A & 0x80);	
	}
};


template <Addr_Mode mode>
class Operation<Mnemonic::RRA, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto data = CPU::modify<mode>(reg, [](CPU::Registers& reg, uint8_t a)
		-> uint8_t { return (a >> 1) | reg.getFlag(CPU::FLAG::CARRY) << 7; });
		reg.setFlag(CPU::FLAG::CARRY, data.first & 0x01);
		reg.setFlag(CPU::FLAG::NEGATIVE, data.second & 0x80);

		uint16_t sum = reg.A + data.second + reg.getFlag(CPU::FLAG::CARRY);
		reg.setFlag(CPU::FLAG::CARRY, sum & 0x100); //0
		reg.setFlag(CPU::FLAG::NEGATIVE, sum & 0x80); //1	 
		reg.setFlag(CPU::FLAG::OVERFLOW, (reg.A ^ sum) & (data.second ^ sum) & 0x80);
		reg.setFlag(CPU::FLAG::ZERO, !(sum & 0xFF));
		reg.A = sum;
	}
};

template <Addr_Mode mode>
class Operation<Mnemonic::SRE, mode> {
	public: inline void operator()(CPU::Registers& reg) {
		auto data = CPU::modify<mode>(reg, [](CPU::Registers&, uint8_t a)
		 -> uint8_t { return a>>1; });
		reg.setFlag(CPU::FLAG::CARRY, data.first & 0x01);
		reg.A ^= data.second;
		reg.setFlag(CPU::FLAG::ZERO, !reg.A);
		reg.setFlag(CPU::FLAG::NEGATIVE, reg.A & 0x80);	
	}
};


