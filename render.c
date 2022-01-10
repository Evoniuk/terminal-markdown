#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tmd.h"

char* read_stdin();
char* file_contents(FILE* file);

int main(int argc, char** argv)
{
    if (argc == 2 && !strcmp(argv[1], "--help")) // help message
    {
        char* help_lines[] = {
            "The special characters are:",
            "    \\#  : #BOLD#",
            "    \\~  : ~DIM~",
            "    \\*  : *ITALIC*",
            "    \\_  : _UNDERLINED_",
            "    \\@  : @BLINKING@",
            "    \\$  : $INVERTED$",
            "    \\`  : `HIDDEN` ~(hidden)~",
            "    \\%  : %STRIKETHROUGH%",
            "    \\^X : ^rFO^gRE^yGR^bOU^mND^cS^",
            "    \\|X : |rBA|gCK|yGR|bOU|mND|cS|",
            "The available colors are:",
            "    X = l : ^l BLACK      ^ L : ^L WHITE          ^ ~('l' for 'lightness')~",
            "    X = r : ^r RED        ^ R : ^R BRIGHT RED     ^",
            "    X = g : ^g GREEN      ^ G : ^G BRIGHT GREEN   ^",
            "    X = y : ^y YELLOW     ^ Y : ^Y BRIGHT YELLOW  ^",
            "    X = b : ^b BLUE       ^ B : ^B BRIGHT BLUE    ^",
            "    X = c : ^c CYAN       ^ C : ^C BRIGHT CYAN    ^",
            "    X = m : ^m MAGENTA    ^ M : ^M BRIGHT MAGENTA ^",
            "Use a backslash to escape these characters.",
            NULL,
        };

        for (int i = 0; help_lines[i]; i++)
        {
            char* formatted_line = substitute_escapes(help_lines[i]);
            puts(formatted_line);
            free(formatted_line);
        }
    }

    else if (argc == 1) // read from stdin if no files provided
    {
        char* original_text = read_stdin();
        format_and_print(original_text);
        free(original_text);
    }

    else for (argv++; *argv; argv++) // advance to first arg, then iterate through
    {
        FILE* file = fopen(*argv, "r");

        if (file == NULL)
        {
            char* ERROR = "\e[1m\e[31mError:\e[39m\e[22m"; // bold and red 'Error:' text
            fprintf(stderr, "%s couldn't read file '%s'.\n", ERROR, *argv);
            continue;
        }

        char* original_text = file_contents(file);
        format_and_print(original_text);

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
