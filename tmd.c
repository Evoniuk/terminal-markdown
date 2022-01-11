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
            text++;
            result_len++;
            continue;
        }

        if (*text == '^' || *text == '|')
        {
            while (text[0] == text[1]) text++;

            char escape  = text[0];
            char control = text[1];
            int  ctrl_i  = ASCII_MAX * (escape == '^' ? 1 : 2) + control;

            result_len += strlen(substitutions_begin[ctrl_i] ?
                substitutions_begin[ctrl_i] :
                substitutions_end[escape]);

            if (control != '\\'             &&
                !substitutions_end[control] &&
                substitutions_begin[ctrl_i])
                text++;

            continue;
        }

        if (substitutions_begin[*text])
            status[*text] = !status[*text];

        char* substitution = status[*text] ?
            substitutions_begin[*text] :
            substitutions_end[*text];

        if (substitution)
            result_len += strlen(substitution);

        else result_len++;
    }

    return result_len + 1;
}

void init_substitutions(char** substitutions_begin, char** substitutions_end)
{
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

    // special end sequences
    substitutions_end['^'] = "\e[39m";    // font color
    substitutions_end['|'] = "\e[49m";    // background

    // text colors
    substitutions_begin[ASCII_MAX + 'l'] = "\e[30m"; // black
    substitutions_begin[ASCII_MAX + 'r'] = "\e[31m"; // red
    substitutions_begin[ASCII_MAX + 'g'] = "\e[32m"; // green
    substitutions_begin[ASCII_MAX + 'y'] = "\e[33m"; // yellow
    substitutions_begin[ASCII_MAX + 'b'] = "\e[34m"; // blue
    substitutions_begin[ASCII_MAX + 'm'] = "\e[35m"; // magenta
    substitutions_begin[ASCII_MAX + 'c'] = "\e[36m"; // cyan

    substitutions_begin[ASCII_MAX + 'L'] = "\e[97m"; // white
    substitutions_begin[ASCII_MAX + 'R'] = "\e[91m"; // bright red
    substitutions_begin[ASCII_MAX + 'G'] = "\e[92m"; // bright green
    substitutions_begin[ASCII_MAX + 'Y'] = "\e[93m"; // bright yellow
    substitutions_begin[ASCII_MAX + 'B'] = "\e[94m"; // bright blue
    substitutions_begin[ASCII_MAX + 'M'] = "\e[95m"; // bright magenta
    substitutions_begin[ASCII_MAX + 'C'] = "\e[96m"; // bright cyan

    // backgrounds
    substitutions_begin[ASCII_MAX * 2 + 'l'] = "\e[40m"; // black
    substitutions_begin[ASCII_MAX * 2 + 'r'] = "\e[41m"; // red
    substitutions_begin[ASCII_MAX * 2 + 'g'] = "\e[42m"; // green
    substitutions_begin[ASCII_MAX * 2 + 'y'] = "\e[43m"; // yellow
    substitutions_begin[ASCII_MAX * 2 + 'b'] = "\e[44m"; // blue
    substitutions_begin[ASCII_MAX * 2 + 'm'] = "\e[45m"; // magenta
    substitutions_begin[ASCII_MAX * 2 + 'c'] = "\e[46m"; // cyan

    substitutions_begin[ASCII_MAX * 2 + 'L'] = "\e[107m"; // white
    substitutions_begin[ASCII_MAX * 2 + 'R'] = "\e[101m"; // bright red
    substitutions_begin[ASCII_MAX * 2 + 'G'] = "\e[102m"; // bright green
    substitutions_begin[ASCII_MAX * 2 + 'Y'] = "\e[103m"; // bright yellow
    substitutions_begin[ASCII_MAX * 2 + 'B'] = "\e[104m"; // bright blue
    substitutions_begin[ASCII_MAX * 2 + 'M'] = "\e[105m"; // bright magenta
    substitutions_begin[ASCII_MAX * 2 + 'C'] = "\e[106m"; // bright cyan
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

    char* substitutions_begin[ASCII_MAX * 3] = {0};
    char* substitutions_end[ASCII_MAX]       = {0};
    init_substitutions(substitutions_begin, substitutions_end);

    char* result = calloc(result_len(text, substitutions_begin, substitutions_end), sizeof(char));

    for (char* result_i = result; *text; text++)
    {
        if (*text == '\\') // provide '\' as an escape character
        {
            *result_i++ = *++text;
            continue;
        }

        if (*text == '^' || *text == '|')
        {
            while (text[0] == text[1]) text++;

            char escape  = text[0];
            char control = text[1];
            int  ctrl_i  = ASCII_MAX * (escape == '^' ? 1 : 2) + control;

            char* substitution = substitutions_begin[ctrl_i] ?
                substitutions_begin[ctrl_i] :
                substitutions_end[escape];

            while (*substitution)
                *result_i++ = *substitution++;

            if (control != '\\'             &&
                !substitutions_end[control] &&
                substitutions_begin[ctrl_i])
                text++;

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
