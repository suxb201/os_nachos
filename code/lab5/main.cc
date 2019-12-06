#define MAIN

#include "copyright.h"

#undef MAIN

#include "utility.h"
#include "system.h"

// External functions used by this file

extern void ThreadTest(void), Copy(char *unixFile, char *nachosFile);

extern void Append(char *unixFile, char *nachosFile, int half);

extern void NAppend(char *nachosFile0, char *nachosFile1);

extern void Print(char *file), PerformanceTest(void);

extern void StartProcess(char *file), ConsoleTest(char *in, char *out);

extern void MailTest(int networkID);

int main(int argc, char **argv) {
    int argCount;
    DEBUG('t', "Entering main");
    (void) Initialize(argc, argv);

    for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
        argCount = 1;
        if (!strcmp(*argv, "-cp")) {
            ASSERT(argc > 2);
            Copy(*(argv + 1), *(argv + 2));
            argCount = 3;
        } else if (!strcmp(*argv, "-ap")) {  // 追加文件
            ASSERT(argc > 2);
            Append(*(argv + 1), *(argv + 2), 0);
            argCount = 3;
        } else if (!strcmp(*argv, "-hap")) {
            // 追加到中间
            ASSERT(argc > 2);
            Append(*(argv + 1), *(argv + 2), 1);
            argCount = 3;
        } else if (!strcmp(*argv, "-nap")) {  // n2n
            ASSERT(argc > 2);
            NAppend(*(argv + 1), *(argv + 2));
            argCount = 3;
        } else if (!strcmp(*argv, "-p")) {    // print a Nachos file
            ASSERT(argc > 1);
            Print(*(argv + 1));
            argCount = 2;
        } else if (!strcmp(*argv, "-r")) {    // remove Nachos file
            ASSERT(argc > 1);
            fileSystem->Remove(*(argv + 1));
            argCount = 2;
        } else if (!strcmp(*argv, "-l")) {    // list Nachos directory
            fileSystem->List();
        } else if (!strcmp(*argv, "-D")) {    // print entire filesystem
            fileSystem->Print();
        } else if (!strcmp(*argv, "-t")) {    // performance test
            PerformanceTest();
        }

    }
    currentThread->Finish();
    return (0);            // Not reached...
}
