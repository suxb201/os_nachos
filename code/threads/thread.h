
#ifndef THREAD_H
#define THREAD_H

#include "copyright.h"
#include "utility.h"

#ifdef USER_PROGRAM
#include "machine.h"
#include "addrspace.h"
#endif


#define MachineStateSize 18


// Size of the thread's private execution stack.
// WATCH OUT IF THIS ISN'T BIG ENOUGH!!!!!
#define StackSize    (sizeof(_int) * 1024)    // in words


// Thread state
enum ThreadStatus {
    JUST_CREATED, RUNNING, READY, BLOCKED
};

// external function, dummy routine whose sole job is to call Thread::Print
extern void ThreadPrint(_int arg);

class Thread {
private:
    // NOTE: DO NOT CHANGE the order of these first two members.
    // THEY MUST be in this position for SWITCH to work.
    int *stackTop;             // the current stack pointer
    _int machineState[MachineStateSize];  // all registers except for stackTop

public:
    Thread(char *debugName);        // initialize a Thread
    ~Thread();                // deallocate a Thread

//    void Join(int SpaceId);
//
//    void Finish();
//
//    void Terminated();

    void Fork(VoidFunctionPtr func, _int arg);    // Make thread run (*func)(arg)
    void Yield();                // Relinquish the CPU if any
    // other thread is runnable
    void Sleep();                // Put the thread to sleep and
    // relinquish the processor
    void Finish();                // The thread is done executing

    void CheckOverflow();            // Check if thread has
    // overflowed its stack
    void setStatus(ThreadStatus st) { status = st; }

    char *getName() { return (name); }

    void Print() { printf("%s, ", name); }

private:
    // some of the private data for this class is listed above

    int *stack;            // Bottom of the stack
    // NULL if this is the main thread
    // (If NULL, don't deallocate stack)
    ThreadStatus status;        // ready, running or blocked
    char *name;

    void StackAllocate(VoidFunctionPtr func, _int arg);
    // Allocate a stack for thread.
    // Used internally by Fork()

#ifdef USER_PROGRAM
    // A thread running a user program actually has *two* sets of CPU registers --
    // one for its state while executing user code, one for its state
    // while executing kernel code.

        int userRegisters[NumTotalRegs];	// user-level CPU register state

      public:
        void SaveUserState();		// save user-level register state
        void RestoreUserState();		// restore user-level register state

        AddrSpace *space;			// User code this thread is running.
#endif
};

// Magical machine-dependent routines, defined in switch.s

extern "C" {
// First frame on thread execution stack; 
//   	enable interrupts
//	call "func"
//	(when func returns, if ever) call ThreadFinish()
void ThreadRoot();

// Stop running oldThread and start running newThread
void SWITCH(Thread *oldThread, Thread *newThread);
}

#endif // THREAD_H
