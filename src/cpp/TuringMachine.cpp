#include "TuringMachine.h"
#include <array>
using std::string;

TuringMachine::TuringMachine(const string& _tape, string initial_state, std::ifstream& instructions_file, TuringConsole& _output)
    : position(0), tape(_tape), current_state(initial_state), instructions(instructions_file), output(_output)
{
    if (_tape.empty())
        tape = " ";
    else
        tape = _tape;

    output.set_tape_value(tape);
}

bool TuringMachine::step()
{
    // <state> <symbol> <new_symbol> <r | l> <new_state>

    // First line is line 1
    unsigned int line_num = 0;

    // look for a matching current_symbol in a matching current_state
    if (instructions.is_open())
        while (instructions.good())
        {
            string line;
            std::getline(instructions, line);
            // First line is line 1
            line_num++;

            std::array<string, 5> read_order = {
                string{}, // state
                string{}, // symbol
                string{}, // new_symbol
                string{}, // move_direction (r | l)
                string{}  // new_state
            };
            unsigned short read_from = 0;
            bool reading = false;

            // assign values
            for (char c : line)
            {
                // comment, can be skipped
                if (c == ';')
                    break;

                // whitespace used as separator
                if (c == ' ' && reading)
                {
                    reading = false;
                    // move on to reading symbol
                    if (read_from < read_order.size() - 1)
                        read_from++;
                    // done reading state and symbol
                    else if (read_from >= read_order.size() - 1)
                        break;
                    continue;
                }
                else if (c != ' ' && reading)
                {
                    read_order[read_from] += c;
                }
                else if (c != ' ' && !reading)
                {
                    reading = true;
                    read_order[read_from] += c;
                }
            }

            // Ignore this line if state does not match; "*" is wildcard
            if (read_order[0] == current_state || read_order[0] == "*")
            {
                // Current_Symbol
                if (read_order[1].size() > 1)
                {
                    std::cerr << "Syntax Error (line " << line_num << "): Symbol must only be 1 character long" << std::endl;
                    return false;
                }
                else if (read_order[1].size() < 1)
                {
                    std::cerr << "Error (line " << line_num << "): Could not find Symbol character" << std::endl;
                    return false;
                }
                // New_Symbol
                if (read_order[2].size() > 1)
                {
                    std::cerr << "Syntax Error (line " << line_num << "): New_Symbol must only be 1 character long" << std::endl;
                    return false;
                }
                else if (read_order[2].size() < 1)
                {
                    std::cerr << "Error (line " << line_num << "): Could not find New_Symbol character" << std::endl;
                    return false;
                }
                // Move_Direction
                if (read_order[3].size() > 1)
                {
                    std::cerr << "Syntax Error (line " << line_num << "): Move_Direction must only be 1 character long" << std::endl;
                    return false;
                }
                else if (read_order[3].size() < 1)
                {
                    std::cerr << "Error (line " << line_num << "): Could not find Move_Direction character" << std::endl;
                    return false;
                }

                string& state       = read_order[0];
                char symbol         = read_order[1][0];
                char new_symbol     = read_order[2][0];
                char move_direction = std::tolower(read_order[3][0]);
                string& new_state   = read_order[4];
                // _ represents space
                if (symbol == '_')
                    symbol = ' ';
                if (new_symbol == '_')
                    new_symbol = ' ';

                // State and Symbol match current
                if (symbol == tape[position] || symbol == '*')
                {
                    // Found a matching instruction in code. Now handle it

                    output.set_current_code_line(line_num, instructions);

                    // * is no change; no need to write new_symbol if it's the same as old
                    if (new_symbol != '*' && new_symbol != tape[position])
                    {
                        // Overwrite symbol in the tape
                        tape[position] = new_symbol;
                        output.write_at(new_symbol, position);
                    }

                    // Move left or right; * is no change
                    if (move_direction != '*')
                    {
                        if (move_direction == 'l' && position == 0)
                        {
                            tape.insert(tape.begin(), ' ');
                            output.set_tape_value(tape);
                        }
                        else if (move_direction == 'r' && position >= tape.size() - 1)
                        {
                            tape.append(" ");
                            output.set_tape_value(tape);
                        }

                        else if (move_direction == 'l')
                        {
                            position--;
                            output.set_tape_cursor(position, tape);
                        }
                        else if (move_direction == 'r')
                        {
                            position++;
                            output.set_tape_cursor(position, tape);
                        }

                        // Direction is not l or r
                        else
                        {
                            std::cerr << "Syntax Error (line " << line_num << "): Move_Direction must be either r or l" << std::endl;
                            return false;
                        }
                    }

                    // * is no change
                    if (new_state != "*")
                        current_state = new_state;

                    // TODO: highlight the line that will be executed next

                    // Step done successfully
                    return true;
                }
            }
        }
    else
        std::cerr << "Error opening file containing Turing instructions" << std::endl;
    
    return false;
}
