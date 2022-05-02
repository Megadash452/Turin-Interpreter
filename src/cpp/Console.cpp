#include "Console.h"
#include <sstream>
#ifndef WIN32 // Linux
#include <curses.h>
#endif

TuringConsole::TuringConsole(std::ifstream& _code_file)
    : tape_display_start({5, 2}), turing_position(0), current_code_line(0), code_file(_code_file)
#ifdef WIN32
    , console_info({})
#endif
{
#ifdef WIN32
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (handle == INVALID_HANDLE_VALUE)
        dbg_error("Bad Handle Read. ErrorCode " << GetLastError());

    // TODO: Get Console info does not work in Clion
    if (GetConsoleScreenBufferInfo(handle, &console_info) > 0)
        dbg_println("Successfully obtained OutputConsole console_info.");
    else
        dbg_error("Failed to obtain OutputConsole console_info. ErrorCode " << GetLastError());

    // TODO: add windows event for when console size changes
    width = console_info.dwSize.X;
    height = console_info.dwSize.Y;
    dbg_println("Console Width:  " << width);
    dbg_println("Console Height: " << height);
#else
    initscr();
    cbreak();
    noecho();
    start_color();

    init_pair(UNACTIVE_SCROLL, );
    init_pair(ACTIVE_SCROLL, );
    init_pair(ACTIVE_CODE_LINE, );
    init_pair(TAPE_CURSOR, );

    width = COLS;
    height = LINES;
#endif

    tape_display_width = width - 10;

    // setting up the Turing Tape
    // margin: 0 1 0 1

    /* scroller-btns
         width:  3
         height: 3
         bg-col: white (47)
         &:disabled
           bg-col: light_black (100)
    */
#ifdef WIN32
    clear();
#endif
    draw_tape_scrollers();
}

#ifndef WIN32 // Linux
    TuringConsole::~TuringConsole() { endwin(); }
#endif

void TuringConsole::clear()
{
#ifdef WIN32
    system("cls");
#else
    system("clear");
#endif
}


void TuringConsole::set_color(color col)
{
#if WIN32
    // ANSI Colors
    std::cout << "\033[" << (unsigned int)col << "m";
#else
    // TODO: figure out a cross-compatible way 
#endif
}

void TuringConsole::set_tape_cursor(unsigned short position, const std::string& tape)
{
#ifdef WIN32
    set_position({ (unsigned short)(tape_display_start.x + turing_position), tape_display_start.y });
    set_color(color::reset);
    std::cout << tape[turing_position];
#else
    mvaddstr(tape_display_start.y, tape_display_start.x + turing_position, tape[turing_position].c_str());
#endif

    // Highlight new position
#ifdef WIN32
    set_position({ (unsigned short)(tape_display_start.x + position), tape_display_start.y });
    set_color(color::cyan_bg);
    std::cout << tape[position];
    set_color(color::reset);
#else
    attron(COLOR_PAIR(TAPE_CURSOR));
    mvaddstr(tape_display_start.y, tape_display_start.x + position, tape[position].c_str());
    attroff(COLOR_PAIR(TAPE_CURSOR));
#endif

    turing_position = position;
}

void TuringConsole::set_position(coord pos)
{
#if WIN32
    SetConsoleCursorPosition(handle, COORD{ (short)pos.x, (short)pos.y });
#else
    // NOTE: for ncurses, the x and y are reversed (y comes first)
    move(pos.y, pos.x);
#endif
}

void TuringConsole::set_current_code_line(unsigned short line, std::ifstream& file)
{
    // start from the beginning
    file.clear();
    file.seekg(0);

    // First line is line 1
    unsigned short line_count = 0;
    // completed resetting the current_code_line
    bool resetted = false;
    // Completed highlighting the line
    bool colored = false;

    if (file.is_open())
    {
        // Stop when both tasks (resetting current_line and highlighting target line) are complete
        while (file.good() && !(resetted && colored))
        {
            std::string s;
            std::getline(file, s);
            // First line is line 1
            line_count++;

            // Reset color of current_line
            if (line_count == current_code_line)
            {
#ifdef WIN32
                set_position({ 0, (unsigned short)(line_count + 5u) });
#else
                set_position({ 0, line_count + 5u });
#endif
                set_color(color::reset);
                std::cout << s;

                resetted = true;
            }
            // line and current_code_line could be the same, so no elseif
            
            // Set color of target line
            if (line_count == line)
            {
#ifdef WIN32
                set_position({ 0, (unsigned short)(line_count + 5u) });
#else
                set_position({ 0, line_count + 5u });
#endif
                set_color(color::green_bg);
                std::cout << s;
                set_color(color::reset);

                colored = true;
            }
        }

        // target line is greater than the number of lines in the file
        if (!colored)
            std::cerr << "Argument <line> is greater than lines in the file" << std::endl;
        // Can safely set current_code_line
        else
            current_code_line = line;
    }
    else
        std::cerr << "Error opening file containing Turing instructions" << std::endl;

    file.clear();
    file.seekg(0);
}

void TuringConsole::write_at(char symbol, unsigned short tape_position)
{
#ifdef WIN32
    set_position({ (unsigned short)(tape_display_start.x + tape_position), tape_display_start.y });
#else
    set_position({ tape_display_start.x + tape_position, tape_display_start.y });
#endif

    if (turing_position == tape_position)
        set_color(color::cyan_bg);
    std::cout << symbol;
    set_color(color::reset);
}

void TuringConsole::draw_tape_scrollers(bool arrow1_disabled, bool arrow2_disabled)
{
    // Left Scroller Arrow
    set_color(color::black_fg);
    if (arrow1_disabled)
        set_color(color::light_black_bg);
    else
        set_color(color::white_bg);
    set_position({ 1, 1 });
    std::cout << "   ";
    set_position({ 1, 2 });
    std::cout << " < ";
    set_position({ 1, 3 });
    std::cout << "   ";

    // Right Scroller Arrow
    if (arrow2_disabled)
        set_color(color::light_black_bg);
    else
        set_color(color::white_bg);
#if WIN32
    auto arrow_right_x = (unsigned short)(width - 1 - 3);
#else
    unsigned int arrow_right_x = width - 1 - 3;
#endif
    set_position({ arrow_right_x, 1 });
    std::cout << "   ";
    set_position({ arrow_right_x, 2 });
    std::cout << " > ";
    set_position({ arrow_right_x, 3 });
    std::cout << "   ";

    set_color(color::reset);
    set_position({ 1, 5 });
}

void TuringConsole::set_tape_value(const std::string& tape)
{
    set_position(tape_display_start);

    if (tape.size() > tape_display_width)
        ; // TODO: note: tape cursor (cyan) should be in the middle.
    else
        for (unsigned int i = 0; i < tape.size(); i++)
            if (i == turing_position)
            {
                set_color(color::cyan_bg);
                std::cout << tape[i];
                set_color(color::reset);
            }
            else
                std::cout << tape[i];

    set_position({ 1, 5 });
}

// Tries to print out Turing instructions. returns false if fails
bool TuringConsole::print_turing_code(std::ifstream& file)
{
    char c;

    set_position({ 0, 5 });
    if (file.is_open())
        while (file.good())
        {
            c = file.get();

            // Comment
            if (c == ';')
                set_color(color::light_black_fg);
            else if (c == '\n')
                set_color(color::reset);

            std::cout << c;
        }
    else
    {
        std::cerr << "Error opening file containing Turing instructions" << std::endl;
        return false;
    }

    file.clear();
    file.seekg(0);
    set_color(color::reset);

    return true;
}
