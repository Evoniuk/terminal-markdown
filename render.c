#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    // substitutions acts as an associative array between ASCII chars
    // and their equivalent escape sequences in terminal markdown
    char* substitutions[128] = {0};
    substitutions['>'] = "\e[7m";   // begin inverted
    substitutions['<'] = "\e[27m";  // end inverted

    int result_len = 0;
    for (int i = 0; i < 128; i++)
        if (substitutions[i]) 
            result_len += strlen(substitutions[i]);

    char* result = calloc(result_len + 1, sizeof(char));

    for (char* result_index = result; *text; text++)
    {
        char* substitution = substitutions[*text];

        if (substitution)
            for (int i = 0; i < strlen(substitution); i++)
                *result_index++ = substitution[i];

        else *result_index++ = *text;
    }

    return result;
}
