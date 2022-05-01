#ifndef TURING_INTERPRETER_MACHINE_H
#define TURING_INTERPRETER_MACHINE_H

#include <string>
#include "Console.h"

class TuringMachine
{
public:
    TuringMachine(const std::string& _tape, std::string initial_state, std::ifstream& instructions_file, TuringConsole& _output);

    const std::string& get_tape() { return tape; }
    unsigned int get_position() { return position; }

    bool step();

private:
    std::string tape;
    std::ifstream& instructions;
    TuringConsole& output;
    unsigned int position;

    std::string current_state;
};


#endif