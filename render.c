#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

char* file_contents(FILE* file, char* filename);
char* read_stdin();
void format_and_print(char* text, char* filename);
char* substitute_escapes(char* text);

char* ERROR = "\e[1m\e[31mError:\e[39m\e[22m"; // bold and red 'Error:' text

int main(int argc, char** argv)
{
    if (argc == 2 && !strcmp(argv[1], "--help")) // help message
    {
        char* help_lines[] = {
            "The special characters are:",
            "    \\# : #BOLD#",
            "    \\~ : ~DIM~",
            "    \\* : *ITALIC*",
            "    \\_ : _UNDERLINED_",
            "    \\@ : @BLINKING@",
            "    \\$ : $INVERTED$",
            "    \\` : `HIDDEN` ~(hidden)~",
            "    \\% : %STRIKETHROUGH%",
            "    \\^ : ^RED^",
            "    \\| : |GREEN|",
            "Use a backslash to escape these characters.",
            NULL,
        };

        for (int i = 0; help_lines[i]; i++)
        {
            char* formatted_line = substitute_escapes(help_lines[i]);
            puts(formatted_line);
            free(formatted_line);
        }

        return 0;
    }

    if (argc == 1) // read from stdin if no files provided
    {
        char* original_text = read_stdin();
        format_and_print(original_text, "stdin");
        free(original_text);
    }

    else for (argv++; *argv; argv++) // advance to first arg, then iterate through
    {
        FILE* file = fopen(*argv, "r");

        if (file == NULL)
        {
            fprintf(stderr, "%s couldn't read file '%s'.\n", ERROR, *argv);
            continue;
        }

        char* original_text = file_contents(file, *argv);
        format_and_print(original_text, *argv);

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
        if (text_index == text_capacity - 1) // -1 to guarantee a NULL at the end
        {
            text_capacity *= 2;
            text = realloc(text, text_capacity);
        }

        text[text_index] = c;
    }

    return text;
}

char* file_contents(FILE* file, char* filename)
{
    fseek(file, 0L, SEEK_END);
    long numbytes = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char* buffer = calloc(numbytes + 1, sizeof(char));
    fread(buffer, sizeof(char), numbytes, file);

    return buffer;
}

void format_and_print(char* text, char* filename)
{
    for (char* text_i = text; *text_i; text_i++) // validate ASCII
    {
        if ((unsigned char) *text_i >= 128)
        {
            fprintf(stderr, "%s text in file '%s' is not ASCII encoded.\n", ERROR, filename);
            return;
        }
    }

    char* formatted_text = substitute_escapes(text);
    puts(formatted_text);
    free(formatted_text);
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
    bool status[256] = {0}; // tell whether text is currently in special state

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
