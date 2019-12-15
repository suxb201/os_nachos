
#include "copyright.h"
#include "filehdr.h"
#include "openfile.h"
#include "system.h"

OpenFile::OpenFile(int sector) {
    this->headPosition = sector;
    hdr = new FileHeader;
    hdr->FetchFrom(sector);
    seekPosition = 0;
}

OpenFile::~OpenFile() {
    delete hdr;
}

void OpenFile::Seek(int position) {
    seekPosition = position;
}

int OpenFile::Read(char *into, int numBytes) {
    int result = ReadAt(into, numBytes, seekPosition);
    seekPosition += result;
    return result;
}

int OpenFile::Write(char *into, int numBytes) {
    int result = WriteAt(into, numBytes, seekPosition);
    seekPosition += result;
    return result;
}

int OpenFile::ReadAt(char *into, int numBytes, int position) {
    int fileLength = hdr->FileLength();
    int i, firstSector, lastSector, numSectors;
    char *buf;

    if ((numBytes <= 0) || (position >= fileLength))
        return 0;                // check request
    if ((position + numBytes) > fileLength)
        numBytes = fileLength - position;
    DEBUG('f', "Reading %d bytes at %d, from file of length %d.\n",
          numBytes, position, fileLength);

    firstSector = divRoundDown(position, SectorSize);
    lastSector = divRoundDown(position + numBytes - 1, SectorSize);
    numSectors = 1 + lastSector - firstSector;

    // read in all the full and partial sectors that we need
    buf = new char[numSectors * SectorSize];
    for (i = firstSector; i <= lastSector; i++)
        synchDisk->ReadSector(hdr->ByteToSector(i * SectorSize),
                              &buf[(i - firstSector) * SectorSize]);

    // copy the part we want
    bcopy(&buf[position - (firstSector * SectorSize)], into, numBytes);
    delete[] buf;
    return numBytes;
}

int OpenFile::WriteAt(char *from, int numBytes, int position) {
    int fileLength = hdr->FileLength();
    int i, firstSector, lastSector, numSectors;
    bool firstAligned, lastAligned;
    char *buf;

    if ((numBytes <= 0) || (position > fileLength))
        return -1;                // 参数错误
    if ((position + numBytes) > fileLength) {
        int incrementBytes = (position + numBytes) - fileLength;
        BitMap *freeBitMap = fileSystem->getBitMap();
        bool hdrRet;
        hdrRet = hdr->Allocate(freeBitMap, fileLength, incrementBytes);
        if (!hdrRet) return -1;
        fileSystem->setBitMap(freeBitMap);
    }
//        numBytes = fileLength - position;
    DEBUG('f', "Writing %d bytes at %d, from file of length %d.\n",
          numBytes, position, fileLength);

    firstSector = divRoundDown(position, SectorSize);
    lastSector = divRoundDown(position + numBytes - 1, SectorSize);
    numSectors = 1 + lastSector - firstSector;

    buf = new char[numSectors * SectorSize];

    firstAligned = (bool) (position == (firstSector * SectorSize));
    lastAligned = (bool) ((position + numBytes) == ((lastSector + 1) * SectorSize));

// read in first and last sector, if they are to be partially modified
    if (!firstAligned)
        ReadAt(buf, SectorSize, firstSector * SectorSize);
    if (!lastAligned && ((firstSector != lastSector) || firstAligned))
        ReadAt(&buf[(lastSector - firstSector) * SectorSize],
               SectorSize, lastSector * SectorSize);

// copy in the bytes we want to change 
    bcopy(from, &buf[position - (firstSector * SectorSize)], numBytes);

// write modified sectors back
    for (i = firstSector; i <= lastSector; i++)
        synchDisk->WriteSector(hdr->ByteToSector(i * SectorSize),
                               &buf[(i - firstSector) * SectorSize]);
    delete[] buf;
    return numBytes;
}

//----------------------------------------------------------------------
// OpenFile::Length
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

int
OpenFile::Length() {
    return hdr->FileLength();
}

void OpenFile::WriteBack() {
    hdr->WriteBack(headPosition);
}