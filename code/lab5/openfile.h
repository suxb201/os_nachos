#ifndef OPENFILE_H
#define OPENFILE_H

#include "copyright.h"
#include "utility.h"

#ifdef FILESYS_STUB            // Temporarily implement calls to
// Nachos file system as calls to UNIX!
// See definitions listed under #else
class OpenFile {
public:
OpenFile(int f) { file = f; currentOffset = 0; }	// open the file
~OpenFile() { Close(file); }			// close the file

int ReadAt(char *into, int numBytes, int position) {
Lseek(file, position, 0);
return ReadPartial(file, into, numBytes);
}
int WriteAt(char *from, int numBytes, int position) {
Lseek(file, position, 0);
WriteFile(file, from, numBytes);
return numBytes;
}
int Read(char *into, int numBytes) {
int numRead = ReadAt(into, numBytes, currentOffset);
currentOffset += numRead;
return numRead;
}
int Write(char *from, int numBytes) {
int numWritten = WriteAt(from, numBytes, currentOffset);
currentOffset += numWritten;
return numWritten;
}

int Length() { Lseek(file, 0, 2); return Tell(file); }

private:
int file;
int currentOffset;
};

#else // FILESYS

class FileHeader;

class OpenFile {
public:
    OpenFile(int sector);

    ~OpenFile();            // Close the file

    void Seek(int position);        // Set the position from which to
    // start reading/writing -- UNIX lseek

    int Read(char *into, int numBytes); // Read/write bytes from the file,

    int Write(char *from, int numBytes);

    int ReadAt(char *into, int numBytes, int position);

    int WriteAt(char *from, int numBytes, int position);

    int Length();

    void WriteBack();

private:

    FileHeader *hdr;            // Header for this file
    int seekPosition;            // Current position within the file
    int headPosition;
};

#endif // FILESYS

#endif // OPENFILE_H
