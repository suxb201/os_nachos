#include "copyright.h"
#include "system.h"
#include "filehdr.h"

//SectorSize 扇区大小
//numSectors 这个文件需要的扇区数量
bool FileHeader::Allocate(BitMap *freeMap, int fileSize) {
    numBytes = fileSize;
    numSectors = divRoundUp(fileSize, SectorSize);
//    剩余空间不足
    if (freeMap->NumClear() < numSectors)
        return FALSE;        // not enough space
    for (int i = 0; i < numSectors; i++)
        dataSectors[i] = freeMap->Find();
    return TRUE;
}

bool FileHeader::Allocate(BitMap *freeMap, int fileSize, int incrementBytes) {
    printf("fileSize: %d, incrementBytes: %d.\n", fileSize, incrementBytes);
    if (numSectors > 30)
        return false;

    numBytes = fileSize;
    int offset = numBytes % SectorSize;
    if (offset == 0) offset = SectorSize;
    int newSectorBytes = incrementBytes - (SectorSize - (offset));
    printf("newSectorBytes: %d\n", newSectorBytes);
    if (newSectorBytes <= 0) {
        numBytes = numBytes + incrementBytes;
        return true;
    }
    int moreSectors = divRoundUp(newSectorBytes, SectorSize);
    if (numSectors + moreSectors > 30) return false;
    if (freeMap->NumClear() < moreSectors) return false;
    for (int i = numSectors; i < numSectors + moreSectors; i++)
        dataSectors[i] = freeMap->Find();
    numBytes = numBytes + incrementBytes;
    numSectors = numSectors + moreSectors;
    return true;
}

//numSectors 这个文件需要的扇区数量
void FileHeader::Deallocate(BitMap *freeMap) {
    for (int i = 0; i < numSectors; i++) {
        ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
        freeMap->Clear((int) dataSectors[i]);
    }
}


void FileHeader::FetchFrom(int sector) {
    synchDisk->ReadSector(sector, (char *) this);
}

void FileHeader::WriteBack(int sector) {
    synchDisk->WriteSector(sector, (char *) this);
}

int FileHeader::ByteToSector(int offset) {
    return (dataSectors[offset / SectorSize]);
}

int FileHeader::FileLength() {
    return numBytes;
}

void FileHeader::Print() {
    int i, j, k;
    char *data = new char[SectorSize];

    printf("FileHeader contents.  \nFile size: %d.  \nFile blocks:\n",
           numBytes);
    for (i = 0; i < numSectors; i++)
        printf("%d ", dataSectors[i]);
    printf("\nFile contents:\n");
    for (i = k = 0; i < numSectors; i++) {
        synchDisk->ReadSector(dataSectors[i], data);
        for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++) {
            if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
                printf("%c", data[j]);
            else
                printf("\\%x", (unsigned char) data[j]);
        }
        printf("\n");
    }
    printf("********************************************\n");
    delete[] data;
}
