#include <string>
#include <fstream>
#include "Console.h"
#include "TuringMachine.h"
using std::string;
using std::cout;


int main(int argc, char** argv)
{
    if (argc <= 1)
    {
        // TODO: add color coding
        cout << "Usage: \n"
             << "turing-interpreter [-i | --initial-input] ___ [-s | -initial-state] {DEFAULT: \"0\"} [-f | --program-file] {DEFAULT: \"Turing-Program.txt\"}\n"
             << "  --help, -h:               Show this help message"
             << "  --initial-input<string>:  \n"
             << "  --initial-state<string>:  \n"
             << "  --program-file<path>:     \n";

        return 0;
    }

    string initial_input, initial_state = "0", program_file_path = "Turing-Program.txt";
    bool found_i = false; // throw error if initial_input is not given

    // assign argument values
    for (int i = 0; i < argc; i++)
    {
        string arg = argv[i];

        if (arg == "-i" || arg == "--initial-input")
        {
            // value of next argument and skip it
            initial_input = argv[++i];
            found_i = true;
        }
        else if (arg == "-s" || arg == "--initial-state")
            initial_state = argv[++i];
        else if (arg == "-f" || arg == "--program-file")
            program_file_path = argv[++i];
    }

    if (!found_i)
    {
        std::cerr << "Argument --initial-input is required" << std::endl;
        return 0;
    }

#ifdef _DEBUG
    // Debug confirmation
    std::cout << "-i: " << initial_input << std::endl
              << "-s: " << initial_state << std::endl
              << "-f: " << program_file_path << std::endl;
    system("pause");
#endif

    std::ifstream program_file{ program_file_path };
    TuringConsole console{ program_file };
    TuringMachine machine{ initial_input, initial_state, program_file, console };

    if (!console.print_turing_code(program_file))
        return 0;

    while (true)
    {
        if (!machine.step())
            break;
    }

    program_file.close();

    return 0;
}
