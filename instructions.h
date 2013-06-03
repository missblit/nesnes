#ifndef INSTRUCTIONS_H 
#define INSTRUCTIONS_H

#include <iostream>
#include <cassert>
#include "memory.h"
#include "cpu.h"
#include "opcodes.h"

/* Here's what the operation class looks like
   really the class is just a dummy wrapper to allow
   partial template specialization on the function */
template <Mnemonic mnemonic, Addr_Mode mode>
class Operation {
	public:	inline void operator()(CPU::Registers& reg) {
		std::cout << "Unimplemented instruction: ";
		std::cout << getString(mnemonic) << " ";
		std::cout << getString(mode) << "\n";
		assert(false);
	};
};

//and here are all the different implementations!
#include "instructions_impl.h" //need this because of inline functions :o

#endif
