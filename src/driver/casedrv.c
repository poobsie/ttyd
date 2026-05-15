#include "driver/casedrv.h"
#include "driver/hitdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <string.h>

typedef struct CaseEntry {
        u16 flags; //0x0
        u8 pad_2[2]; //0x2
        s32 activeConditionId; //0x4, TODO rename
        char hitObjName[0x40]; //0x8
        s32 caseId; //0x48
        u32 swFlag; //0x4C
        void* activeFunc; //0x50
        s32 lwData[16]; //0x54
        u8 field_0x94[4]; //0x94
        void* evtCode; //0x98, TODO re-type
        s32 priority; //0x9C
} CaseEntry;

typedef struct CaseWork {
        s32 count; //0x0
        CaseEntry* entries; //0x4
} CaseWork;

extern GlobalWork* gp;

//.sbss
static CaseWork work;
static CaseWork* wp = &work;


void caseInit(void) {
        wp->count = 64;
        wp->entries = __memAlloc(HEAP_DEFAULT, wp->count * sizeof(CaseEntry));
        memset(wp->entries, 0, wp->count * sizeof(CaseEntry));
}

void caseReInit(void) {
        CaseEntry* entry;
        s32 i;

        i = 0;
        entry = wp->entries;
        while (i < wp->count) {
                entry->flags &= ~1u;
                i++;
                entry++;
        }
}

s32 caseEntry(CaseSetup* setup) {
        CaseEntry* entry;
        s32 i;
        s32 n;

        i = 0;
        for (n = wp->count, entry = wp->entries; n > 0; n--) {
                if (!(entry->flags & 1)) break;
                i++;
                entry++;
        }
        memset(entry, 0, sizeof(CaseEntry));
        entry->caseId = i;
        entry->flags = 0;
        entry->activeConditionId = setup->activeConditionId & ~0x8000u;
        if (setup->activeConditionId & 0x8000) {
                entry->flags |= 4u;
        }
        entry->flags |= 1u;
        strcpy(entry->hitObjName, setup->hitObjName);
        entry->swFlag = setup->swFlag;
        entry->activeFunc = setup->activeFunc;
        entry->evtCode = setup->evtCode;
        entry->priority = setup->priority;
        entry->lwData[16] = 0;
        memcpy(entry->lwData, setup->lwData, sizeof(entry->lwData));
        return i;
}

void caseDelete(s32 caseId) {
        wp->entries[caseId].flags &= ~1u;
}

void caseMain(void) {

}

CaseEntry* caseCheckHitObj(HitObj* hitobj) {
        CaseEntry* entry;
        s32 i;

        if (gp->inBattle) return NULL;
        i = 0;
        entry = wp->entries;
        while (i < wp->count) {
                if ((entry->flags & 1) &&
                    entry->activeConditionId != 0x11 &&
                    (u32)(entry->activeConditionId - 0x12) > 1u) {
                        if (strcmp(hitGetName(hitobj), entry->hitObjName) == 0) {
                                return entry;
                        }
                }
                i++;
                entry++;
        }
        return NULL;
}

CaseEntry* caseCheckHitObj2(HitObj* hitobj) {
        CaseEntry* entry;
        s32 i;

        if (gp->inBattle) return NULL;
        i = 0;
        entry = wp->entries;
        while (i < wp->count) {
                if ((entry->flags & 1) &&
                    entry->activeConditionId != 0x11 &&
                    (u32)(entry->activeConditionId - 0x12) > 1u &&
                    entry->activeConditionId == 0xF) {
                        if (strcmp(hitGetName(hitobj), entry->hitObjName) == 0) {
                                return entry;
                        }
                }
                i++;
                entry++;
        }
        return NULL;
}

CaseEntry* caseIdToPtr(s32 id) {
        CaseEntry* entry;

        entry = &wp->entries[id];
        if (!(entry->flags & 1)) {
                return NULL;
        }
        return entry;
}
