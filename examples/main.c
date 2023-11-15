#include "xargparse.h"

int main(int argc, const char **argv) {
    int integer;
    char *str, *dest = NULL;
    int src;
    int *other_numbers;
    argparse_option options[] = {
        XBOX_ARG_BOOLEAN(NULL, "-h", "--help", "show help information", NULL, "help"),
        XBOX_ARG_BOOLEAN(NULL, "-v", "--version", "show version", NULL, "version"),
        XBOX_ARG_INT(&integer, "-i", "--input", "input file", " <NUMBER>", "input"),
        XBOX_ARG_STR(&str, "-s", "--string", NULL, " <STRING>", "string"),
        XBOX_ARG_STR_GROUP(&dest, NULL, NULL, "destination", NULL, "dest"),
        XBOX_ARG_INT_GROUP(&src, NULL, NULL, "source", NULL, "src"),
        XBOX_ARG_INTS_GROUP(&other_numbers, NULL, NULL, "catch the other number ...", NULL, "other-number"),
        XBOX_ARG_END()};

    XBOX_argparse parser;
    XBOX_argparse_init(&parser, options, XBOX_ARGPARSE_ENABLE_ARG_STICK);
    XBOX_argparse_describe(&parser,
                           "main",
                           "\nA brief description of what the program does and how it works.",
                           "\nAdditional description of the program after the description of the arguments.");
    XBOX_argparse_parse(&parser, argc, argv);
    if (XBOX_ismatch(&parser, "help")) {
        XBOX_argparse_info(&parser);
    }
    if (XBOX_ismatch(&parser, "version")) {
        printf("v0.0.1\n");
    }
    if (XBOX_ismatch(&parser, "input")) {
        printf("i = %d\n", integer);
    }
    if (XBOX_ismatch(&parser, "string")) {
        printf("s = %s\n", str);
    }
    if (XBOX_ismatch(&parser, "dest")) {
        printf("dest = %s\n", dest);
    }
    if (XBOX_ismatch(&parser, "src")) {
        printf("src = %d\n", src);
    }
    int n = XBOX_ismatch(&parser, "other-number");
    for (int i = 0; i < n; i++) {
        printf("other number[%d] = %d\n", i, other_numbers[i]);
    }
    XBOX_free_argparse(&parser);
    return 0;
}