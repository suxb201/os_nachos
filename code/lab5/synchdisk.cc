#include "copyright.h"
#include "synchdisk.h"

static void DiskRequestDone(_int arg) {
    SynchDisk *dsk = (SynchDisk *) arg;    // disk -> dsk

    dsk->RequestDone();                    // disk -> dsk
}

SynchDisk::SynchDisk(char *name) {
    semaphore = new Semaphore("synch disk", 0);
    lock = new Lock("synch disk lock");
    disk = new Disk(name, DiskRequestDone, (_int)
    this);
}

SynchDisk::~SynchDisk() {
    delete disk;
    delete lock;
    delete semaphore;
}

void SynchDisk::ReadSector(int sectorNumber, char *data) {
    lock->Acquire();            // only one disk I/O at a time
    disk->ReadRequest(sectorNumber, data);
    semaphore->P();            // wait for interrupt
    lock->Release();
}

void SynchDisk::WriteSector(int sectorNumber, char *data) {
    lock->Acquire();            // only one disk I/O at a time
    disk->WriteRequest(sectorNumber, data);
    semaphore->P();            // wait for interrupt
    lock->Release();
}

void SynchDisk::RequestDone() {
    semaphore->V();
}
