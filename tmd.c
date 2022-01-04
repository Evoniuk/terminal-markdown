#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define ASCII_MAX 0x80

int result_len(char* text, char** substitutions_begin, char** substitutions_end)
{
    int  result_len        = 0;
    bool status[ASCII_MAX] = {0};

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
    for (char* text_i = text; *text_i; text_i++)
    {
        if ((unsigned char) *text_i >= ASCII_MAX)
        {
            char* ERROR = "\e[1m\e[31mError:\e[39m\e[22m"; // bold and red 'Error:' text
            fprintf(stderr, "%s text is not ASCII encoded.\n", ERROR);
            return NULL;
        }
    }

    bool status[ASCII_MAX] = {0}; // tell whether text is currently in special state

    char* substitutions_begin[ASCII_MAX] = {0};
    char* substitutions_end[ASCII_MAX]   = {0};

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

void format_and_print(char* text)
{
    char* formatted_text = substitute_escapes(text);
    if (!formatted_text) return;
    printf("%s", formatted_text);
    free(formatted_text);
}
