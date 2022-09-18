/**
 * extreme_edge_cases
 * CS 241 - Spring 2022
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camelCaser.h"
#include "camelCaser_tests.h"

int check_output(char** solution, char** output) {
    if (output == NULL)
        return 0;

    while (*solution != NULL && *output != NULL) {
        if (strcmp(*solution, *output)) {
            return 0;
        }
        solution ++;
        output ++;
    }

    if (*solution == NULL && *output == NULL)
        return 1;
    return 0;
}

 int test_camelCaser(char **(*camelCaser)(const char *),
                     void (*destroy)(char **)) {
    
    // test strings
    const char *input_str_1 = NULL;
    const char *input_str_2 = " hello world ";
    const char *input_str_3 = " hello... world ";
    const char *input_str_4 = "Considered to be (sacred, authoritative religious texts), the books were compiled\
                                by different biblical canons (official collections of). of the Bible and called the Torah (meaning \"law\", \"instruction\", or \"teaching\")";
    const char *input_str_5 = "Considered to be scriptures (sacred, authoritative religious texts), [14][15][78+++the books were compiled by different religious communities into";
    const char *input_str_6 = ".You've, tenacity and C. And the big picture - this course is a stepping stone - You're land (where you) your launch pad you for the \
                                big leagu CS4xx at a tier-1 CS school. python/Go/Javascript ... you' written in ... C. \n\n";
    const char *input_str_7 = "\0";
    const char *input_str_8 = "Welcome to CS241 System Programming Welcome to CS241 System Programming Welcome to CS241 System Programming Welcome to CS241 System o";
    const char *input_str_9 = "Welcome to CS241 System Programming Welcome to CS241 System Programming Welcome to CS241 System Programming Welcome to CS241 System.";
    const char *input_str_10 = "          \t\t\t\t\t\t\t\t\t\t\t";

    // solutions
    char *solution_2[] = {
        NULL
    };
    
    char *solution_3[] = {
        "hello",
        "",
        "",
        NULL
    };

     char *solution_4[] = {
        "consideredToBe",
        "sacred",
        "authoritativeReligiousTexts",
        "",
        "theBooksWereCompiledByDifferentBiblicalCanons",
        "officialCollectionsOf",
        "",
        "ofTheBibleAndCalledTheTorah",
        "meaning",
        "law",
        "",
        "",
        "instruction",
        "",
        "or",
        "teaching",
        "",
        NULL
     };
    
    char *solution_5[] = {
        "consideredToBeScriptures",
        "sacred",
        "authoritativeReligiousTexts",
        "",
        "",
        "14",
        "",
        "15",
        "",
        "78",
        "",
        "",
        NULL
    };

    char *solution_6[] = {
        "",
        "you",
        "ve",
        "tenacityAndC",
        "andTheBigPicture",
        "thisCourseIsASteppingStone",
        "you",
        "reLand",
        "whereYou",
        "yourLaunchPadYouForTheBigLeaguCs4xxAtATier",
        "1CsSchool",
        "python",
        "go",
        "javascript",
        "",
        "",
        "you",
        "writtenIn",
        "",
        "",
        "c",
        NULL
    };

    char *solution_7[] = {
        NULL
    };

    char *solution_8[] = {
        NULL
    };

    char *solution_9[] = {
        "welcomeToCs241SystemProgrammingWelcomeToCs241SystemProgrammingWelcomeToCs241SystemProgrammingWelcomeToCs241System",
        NULL
    };

    char *solution_10[] = {
        NULL
    };

    char **output_1 = camelCaser(input_str_1);
    char **output_2 = camelCaser(input_str_2);
    char **output_3 = camelCaser(input_str_3);
    char **output_4 = camelCaser(input_str_4);
    char **output_5 = camelCaser(input_str_5);
    char **output_6 = camelCaser(input_str_6);
    char **output_7 = camelCaser(input_str_7);
    char **output_8 = camelCaser(input_str_8);
    char **output_9 = camelCaser(input_str_9);
    char **output_10 = camelCaser(input_str_10);

    if (output_1 != NULL) {
        printf("Case 1 failed!\n");
        return 0;
    }

    if (!check_output(solution_2, output_2)) {
        printf("Case 2 failed!\n");
        return 0;
    }

    if (!check_output(solution_3, output_3)) {
        printf("Case 3 failed!\n");
        return 0;
    }

    if (!check_output(solution_4, output_4)) {
        printf("Case 4 failed!\n");
        return 0;
    }

    if (!check_output(solution_5, output_5)) {
        printf("Case 5 failed!\n");
        return 0;
    }

    if (!check_output(solution_6, output_6)) {
        printf("Case 6 failed!\n");
        return 0;
    }

    if (!check_output(solution_7, output_7)) {
        printf("Case 7 failed!\n");
        return 0;
    }

    if (!check_output(solution_8, output_8)) {
        printf("Case 8 failed!\n");
        return 0;
    }

    if (!check_output(solution_9, output_9)) {
        printf("Case 9 failed!\n");
        return 0;
    }

    if (!check_output(solution_10, output_10)) {
        printf("Case 10 failed!\n");
        return 0;
    }

    destroy(output_2);
    destroy(output_3);
    destroy(output_4);
    destroy(output_5);
    destroy(output_6);
    destroy(output_7);
    destroy(output_8);
    destroy(output_9);
    destroy(output_10);

    return 1;
}
