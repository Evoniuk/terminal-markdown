#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tmd.h"

char* read_stdin();
char* file_contents(FILE* file);
void  print_help_message();

int main(int argc, char** argv)
{
    argv++; // advance to first actual argument

    bool help_flag     = *argv && (!strcmp(*argv, "-h") || !strcmp(*argv, "--help"));
    bool unformat_flag = *argv && (!strcmp(*argv, "-u") || !strcmp(*argv, "--unformatted"));

    if (unformat_flag) argv++;

    if (argc == 2 && help_flag)
        print_help_message();

    else if (argc == 1 || (argc == 2 && unformat_flag)) // read from stdin if no files provided
    {
        char* original_text = read_stdin();
        format_and_print(original_text, !unformat_flag);
        free(original_text);
    }

    else for (; *argv; argv++)
    {
        FILE* file = fopen(*argv, "r");

        if (file == NULL)
        {
            char* ERROR = "\e[1m\e[31mError:\e[39m\e[22m"; // bold and red 'Error:' text
            fprintf(stderr, "%s couldn't read file '%s'.\n", ERROR, *argv);
            continue;
        }

        char* original_text = file_contents(file);
        format_and_print(original_text, !unformat_flag);

        free(original_text);
        fclose(file);
    }

    return 0;
}

char* read_stdin()
{
    int text_capacity = 8;
    int text_index    = 0;
    char* text = calloc(text_capacity, sizeof(char));

    for (char c; (c = getchar()) != EOF; text_index++)
    {
        if (text_index == text_capacity - 1) // -1 to guarantee space for NULL
        {
            text_capacity *= 2;
            text = realloc(text, text_capacity);
        }

        text[text_index] = c;
    }

    text[text_index] = '\0';

    return text;
}

char* file_contents(FILE* file)
{
    fseek(file, 0L, SEEK_END);
    long numbytes = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char* buffer = calloc(numbytes + 1, sizeof(char));
    fread(buffer, sizeof(char), numbytes, file);

    return buffer;
}

void print_help_message()
{
    char* help_lines[] = {
        "_#Options:#                                   _",
        "",
        "    |b #-u# |B #_--unformatted_#                    |",
        "    |b    |B *Prints text without any styling.* |",
        "",
        "    |b #-h# |B #_--help_#                           |",
        "    |b    |B *Prints help information.*         |",
        "",
        "_#Special Characters:#                        _",
        "",
        "    ^G\\#^  : #BOLD#",
        "    ^G\\~^  : ~DIM~",
        "    ^G\\*^  : *ITALIC*",
        "    ^G\\_^  : _UNDERLINED_",
        "    ^G\\@^  : @BLINKING@",
        "    ^G\\$^  : $INVERTED$",
        "    ^G\\`^  : `HIDDEN` ~(hidden)~",
        "    ^G\\%^  : %STRIKETHROUGH%",
        "    ^G\\^^MX^ : ^rFO^gRE^yGR^bOU^mND^cS^",
        "    ^G\\|^MX^ : |rBA|gCK|yGR|bOU|mND|cS|",
        "",
        "_#Available Colors:#                          _",
        "",
        "    ^MX^ = l : ^lBLACK    ^ L : ^LWHITE          ^ ~('l' for 'lightness')~",
        "    ^MX^ = r : ^rRED      ^ R : ^RBRIGHT RED     ^",
        "    ^MX^ = g : ^gGREEN    ^ G : ^GBRIGHT GREEN   ^",
        "    ^MX^ = y : ^yYELLOW   ^ Y : ^YBRIGHT YELLOW  ^",
        "    ^MX^ = b : ^bBLUE     ^ B : ^BBRIGHT BLUE    ^",
        "    ^MX^ = c : ^cCYAN     ^ C : ^CBRIGHT CYAN    ^",
        "    ^MX^ = m : ^mMAGENTA  ^ M : ^MBRIGHT MAGENTA ^",
        "",
        "*Use a backslash to escape these characters.*",
        NULL,
    };

    for (int i = 0; help_lines[i]; i++)
    {
        char* formatted_line = substitute_escapes(help_lines[i], true);
        puts(formatted_line);
        free(formatted_line);
    }
}
