#include "driver/arcdrv.h"
#include <string.h>

void arcDelete(s32 id);
BOOL arcDataCheck(void* addr);

//.bss
static ArchiveEntry work[4];

void arcInit(void) {
        int i;

        for (i = 0; i < 4; i++) {
                memset(&work[i], 0, sizeof(ArchiveEntry));
        }
}

BOOL arcDataCheck(void* addr) {
        ArchiveEntry* entry;
        s32 i;

        for (i = 0, entry = work; i < 4; i++, entry++) {
                if ((u8*)entry->data <= (u8*)addr &&
                    (u8*)entry->data + entry->size >= (u8*)addr) {
                        return 1;
                }
        }
        return 0;
}

void arcEntry(s32 id, void* data, u32 size) {
        ArchiveEntry* entry;

        entry = &work[id];
        entry->data = data;
        entry->size = size;
        ARCInitHandle(entry->data, &entry->handle);
        entry->flags |= 1;
}

void arcDelete(s32 id) {
        memset(&work[id], 0, sizeof(ArchiveEntry));
}

void* arcOpen(const char* filename, void** addr, u32* length) {
        ARCFileInfo info;
        void* startAddr;
        ArchiveEntry* entry;
        s32 i;

        entry = work;
        i = 0;
        while (i < 4) {
                if ((entry->flags & 1) && ARCOpen(&entry->handle, filename, &info)) {
                        startAddr = ARCGetStartAddrInMem(&info);
                        if (addr) {
                                *addr = startAddr;
                        }
                        if (length) {
                                *length = ARCGetLength(&info);
                        }
                        ARCClose(&info);
                        return startAddr;
                }
                i++;
                entry++;
        }
        if (addr) {
                *addr = NULL;
        }
        if (length) {
                *length = 0;
        }
        return NULL;
}
