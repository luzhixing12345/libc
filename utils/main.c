#include "xargparse.h"

int main(int argc, const char **argv) {
    int i;
    char *s, *dest;
    int src;
    int *other_numbers;
    argparse_option options[] = {XBOX_ARG_BOOLEAN(NULL, [-h][--help][help = "show help information"]),
                                 XBOX_ARG_BOOLEAN(NULL, [-v][--version][help = "show version"]),
                                 XBOX_ARG_INT(&i, [-i][--input][help = "input file"]),
                                 XBOX_ARG_STR(&s, [-s][--string]),
                                 XBOX_ARG_STR_GROUP(&dest, [name = dest][help = "destination"]),
                                 XBOX_ARG_INT_GROUP(&src, [name = src][help = "source"]),
                                 XBOX_ARG_INTS_GROUP(&other_numbers, [name = "other-number"][help = "catch the other number..."]),
                                 XBOX_ARG_END()};
    XBOX_argparse parser;
    XBOX_argparse_init(&parser, options, 0);
    XBOX_argparse_describe(&parser,
                           "ls",
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
        printf("i = %d\n", i);
    }
    if (XBOX_ismatch(&parser, "string")) {
        printf("s = %s\n", s);
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