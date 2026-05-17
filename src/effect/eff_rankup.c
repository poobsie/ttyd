/* Rankup effect */

#include "eff/eff_rankup.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/mtx.h>

extern GlobalWork* gp;

typedef struct EffRankupData {
    /* 0x00 */ s32 type;
    /* 0x04 */ f32 x;
    /* 0x08 */ f32 y;
    /* 0x0C */ f32 z;
    /* 0x10 */ f32 scale;
    /* 0x14 */ s32 animHandle;
    /* 0x18 */ s32 animState;
    /* 0x1C */ s32 unk1C;
    /* 0x20 */ s32 alpha;
} EffRankupData; /* size 0x24 */

static const GXColor dat_80421be0 = {0xFF, 0xFF, 0xFF, 0x00};
static const Vec vec3_80300170 = {0.0f, 0.0f, 0.0f};

void effRankupMain(EffectEntry* effect);
void effRankupDisp(CameraId camId, void* param);
void animPoseDrawMtx(s32 poseId, Mtx matrix, s32 xluMode, f32 rotY, f32 scale);

EffectEntry* effRankupEntry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    EffRankupData* data;

    effect = effEntry();
    effect->type = "Rankup";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(EffRankupData) * effect->count);
    effect->userdata = data;
    effect->callback = effRankupMain;
    effect->flags |= 2;

    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = 1.0f;
    data->animHandle = -1;
    data->animState = 0;
    data->unk1C = 0;
    data->alpha = 0xFF;

    return effect;
}

void effRankupMain(EffectEntry* effect) {
    Vec pos;
    Vec tempPos;
    EffRankupData* data;
    s32 isInBattle;

    data = effect->userdata;
    *(s32*)&tempPos.x = *(s32*)&vec3_80300170.x;
    *(s32*)&tempPos.y = *(s32*)&vec3_80300170.y;
    *(s32*)&tempPos.z = *(s32*)&vec3_80300170.z;
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

    switch (data->animState) {
    case 0:
        if (!animGroupBaseAsync("EFF_TekiKyouka", isInBattle, NULL)) {
            return;
        }
        data->animHandle = animPoseEntry("EFF_TekiKyouka", isInBattle);
        animPoseSetAnim(data->animHandle, "Z_1", 1);
        data->animState++;
        break;
    case 1:
        if (animPoseGetLoopTimes(data->animHandle) > 1.0f) {
            animPoseRelease(data->animHandle);
            effDelete(effect);
            return;
        }
        break;
    }

    if (!(animPoseGetLoopTimes(data->animHandle) < 0.61538462f)) {
        data->alpha -= 0x14;
        if (data->alpha < 0) {
            data->alpha = 0;
        }
    }

    dispEntry(4, 2, effRankupDisp, effect, dispCalcZ(&pos));
}

void effRankupDisp(CameraId camId, void* param) {
    Mtx mtxTrans;
    Mtx mtxScale;
    Mtx mtxRot;
    GXColor colorArg;
    GXColor color;
    EffRankupData* data;

    data = ((EffectEntry*)param)->userdata;

    if (data->animHandle != -1) {
        PSMTXTrans(mtxTrans, data->x, data->y, data->z);
        PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
        PSMTXRotRad(mtxRot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
        PSMTXConcat(mtxTrans, mtxRot, mtxTrans);
        PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
        animPoseSetMaterialFlagOn(data->animHandle, 0x40);
        *(s32*)&color = *(s32*)&dat_80421be0;
        color.a = (u8)data->alpha;
        *(s32*)&colorArg = *(s32*)&color;
        animPoseSetMaterialEvtColor(data->animHandle, &colorArg);
        animPoseMain(data->animHandle);
        animPoseDrawMtx(data->animHandle, mtxTrans, 1, 0.0f, 6.0f);
        animPoseDrawMtx(data->animHandle, mtxTrans, 2, 0.0f, 6.0f);
        animPoseDrawMtx(data->animHandle, mtxTrans, 3, 0.0f, 6.0f);
    }
}
