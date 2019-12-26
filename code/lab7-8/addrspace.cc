#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#include "bitmap.h"
#include <string.h>

static void SwapHeader(NoffHeader *noffH) {
    noffH->noffMagic = WordToHost(noffH->noffMagic);
    noffH->code.size = WordToHost(noffH->code.size);
    noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
    noffH->initData.size = WordToHost(noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost(noffH->uninitData.size);
    noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

#define MAX_USERPROCESSES 256
BitMap *bitmap;
bool ThreadMap[MAX_USERPROCESSES];

AddrSpace::AddrSpace(OpenFile *executable) {
    // ---------------------------
    bool hasPid = false;
    for (int i = 100; i < MAX_USERPROCESSES; i++) {
        if (!ThreadMap[i]) {
            ThreadMap[i] = true;
            spaceID = i;
            hasPid = true;
            break;
        }
    }
    if (!hasPid) {
        printf("too many processes in NACHOS\n");
        return;
    }
    if (bitmap == NULL) {
        bitmap = new BitMap(NumPhysPages);
    }

    NoffHeader noffH; // Noff Header 中有四项：magixnumber code init uninit
    unsigned int i, size;
    executable->ReadAt((char *) &noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && (WordToHost(noffH.noffMagic) == NOFFMAGIC))
        SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);
    // ------------ 上述代码从文件中读入 noffH --------------

    // 地址空间大小 = 代码段 + 初始化数据段 + 未初始化数据段 + 用户栈
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize;
    // we need to increase the size to leave room for the stack

    numPages = divRoundUp(size, PageSize); // PageSize = SectorSize = 128
    size = numPages * PageSize;
    ASSERT(numPages <= NumPhysPages); // NumPhysPages 是 32
    // check we're not trying to run anything too big -- at least until we have virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", numPages, size);
    // 首先，设置映射关系
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
        pageTable[i].virtualPage = i;    // vir 和 phy 相等， 1:1 映射
        pageTable[i].physicalPage = bitmap->Find();
        pageTable[i].valid = TRUE;  // 未初始化
        pageTable[i].use = FALSE;   // 未使用
        pageTable[i].dirty = FALSE; // 未修改
        pageTable[i].readOnly = FALSE;  // 如果这个页全是代码段，可以设置为只读
    }
    // 先读入 tmp_momory
    char *tmp_momory = new char[size];
    if (noffH.code.size > 0) {
        executable->ReadAt(&(tmp_momory[noffH.code.virtualAddr]), noffH.code.size, noffH.code.inFileAddr);
    }
    if (noffH.initData.size > 0) {
        executable->ReadAt(&(tmp_momory[noffH.initData.virtualAddr]), noffH.initData.size, noffH.initData.inFileAddr);
    }
    // 逐页转存到 mainMemory
    for (int i = 0; i < numPages; i++) {
        memcpy(&(machine->mainMemory[pageTable[i].physicalPage * PageSize]),
               &(tmp_momory[pageTable[i].virtualPage * PageSize]), PageSize);
    }
    delete tmp_momory;

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace() {
    ThreadMap[spaceID] = 0;
    for (int i = 0; i < numPages; i++) {
        bitmap->Clear(pageTable[i].physicalPage);
    }
    delete[] pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void AddrSpace::InitRegisters() {
    for (int i = 0; i < NumTotalRegs; i++)
        machine->WriteRegister(i, 0);  // registers[num] = value;
    // 初始化 PC -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);
    // next PC
    machine->WriteRegister(NextPCReg, 4);
    // 栈低向上偏移 16
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() {}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() {
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}


void AddrSpace::Print() {
    printf("page table dump: %d pages in total\n", numPages);
    printf("=========================================\n");
    printf("\tVirtPage,\tPhysPage\n");
    for (int i = 0; i < numPages; i++) {
        printf("\t %d,\t\t%d\n", pageTable[i].virtualPage, pageTable[i].physicalPage);
    }
    printf("=========================================\n");
}