#include <CppUTest/CommandLineTestRunner.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Executing: %s...\n", argv[0]);
    return RUN_ALL_TESTS(argc, argv);
}
