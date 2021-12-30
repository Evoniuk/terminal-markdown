#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

char* file_contents(char* filename);
char* substitute_escapes(char* text);

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "%s", substitute_escapes("#^ERROR:^ You must provide a file.#\n"));
        return 1;
    }

    if (argc == 2 && !strcmp(argv[1], "--help")) // help message
    {
        puts(substitute_escapes("The special characters are:"));
        puts(substitute_escapes("    \\# : #BOLD#"));
        puts(substitute_escapes("    \\~ : ~DIM~"));
        puts(substitute_escapes("    \\* : *ITALIC*"));
        puts(substitute_escapes("    \\_ : _UNDERLINED_"));
        puts(substitute_escapes("    \\@ : @BLINKING@"));
        puts(substitute_escapes("    \\$ : $INVERTED$"));
        puts(substitute_escapes("    \\` : `HIDDEN` ~(hidden)~"));
        puts(substitute_escapes("    \\% : %STRIKETHROUGH%"));
        puts(substitute_escapes("    \\^ : ^RED^"));
        puts(substitute_escapes("    \\| : |GREEN|"));
        puts(substitute_escapes("Use a backslash to escape these characters."));

        return 0;
    }

    for (argv++; *argv; argv++) // advance to first arg, then iterate through
    {
        char* original_text  = file_contents(*argv);
        char* formatted_text = substitute_escapes(original_text);
        free(original_text);

        puts(formatted_text);
        free(formatted_text);
    }

    return 0;
}

char* file_contents(char* filename)
{
    FILE* infile = fopen(filename, "r");

    if (infile == NULL)
    {
        fprintf(stderr, "Couldn't read file.\n");
        exit(1);
    }

    fseek(infile, 0L, SEEK_END);
    long numbytes = ftell(infile);
    fseek(infile, 0L, SEEK_SET);

    char* buffer = calloc(numbytes + 1, sizeof(char));

    fread(buffer, sizeof(char), numbytes, infile);
    fclose(infile);

    return buffer;
}

int result_len(char* text, char** substitutions_begin, char** substitutions_end)
{
    int  result_len  = 0;
    bool status[128] = {0};

    for (; *text; text++)
    {
        if (*text == '\\') // skip past escaped chars
        {
            text += 2;
            result_len++;
        }

        if (substitutions_begin[*text])
        {
            status[*text] = !status[*text];
            result_len += strlen(
                status[*text] ?
                    substitutions_begin[*text] :
                    substitutions_end[*text]
            );
        }

        else result_len++;
    }

    return result_len + 1;
}

char* substitute_escapes(char* text)
{
    // status tells whether text is currently in special state
    bool status[128] = {0};

    char* substitutions_begin[128] = {0};
    char* substitutions_end[128]   = {0};

    substitutions_begin['#'] = "\e[1m";   // bold
    substitutions_end['#']   = "\e[22m";
    substitutions_begin['~'] = "\e[2m";   // dim
    substitutions_end['~']   = "\e[22m";
    substitutions_begin['*'] = "\e[3m";   // italic
    substitutions_end['*']   = "\e[23m";
    substitutions_begin['_'] = "\e[4m";   // underline
    substitutions_end['_']   = "\e[24m";
    substitutions_begin['@'] = "\e[5m";   // blink
    substitutions_end['@']   = "\e[25m";
    substitutions_begin['$'] = "\e[7m";   // inverted
    substitutions_end['$']   = "\e[27m";
    substitutions_begin['`'] = "\e[8m";   // hidden
    substitutions_end['`']   = "\e[28m";
    substitutions_begin['%'] = "\e[9m";   // strikethrough
    substitutions_end['%']   = "\e[29m";
    substitutions_begin['^'] = "\e[31m";  // red
    substitutions_end['^']   = "\e[39m";
    substitutions_begin['|'] = "\e[32m";  // green
    substitutions_end['|']   = "\e[39m";

    char* result = calloc(result_len(text, substitutions_begin, substitutions_end), sizeof(char));

    for (char* result_i = result; *text; text++)
    {
        if (*text == '\\') // provide '\' as an escape character
        {
            *result_i++ = *++text;
            continue;
        }

        if (substitutions_begin[*text]) // if char is special char, change status
            status[*text] = !status[*text];

        char* substitution = status[*text] ?
            substitutions_begin[*text] :
            substitutions_end[*text];

        if (substitution)
            while (*substitution)
                *result_i++ = *substitution++;

        else *result_i++ = *text;
    }

    return result;
}
