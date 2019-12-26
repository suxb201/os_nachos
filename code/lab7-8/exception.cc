#include "copyright.h"
#include "system.h"
#include "syscall.h"

extern void StartProcess(int);

// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//	The result of the system call, if any, must be put back into r2.
void AdvancePC() {
    machine->WriteRegister(PCReg, machine->ReadRegister(PCReg) + 4);
    machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
}

void ExceptionHandler(ExceptionType which) {
    int type = machine->ReadRegister(2);  // 系统调用号
    printf("-------\nwhich: %d, type: %d\n", which, type);
    if (which == SyscallException) {
        switch (type) {
            case SC_Halt: {
                interrupt->Halt();
                break;
            }
            case SC_Exec: {
                char filename[128];
                int addr = machine->ReadRegister(4);
                int i = 0;
                do { machine->ReadMem(addr + i, 1, (int *) &filename[i]); } while (filename[i++] != '\0');
                OpenFile *executable = fileSystem->Open(filename);
                if (executable == NULL) {
                    printf("Unable to open file: %s", filename);
                    return;
                }
                AddrSpace *space = new AddrSpace(executable);
                delete executable;

                Thread *thread = new Thread(filename);

                printf("---space->getSpaceID():%d\n", space->getSpaceID());
                thread->space = space;
                thread->Fork(StartProcess, space->getSpaceID());
                printf("---sc exec:%s\n", filename);
                machine->WriteRegister(2, space->getSpaceID());
                AdvancePC();
                break;
            }
            case SC_Exit: {
                int ExitStatus = machine->ReadRegister(4);
//                machine->WriteRegister(2, ExitStatus);
//                currentThread->setExitStatus(ExitStatus);
//                if (ExitStatus == 99) {
//                    List *terminatedList = scheduler->getTerminatedList();
//                    scheduler->emptyList(terminatedList);
//                }
                delete currentThread->space;
                printf("--exit code:%d\n", ExitStatus);
                currentThread->Finish();
                printf("--currentThread->Finish(); ok\n");
                AdvancePC();
                break;
            }

//            case SC_Join: {
//                int SpaceId = machine->ReadRegister(4);
//                currentThread->Join(SpaceId);
//                machine->WriteRegister(2, currentThread->waitProcessExit());
//                break;
//            }
            default: {
                printf("Unexpected syscall %d %d\n", which, type);
                ASSERT(FALSE);
            }
        }
    } else {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
    }
}
