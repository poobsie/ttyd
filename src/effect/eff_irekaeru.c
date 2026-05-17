/* Irekaeru effect */

#include "eff/eff_irekaeru.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/mtx.h>

extern GlobalWork* gp;

typedef struct EffIrekaeruData {
    /* 0x00 */ s32 type;
    /* 0x04 */ f32 x;
    /* 0x08 */ f32 y;
    /* 0x0C */ f32 z;
    /* 0x10 */ f32 scale;
    /* 0x14 */ s32 animHandle;
    /* 0x18 */ s32 state;
    /* 0x1C */ s32 unk1c;
    /* 0x20 */ s32 alpha;
} EffIrekaeruData; /* size 0x24 */

static const GXColor dat_80421c00 = {0xFF, 0xFF, 0xFF, 0x00};
static const Vec vec3_80300198 = {0.0f, 0.0f, 0.0f};

void effIrekaeruMain(EffectEntry* effect);
void effIrekaeruDisp(CameraId camId, void* param);
void animPoseDrawMtx(s32 poseId, Mtx matrix, s32 xluMode, f32 rotY, f32 scale);

EffectEntry* effIrekaeruEntry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    EffIrekaeruData* data;

    effect = effEntry();
    effect->type = "Irekaeru";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(EffIrekaeruData) * effect->count);
    effect->userdata = data;
    effect->callback = effIrekaeruMain;
    effect->flags |= 2;

    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = 1.0f;
    data->animHandle = -1;
    data->state = 0;
    data->unk1c = 0;
    data->alpha = 0;

    return effect;
}

void effIrekaeruMain(EffectEntry* effect) {
    Vec pos;
    Vec tempPos;
    EffIrekaeruData* data;
    s32 isInBattle;

    data = effect->userdata;
    *(s32*)&tempPos.x = *(s32*)&vec3_80300198.x;
    *(s32*)&tempPos.y = *(s32*)&vec3_80300198.y;
    *(s32*)&tempPos.z = *(s32*)&vec3_80300198.z;
    tempPos.x = data->x;
    tempPos.y = data->y;
    tempPos.z = data->z;
    *(s32*)&pos.x = *(s32*)&tempPos.x;
    *(s32*)&pos.y = *(s32*)&tempPos.y;
    *(s32*)&pos.z = *(s32*)&tempPos.z;
    isInBattle = (s32)((u32)((-gp->inBattle) | gp->inBattle) >> 31);

    if (effect->flags & 4) {
        effect->flags &= ~4;
        if (data->animHandle != -1) {
            animPoseRelease(data->animHandle);
        }
        effDelete(effect);
        return;
    }

    switch (data->state) {
    case 0:
        if (!animGroupBaseAsync("EFF_Irekae", isInBattle, NULL)) return;
        data->animHandle = animPoseEntry("EFF_Irekae", isInBattle);
        animPoseSetAnim(data->animHandle, "Z_1", 1);
        data->state++;
        break;
    case 1:
        if (animPoseGetLoopTimes(data->animHandle) > 1.0f) {
            animPoseRelease(data->animHandle);
            effDelete(effect);
            return;
        }
        break;
    }

    if (animPoseGetLoopTimes(data->animHandle) < 0.75f) {
        data->alpha += 0x14;
        if (data->alpha > 0xFF) data->alpha = 0xFF;
    } else {
        data->alpha -= 0x14;
        if (data->alpha < 0) data->alpha = 0;
    }
    dispEntry(4, 2, effIrekaeruDisp, effect, dispCalcZ(&pos));
}

void effIrekaeruDisp(CameraId camId, void* param) {
    Mtx mtxTrans;
    Mtx mtxScale;
    Mtx mtxRot;
    GXColor colorArg;
    GXColor color;
    EffIrekaeruData* data;

    data = ((EffectEntry*)param)->userdata;

    if (data->animHandle != -1) {
        PSMTXTrans(mtxTrans, data->x, data->y, data->z);
        PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
        PSMTXRotRad(mtxRot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
        PSMTXConcat(mtxTrans, mtxRot, mtxTrans);
        PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
        animPoseSetMaterialFlagOn(data->animHandle, 0x40);
        *(s32*)&color = *(s32*)&dat_80421c00;
        color.a = (u8)data->alpha;
        *(s32*)&colorArg = *(s32*)&color;
        animPoseSetMaterialEvtColor(data->animHandle, &colorArg);
        animPoseMain(data->animHandle);
        animPoseDrawMtx(data->animHandle, mtxTrans, 1, 0.0f, 2.0f);
        animPoseDrawMtx(data->animHandle, mtxTrans, 2, 0.0f, 2.0f);
        animPoseDrawMtx(data->animHandle, mtxTrans, 3, 0.0f, 2.0f);
    }
}
