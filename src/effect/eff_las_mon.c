/* LasMon (Laser Monster) effect */

#include "eff/eff_las_mon.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/mtx.h>

extern GlobalWork* gp;

typedef struct EffLasMonData {
    /* 0x00 */ s32 type;
    /* 0x04 */ f32 x;
    /* 0x08 */ f32 y;
    /* 0x0C */ f32 z;
    /* 0x10 */ f32 scale;
    /* 0x14 */ s32 animHandle;
    /* 0x18 */ s32 animState;
    /* 0x1C */ s32 alpha;
    /* 0x20 */ s32 counter;
} EffLasMonData; /* size 0x24 */

static const GXColor dat_80422408 = {0xFF, 0xFF, 0xFF, 0x00};
static const Vec vec3_80301438 = {0.0f, 0.0f, 0.0f};

void effLasMonMain(EffectEntry* effect);
void effLasMonDisp(CameraId camId, void* param);
void animPoseDrawMtx(s32 poseId, Mtx matrix, s32 xluMode, f32 rotY, f32 scale);

EffectEntry* effLasMonEntry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    EffLasMonData* data;

    effect = effEntry();
    effect->type = "LasMon";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(EffLasMonData) * effect->count);
    effect->userdata = data;
    effect->callback = effLasMonMain;
    effect->flags |= 2;

    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = 1.0f;
    data->animState = 0;
    data->animHandle = -1;
    data->alpha = 0xFF;
    data->counter = 0x3E8;

    return effect;
}

void effLasMonMain(EffectEntry* effect) {
    Vec pos;
    Vec tempPos;
    EffLasMonData* data;
    s32 isInBattle;

    data = effect->userdata;
    *(s32*)&tempPos.x = *(s32*)&vec3_80301438.x;
    *(s32*)&tempPos.y = *(s32*)&vec3_80301438.y;
    tempPos.x = data->x;
    *(s32*)&tempPos.z = *(s32*)&vec3_80301438.z;
    tempPos.y = data->y;
    tempPos.z = data->z;
    *(s32*)&pos.x = *(s32*)&tempPos.x;
    *(s32*)&pos.y = *(s32*)&tempPos.y;
    *(s32*)&pos.z = *(s32*)&tempPos.z;
    isInBattle = (s32)((u32)((-gp->inBattle) | gp->inBattle) >> 31);

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->counter = 0x10;
    }

    if (data->counter < 0x3E8) {
        data->counter--;
    }

    if (data->counter < 0x10) {
        data->alpha = data->counter * 0x10;
    }

    if (data->counter < 0) {
        if (data->animHandle != -1) {
            animPoseRelease(data->animHandle);
        }
        effDelete(effect);
        return;
    }

    if (animGroupBaseAsync("MOBJ_EFF_las_mon", isInBattle, NULL)) {
        if (data->animHandle == -1) {
            data->animHandle = animPoseEntry("MOBJ_EFF_las_mon", isInBattle);
            animPoseSetAnim(data->animHandle, "A_1", 1);
            data->animState = 0;
        }
        switch (data->animState) {
        case 0:
            if (animPoseGetLoopTimes(data->animHandle) >= 1.0f) {
                animPoseSetAnim(data->animHandle, "S_1", 1);
                data->animState++;
            }
            break;
        case 1:
            break;
        }
        dispEntry(4, 2, effLasMonDisp, effect, dispCalcZ(&pos));
    }
}

void effLasMonDisp(CameraId camId, void* param) {
    Mtx mtxTrans;
    Mtx mtxScale;
    GXColor colorArg;
    GXColor color;
    EffLasMonData* data;

    data = ((EffectEntry*)param)->userdata;

    if (data->animHandle != -1) {
        PSMTXTrans(mtxTrans, data->x, data->y, data->z);
        PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
        PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
        animPoseSetMaterialFlagOn(data->animHandle, 0x40);
        *(s32*)&color = *(s32*)&dat_80422408;
        color.a = (u8)data->alpha;
        *(s32*)&colorArg = *(s32*)&color;
        animPoseSetMaterialEvtColor(data->animHandle, &colorArg);
        animPoseMain(data->animHandle);
        animPoseDrawMtx(data->animHandle, mtxTrans, 1, 0.0f, 10.0f);
        animPoseDrawMtx(data->animHandle, mtxTrans, 2, 0.0f, 10.0f);
        animPoseDrawMtx(data->animHandle, mtxTrans, 3, 0.0f, 10.0f);
    }
}
