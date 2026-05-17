/* Queen effect */

#include "eff/eff_queen.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/mtx.h>

extern GlobalWork* gp;

typedef struct EffQueenData {
    /* 0x00 */ s32 type;
    /* 0x04 */ f32 x;
    /* 0x08 */ f32 y;
    /* 0x0C */ f32 z;
    /* 0x10 */ f32 scaleX;
    /* 0x14 */ f32 scaleY;
    /* 0x18 */ s32 animHandle;
    /* 0x1C */ s32 unk1c;
    /* 0x20 */ s32 timer;
    /* 0x24 */ s32 alpha;
    /* 0x28 */ s32 maxAlpha;
} EffQueenData; /* size 0x2C */

static const GXColor dat_804224a8 = {0xFF, 0xFF, 0xFF, 0x00};
static const Vec vec3_803014c8 = {0.0f, 0.0f, 0.0f};

void effQueenMain(EffectEntry* effect);
void effQueenDisp(CameraId camId, void* param);
void animPoseDrawMtx(s32 poseId, Mtx matrix, s32 xluMode, f32 rotY, f32 scale);

EffectEntry* effQueenEntry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    EffQueenData* data;

    effect = effEntry();
    effect->type = "Queen";
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(EffQueenData) * effect->count);
    effect->userdata = data;
    effect->callback = effQueenMain;
    effect->flags |= 2;

    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scaleY = scale;
    data->scaleX = 0.0f;
    if (duration <= 0) {
        data->timer = 0x3E8;
    } else {
        data->timer = duration;
    }
    data->unk1c = 0;
    data->animHandle = -1;
    data->alpha = 0;
    data->maxAlpha = 0xFF;
    data->scaleX = 1.0f;

    return effect;
}

void effQueenMain(EffectEntry* effect) {
    Vec pos;
    Vec tempPos;
    EffQueenData* data;
    s32 isInBattle;

    data = effect->userdata;
    *(s32*)&tempPos.x = *(s32*)&vec3_803014c8.x;
    *(s32*)&tempPos.y = *(s32*)&vec3_803014c8.y;
    *(s32*)&tempPos.z = *(s32*)&vec3_803014c8.z;
    tempPos.x = data->x;
    tempPos.y = data->y;
    tempPos.z = data->z;
    *(s32*)&pos.x = *(s32*)&tempPos.x;
    *(s32*)&pos.y = *(s32*)&tempPos.y;
    *(s32*)&pos.z = *(s32*)&tempPos.z;
    isInBattle = (s32)((u32)((-gp->inBattle) | gp->inBattle) >> 31);

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->timer = 0x64;
    }

    if (animGroupBaseAsync("MOBJ_EFF_queen_tornade", isInBattle, NULL)) {
        if (data->animHandle == -1) {
            data->animHandle = animPoseEntry("MOBJ_EFF_queen_tornade", isInBattle);
            animPoseSetAnim(data->animHandle, "S_1", 1);
        }
        if (data->timer < 0x3E8) {
            data->timer--;
        }
        if (data->timer < 0) {
            if (data->animHandle != -1) {
                animPoseRelease(data->animHandle);
            }
            effDelete(effect);
            return;
        }
        if (data->timer < 0x64) {
            data->alpha -= 3;
        } else {
            data->alpha += 3;
        }
        if (data->alpha > (s32)(0.8f * (f32)data->maxAlpha)) {
            data->alpha = (s32)(0.8f * (f32)data->maxAlpha);
        }
        if (data->alpha < 0) {
            data->alpha = 0;
        }
        dispEntry(4, 2, effQueenDisp, effect, dispCalcZ(&pos));
    }
}

void effQueenDisp(CameraId camId, void* param) {
    Mtx mtxTrans;
    Mtx mtxScale;
    Mtx mtxRot;
    GXColor colorArg;
    GXColor color;
    EffQueenData* data;
    f32 sx;

    data = ((EffectEntry*)param)->userdata;

    if (data->animHandle != -1) {
        PSMTXTrans(mtxTrans, data->x, data->y, data->z);
        sx = data->scaleX * data->scaleY;
        PSMTXScale(mtxScale, sx, data->scaleY, sx);
        PSMTXRotRad(mtxRot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
        PSMTXConcat(mtxTrans, mtxRot, mtxTrans);
        PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
        animPoseSetMaterialFlagOn(data->animHandle, 0x40);
        *(s32*)&color = *(s32*)&dat_804224a8;
        color.a = (u8)data->alpha;
        *(s32*)&colorArg = *(s32*)&color;
        animPoseSetMaterialEvtColor(data->animHandle, &colorArg);
        animPoseMain(data->animHandle);
        animPoseDrawMtx(data->animHandle, mtxTrans, 1, 0.0f, 10.0f);
        animPoseDrawMtx(data->animHandle, mtxTrans, 2, 0.0f, 10.0f);
        animPoseDrawMtx(data->animHandle, mtxTrans, 3, 0.0f, 10.0f);
    }
}
