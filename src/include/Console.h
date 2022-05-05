#ifndef TURING_INTERPRETER_CONSOLE_H
#define TURING_INTERPRETER_CONSOLE_H


#include <iostream>
#include <string>
#include <fstream>
#ifdef WIN32
#include <Windows.h>
#endif

#ifdef _DEBUG
#   define dbg_print(x)   std::cout << x
#   define dbg_println(x) std::cout << x << std::endl
#   define dbg_error(x)   std::cerr << x << std::endl
#else
#   define dbg_print(x)
#   define dbg_println(x)
#   define dbg_error(x)
#endif

// ncurses brighter colors
#define COLOR_LIGHT_BLACK  8
#define COLOR_LIGHT_RED    9
#define COLOR_LIGHT_GREEN  10
#define COLOR_LIGHT_YELLOW 11
#define COLOR_LIGHT_BLUE   12
#define COLOR_LIGHT_PURPLE 13
#define COLOR_LIGHT_CYAN   14
#define COLOR_LIGHT_WHITE  15

// Color pair names used
#define UNACTIVE_SCROLL  1
#define ACTIVE_SCROLL    2
#define ACTIVE_CODE_LINE 3
#define TAPE_CURSOR      4
#define COMMENT_LINE     5


struct coord
{
#ifdef WIN32
    unsigned short x, y;
#else
    unsigned int   x, y;
#endif
};

#ifdef WIN32
enum class color {
    // e.g.: foreground + red, background + green + brighter. Must not use a color alone
    /*black = 0, red, green, yellow, blue, purple, cyan, white,
    foreground = 30, background = 40, brighter = 60*/

    reset = 0,

    black_fg  = 30,
    red_fg    ,
    green_fg  ,
    yellow_fg ,
    blue_fg   ,
    purple_fg ,
    cyan_fg   ,
    white_fg  ,

    black_bg  = 40,
    red_bg    ,
    green_bg  ,
    yellow_bg ,
    blue_bg   ,
    purple_bg ,
    cyan_bg   ,
    white_bg  ,

    light_black_fg  = 90,
    light_red_fg    ,
    light_green_fg  ,
    light_yellow_fg ,
    light_blue_fg   ,
    light_purple_fg ,
    light_cyan_fg   ,
    light_white_fg  ,

    light_black_bg  = 100,
    light_red_bg    ,
    light_green_bg  ,
    light_yellow_bg ,
    light_blue_bg   ,
    light_purple_bg ,
    light_cyan_bg   ,
    light_white_bg  ,
};
#endif



class TuringConsole
{
public:
    explicit TuringConsole(std::ifstream& _code_file);
#ifndef WIN32 // Linux
    ~TuringConsole();
#endif

    short get_width()  const { return this->width;  }
    short get_height() const { return this->height; }

    void static clear();
    void set_tape_cursor(unsigned short position, const std::string& tape);
    // Highlights the current line in the code section. First line has value 0
    void set_current_code_line(unsigned short line, std::ifstream& file);
    void write_at(char symbol, unsigned short tape_position);

    bool print_turing_code(std::ifstream& file);
    void set_tape_value(const std::string& tape);

private:
#ifdef WIN32
    HANDLE handle;
    CONSOLE_SCREEN_BUFFER_INFO console_info;
#endif

    unsigned short turing_position;
    // First line is line 1
    unsigned int current_code_line;
    std::ifstream& code_file;

#ifdef WIN32
    short width, height;
#else
    int width, height;
#endif
    const coord tape_display_start = { 5, 2 };
    const coord code_start         = { 0, 5 };
    unsigned short tape_display_width;

#ifdef WIN32
    // Set color for printing, such as text color and background color (Windows Only)
    inline static void set_color(color col);
#endif
    inline void set_position(coord pos);
    inline static void print(std::string& str);
    void draw_tape_scrollers(bool arrow1_disabled = true, bool arrow2_disabled = true);
};


#endif //TURING_INTERPRETER_CONSOLE_H
