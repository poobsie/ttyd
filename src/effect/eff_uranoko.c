/* Uranoko effect */

#include "eff/eff_uranoko.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/mtx.h>

extern GlobalWork* gp;

typedef struct EffUranokoData {
    /* 0x00 */ s32 type;
    /* 0x04 */ f32 x;
    /* 0x08 */ f32 y;
    /* 0x0C */ f32 z;
    /* 0x10 */ f32 scale;
    /* 0x14 */ f32 targetScale;
    /* 0x18 */ s32 counter;
    /* 0x1C */ s32 life;
    /* 0x20 */ s32 animHandle;
    /* 0x24 */ s32 alpha;
} EffUranokoData; /* size 0x28 */

static const GXColor dat_80422318 = {0xFF, 0xFF, 0xFF, 0x00};
static const Vec vec3_80301390 = {0.0f, 0.0f, 0.0f};

void effUranokoMain(EffectEntry* effect);
void effUranokoDisp(CameraId camId, void* param);
void animPoseDrawMtx(s32 poseId, Mtx matrix, s32 xluMode, f32 rotY, f32 scale);

EffectEntry* effUranokoEntry(s32 type, s32 arg1, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    EffUranokoData* data;

    effect = effEntry();
    effect->type = "Uranoko";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(EffUranokoData) * effect->count);
    effect->userdata = data;
    effect->callback = effUranokoMain;
    effect->flags |= 2;

    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->targetScale = 1.0f;
    data->scale = 0.0f;
    data->counter = 0;
    if (arg1 <= 0) {
        data->life = 0x3E8;
    } else {
        data->life = arg1;
    }
    data->alpha = 0xFF;
    data->animHandle = -1;

    return effect;
}

void effUranokoMain(EffectEntry* effect) {
    Vec pos;
    Vec tempPos;
    EffUranokoData* data;
    s32 isInBattle;

    data = effect->userdata;
    *(s32*)&tempPos.x = *(s32*)&vec3_80301390.x;
    *(s32*)&tempPos.y = *(s32*)&vec3_80301390.y;
    *(s32*)&tempPos.z = *(s32*)&vec3_80301390.z;
    tempPos.x = data->x;
    tempPos.y = data->y;
    tempPos.z = data->z;
    *(s32*)&pos.x = *(s32*)&tempPos.x;
    *(s32*)&pos.y = *(s32*)&tempPos.y;
    *(s32*)&pos.z = *(s32*)&tempPos.z;
    isInBattle = (s32)((u32)((-gp->inBattle) | gp->inBattle) >> 31);

    if (!animGroupBaseAsync("MOBJ_EFF_uranoko", isInBattle, NULL)) {
        return;
    }

    if (data->animHandle == -1) {
        data->animHandle = animPoseEntry("MOBJ_EFF_uranoko", isInBattle);
        animPoseSetAnim(data->animHandle, "A_1", 1);
    }

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->life = 0x10;
    }

    if (data->life < 0x3E8) {
        data->life--;
    }

    if (data->life < 0x10) {
        data->alpha = data->life << 4;
    }

    if (data->life < 0) {
        if (data->animHandle != -1) {
            animPoseRelease(data->animHandle);
        }
        effDelete(effect);
        return;
    }

    data->counter++;
    data->scale = (1.0f - data->scale) * 0.125f + data->scale;
    dispEntry(4, 2, effUranokoDisp, effect, dispCalcZ(&pos));
}

void effUranokoDisp(CameraId camId, void* param) {
    Mtx mtxTrans;
    Mtx mtxScale;
    Mtx mtxRot;
    GXColor colorArg;
    GXColor color;
    EffUranokoData* data;

    data = ((EffectEntry*)param)->userdata;

    if (data->animHandle != -1) {
        PSMTXTrans(mtxTrans, data->x, data->y, data->z);
        PSMTXScale(mtxScale, data->scale * data->targetScale, data->scale * data->targetScale, data->scale * data->targetScale);
        PSMTXRotRad(mtxRot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
        PSMTXConcat(mtxTrans, mtxRot, mtxTrans);
        PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
        animPoseSetMaterialFlagOn(data->animHandle, 0x40);
        *(s32*)&color = *(s32*)&dat_80422318;
        color.a = (u8)data->alpha;
        *(s32*)&colorArg = *(s32*)&color;
        animPoseSetMaterialEvtColor(data->animHandle, &colorArg);
        animPoseMain(data->animHandle);
        animPoseDrawMtx(data->animHandle, mtxTrans, 1, 0.0f, 6.0f);
        animPoseDrawMtx(data->animHandle, mtxTrans, 2, 0.0f, 6.0f);
        animPoseDrawMtx(data->animHandle, mtxTrans, 3, 0.0f, 6.0f);
    }
}
