#include "copyright.h"

#ifndef FILEHDR_H
#define FILEHDR_H

#include "disk.h"
#include "bitmap.h"

#define NumDirect    ((SectorSize - 2 * sizeof(int)) / sizeof(int))
#define MaxFileSize    (NumDirect * SectorSize)

class FileHeader {
public:
    bool Allocate(BitMap *bitMap, int fileSize);

    bool Allocate(BitMap *bitMap, int fileSize, int incrementBytes);

    void Deallocate(BitMap *bitMap);

    void FetchFrom(int sectorNumber);

    void WriteBack(int sectorNumber);

    int ByteToSector(int offset);

    int FileLength();

    void Print();            // Print the contents of the file.

private:
    int numBytes;            // Number of bytes in the file
    int numSectors;            // Number of data sectors in the file
    int dataSectors[NumDirect];        // Disk sector numbers for each data
};

#endif // FILEHDR_H
