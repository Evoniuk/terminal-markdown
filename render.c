#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

char* file_contents(char* filename);
char* substitute_escapes(char* text);

int main(int argc, char** argv)
{
    if (argc != 2) return 1;

    char* text   = file_contents(argv[1]);
    char* result = substitute_escapes(text);
    free(text);

    printf("%s", result);
    free(result);

    return 0;
}

char* file_contents(char* filename)
{
    FILE* infile = fopen(filename, "r");

    fseek(infile, 0L, SEEK_END);
    long numbytes = ftell(infile);
    fseek(infile, 0L, SEEK_SET);

    char* buffer = calloc(numbytes + 1, sizeof(char));

    fread(buffer, sizeof(char), numbytes, infile);
    fclose(infile);

    return buffer;
}

char* substitute_escapes(char* text)
{
    // status tells whether text is currently in special state
    bool status[128] = {0};

    char* substitutions_begin[128] = {0};
    char* substitutions_end[128]   = {0};

    substitutions_begin['*'] = "\e[1m";   // bold
    substitutions_end['*']   = "\e[22m";
    substitutions_begin['~'] = "\e[2m";   // dim
    substitutions_end['~']   = "\e[22m";
    substitutions_begin['_'] = "\e[4m";   // underline
    substitutions_end['_']   = "\e[24m";
    substitutions_begin['@'] = "\e[5m";   // blink
    substitutions_end['@']   = "\e[25m";
    substitutions_begin['$'] = "\e[7m";   // inverted
    substitutions_end['$']   = "\e[27m";
    substitutions_begin['`'] = "\e[8m";   // hidden
    substitutions_end['`']   = "\e[28m";

    int result_len = 0;
    for (char* text_i = text; *text_i; text_i++)
    {
        if (*text_i == '\\') // skip past escaped chars
        {
            text_i += 2;
            result_len++;
        }

        if (substitutions_begin[*text_i])
        {
            status[*text_i] = !status[*text_i];
            result_len += strlen(
                status[*text_i] ?
                    substitutions_begin[*text_i] :
                    substitutions_end[*text_i]
            );
        }

        else result_len++;
    }

    memset(status, 0, sizeof status); // reset status array

    char* result = calloc(result_len + 1, sizeof(char));

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
            for (int i = 0; i < strlen(substitution); i++)
                *result_i++ = substitution[i];

        else *result_i++ = *text;
    }

    return result;
}
