#include "battle/ac/ac_timing_a.h"
#include "battle/battle_pad.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "system.h"
#include <string.h>

extern BattleWork* _battleWorkPointer;

static void actionCommandDisp(f32 iconX, f32 iconY);

s32 battleAcTimingA_GetSuccessFrame(BattleWork* work);
s32 battleAcResult_TimingA(BattleWork* work);
void battleAcDisp_TimingA(CameraId camId, void* param);
void battleAcDelete_TimingA(BattleWork* work);

s32 _ac_timing_a_timing_lv_0[] = {
    4, 10,
    5, 9,
    6, 8,
    7, 7,
    8, 6,
    9, 5,
    0x1869F, 1,
};

s32 _ac_timing_a_timing_lv_1[] = {
    0, 10,
    1, 9,
    2, 8,
    3, 7,
    4, 6,
    5, 5,
    6, 4,
    7, 4,
    8, 3,
    9, 2,
    0x1869F, 1,
};

s32 _ac_timing_a_timing_lv_2[] = {
    0, 9,
    1, 8,
    2, 7,
    3, 6,
    4, 5,
    5, 4,
    6, 3,
    7, 2,
    8, 1,
    9, 1,
    0x1869F, 1,
};

s32 _ac_timing_a_timing_lv_3[] = {
    0, 8,
    1, 7,
    2, 6,
    3, 5,
    4, 4,
    5, 3,
    6, 2,
    8, 1,
    9, 1,
    0x1869F, 1,
};

s32 _ac_timing_a_timing_lv_4[] = {
    0, 7,
    1, 6,
    2, 5,
    3, 4,
    4, 3,
    5, 2,
    8, 1,
    9, 1,
    0x1869F, 1,
};

s32 _ac_timing_a_timing_lv_5[] = {
    0, 6,
    1, 5,
    2, 4,
    3, 3,
    4, 2,
    8, 1,
    9, 1,
    0x1869F, 1,
};

s32 _ac_timing_a_timing_lv_6[] = {
    0, 5,
    1, 4,
    2, 3,
    3, 2,
    8, 1,
    9, 1,
    0x1869F, 1,
};

s32* _ac_timing_a_timing_tbl[7] = {
    _ac_timing_a_timing_lv_0,
    _ac_timing_a_timing_lv_1,
    _ac_timing_a_timing_lv_2,
    _ac_timing_a_timing_lv_3,
    _ac_timing_a_timing_lv_4,
    _ac_timing_a_timing_lv_5,
    _ac_timing_a_timing_lv_6,
};

const Vec vec3_802efbc8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802efbd4 = {0.0f, 0.0f, 0.0f};

s32 battleAcMain_TimingA(BattleWork* work) {
    BattleACExtraParams* extraParams;
    BattleACDispParams* dispParams;

    extraParams = &work->actionCommands.extraParams;
    dispParams = &work->actionCommands.dispParams;

    switch (work->actionCommands.mAcState) {
        case 0:
            memset(dispParams, 0, sizeof(BattleACDispParams));
            dispParams->iconX = -300.0f;
            *(s32*)(dispParams->field_0x1C + 4) = 0x14;
            dispParams->iconY = 0.0f;
            work->actionCommands.mAcState = 0x63;
            *(s32*)(extraParams->field_0x0 + 4) &= ~1;
            *(s32*)(&work->actionCommands.field_0x34[0x24]) = 0;
            break;
        case 0x64:
            break;
        case 0x3e8:
            *(s32*)(extraParams->field_0x0 + 8) = 0x3c;
            work->actionCommands.mAcState = 0x3e9;
        case 0x3e9:
            *(s32*)(extraParams->field_0x0 + 8) = *(s32*)(extraParams->field_0x0 + 8) - 1;
            if (*(s32*)(extraParams->field_0x0 + 8) > 0) return 1;
            work->actionCommands.mAcState = 0x3ea;
            break;
        case 0x3ea:
            work->actionCommands.maincb = NULL;
            work->actionCommands.dispcb = NULL;
            work->actionCommands.resultcb = NULL;
            work->actionCommands.deletecb = NULL;
            return 0;
        default:
            break;
    }
    return 1;
}

s32 battleAcTimingA_GetSuccessFrame(BattleWork* work) {
    s32* tbl;
    s32 flags;
    s32 mResultCount;

    tbl = _ac_timing_a_timing_tbl[BattleActionCommandGetDifficulty(work)];
    flags = *(s32*)(work->actionCommands.extraParams.field_0x0 + 4);
    mResultCount = work->actionCommands.mResultCount;

    if (!(flags & 2)) {
        return tbl[1];
    }

    if (mResultCount >= 0x186A0) {
        return 0;
    }

    while (mResultCount > tbl[0]) {
        tbl += 2;
    }
    return tbl[1];
}

s32 battleAcResult_TimingA(BattleWork* work) {
    s32 successFrame;
    BattleWorkUnit* unit;
    s32 hitCount;
    s32 i;

    successFrame = battleAcTimingA_GetSuccessFrame(work);
    hitCount = 0;

    if (work->actionCommands.field_0x4 & 1) {
        unit = work->actionCommands.mAcUnit;
        if (unit->mTokenFlags & kConfusedAction) {
            if (irand(100) < 0) {
                work->actionCommands.result = 2;
                work->actionCommands.mResultCount++;
            } else {
                work->actionCommands.result = 0;
            }
            return work->actionCommands.result;
        }
    }

    unit = work->actionCommands.mAcUnit;
    if (unit->mBadgesEquipped.mAutoCommandBadge != 0) {
        work->actionCommands.result = 2;
        work->actionCommands.mResultCount++;
        return work->actionCommands.result;
    }

    work->actionCommands.result = 0;

    if (work->flags & 0x01000000) {
        *(s32*)(&work->actionCommands.field_0x34[0x24]) = 3;
        work->actionCommands.result = 2;
        work->actionCommands.mResultCount++;
        return work->actionCommands.result;
    }

    for (i = 0; i < 0x14; i++) {
        if (BattlePadCheckRecordTrigger(i, 0x100)) {
            hitCount++;
        }
    }

    if (hitCount >= 2) {
        *(s32*)(&work->actionCommands.field_0x34[0x24]) = 1;
        return work->actionCommands.result;
    }

    i = 0;
    while (i < successFrame) {
        if (BattlePadCheckRecordTrigger(i, 0x100)) {
            break;
        }
        i++;
    }

    if (i >= successFrame) {
        if (hitCount > 0) {
            *(s32*)(&work->actionCommands.field_0x34[0x24]) = 1;
        } else {
            *(s32*)(&work->actionCommands.field_0x34[0x24]) = 2;
        }
        return work->actionCommands.result;
    }

    *(s32*)(&work->actionCommands.field_0x34[0x24]) = 3;
    work->actionCommands.result = 2;
    work->actionCommands.mResultCount++;
    return work->actionCommands.result;
}

void battleAcDisp_TimingA(CameraId camId, void* param) {
    BattleACDispParams* dispParams;
    s32 timer;

    dispParams = &((BattleWork*)param)->actionCommands.dispParams;

    switch (((BattleWork*)param)->actionCommands.mAcState) {
        case 0x63:
            timer = *(s32*)(dispParams->field_0x1C + 4);
            dispParams->iconX = intplGetValue(4, 0x14 - timer, 0x14, -300.0f, 30.0f);
        case 0x64:
            dispParams->iconX = 30.0f;
            actionCommandDisp(dispParams->iconX, dispParams->iconY);
            timer = *(s32*)(dispParams->field_0x1C + 4);
            if (timer > 0) {
                *(s32*)(dispParams->field_0x1C + 4) = timer - 1;
            }
            break;
        case 0x3e8:
        case 0x3e9:
            timer = *(s32*)(dispParams->field_0x1C + 4);
            if (timer >= 0x28) {
                dispParams->iconX = intplGetValue(4, timer - 0x28, 0x14, 30.0f, -300.0f);
            } else {
                dispParams->iconX = 30.0f;
            }
            actionCommandDisp(dispParams->iconX, dispParams->iconY);
            timer = *(s32*)(dispParams->field_0x1C + 4);
            if (timer < 0x3c) {
                *(s32*)(dispParams->field_0x1C + 4) = timer + 1;
            }
            break;
        default:
            break;
    }
}

void battleAcDelete_TimingA(BattleWork* work) {
    work->actionCommands.mAcState = 0x3e8;
}

static void actionCommandDisp(f32 iconX, f32 iconY) {
    BattleWork* work;
    BattleACDispParams* dispParams;
    Vec posA;
    Vec posB;

    work = _battleWorkPointer;
    dispParams = &work->actionCommands.dispParams;
    camGetPtr(8);

    if (*(s32*)(work->actionCommands.extraParams.field_0x0 + 4) & 1) {
        posA = vec3_802efbc8;
        posA.x = -200.0f + iconX;
        posA.y = 50.0f + iconY;
        iconDispGx(posA, 0x10, 0x65, 1.0f);
    } else {
        posB = vec3_802efbd4;
        posB.x = -200.0f + iconX;
        posB.y = 50.0f + iconY;
        iconDispGx(posB, 0x10, 0x64, 1.0f);
    }

    (*(s32*)dispParams->field_0x0)++;
}
