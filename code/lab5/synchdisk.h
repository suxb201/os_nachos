#include "copyright.h"

#ifndef SYNCHDISK_H
#define SYNCHDISK_H

#include "disk.h"
#include "synch.h"

class SynchDisk {
public:
    SynchDisk(char *name);

    ~SynchDisk();

    void ReadSector(int sectorNumber, char *data);

    void WriteSector(int sectorNumber, char *data);

    void RequestDone();

private:
    Disk *disk;
    Semaphore *semaphore;
    Lock *lock;
};

#endif // SYNCHDISK_H
