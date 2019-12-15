#include "copyright.h"
#include "utility.h"
#include "filehdr.h"
#include "directory.h"

// 目录初始化，其中每一个条都是没有使用
Directory::Directory(int size) {
    table = new DirectoryEntry[size];
    tableSize = size;
    for (int i = 0; i < tableSize; i++)
        table[i].inUse = FALSE;
}

// 删除目录 = 删除 entry table
Directory::~Directory() {
    delete[] table;
}

// 读取
void Directory::FetchFrom(OpenFile *file) {
    (void) file->ReadAt((char *) table, tableSize * sizeof(DirectoryEntry), 0);
}

// 写回
void Directory::WriteBack(OpenFile *file) {
    printf("----- %d\n",tableSize * sizeof(DirectoryEntry));
    (void) file->WriteAt((char *) table, tableSize * sizeof(DirectoryEntry), 0);
}

// 找到文件在目录表中第几项
int Directory::FindIndex(char *name) {
    for (int i = 0; i < tableSize; i++)
        if (table[i].inUse && !strncmp(table[i].name, name, FileNameMaxLen))
            return i;
    return -1;        // name not in directory
}

// 读取文件，给文件名，返回扇区号
int Directory::Find(char *name) {
    int i = FindIndex(name);
    if (i != -1)
        return table[i].sector;
    return -1;
}

// 在目录表中添加新文件记录
bool Directory::Add(char *name, int newSector) {
// 文件名重复
    if (FindIndex(name) != -1)
        return FALSE;
// 随便找一个空项，然后塞进去
    for (int i = 0; i < tableSize; i++)
        if (!table[i].inUse) {
            table[i].inUse = TRUE;
            strncpy(table[i].name, name, FileNameMaxLen);
            table[i].sector = newSector;
            return TRUE;
        }
//目录表没空间了
    return FALSE;
}

//在目录表中删除
bool Directory::Remove(char *name) {
    int i = FindIndex(name);
    if (i == -1)
        return FALSE;
    table[i].inUse = FALSE;
    return TRUE;
}

//列出所有文件
void Directory::List() {
    for (int i = 0; i < tableSize; i++)
        if (table[i].inUse)
            printf("%s\n", table[i].name);
}

//打印此目录下所有文件
void Directory::Print() {
    FileHeader *hdr = new FileHeader;

    printf("Directory contents:\n");
    for (int i = 0; i < tableSize; i++)
        if (table[i].inUse) {
            printf("Name: %s, Sector: %d\n", table[i].name, table[i].sector);
            hdr->FetchFrom(table[i].sector);
            hdr->Print();
        }
    printf("\n");
    delete hdr;
}
