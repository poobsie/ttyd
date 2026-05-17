/* Syuryou effect */

#include "eff/eff_syuryou.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <dolphin/mtx.h>

extern GlobalWork* gp;

typedef struct EffSyuryouData {
    /* 0x00 */ s32 type;
    /* 0x04 */ f32 x;
    /* 0x08 */ f32 y;
    /* 0x0C */ f32 z;
    /* 0x10 */ f32 scale;
    /* 0x14 */ s32 animHandle;
    /* 0x18 */ s32 alpha;
    /* 0x1C */ s32 timer;
    /* 0x20 */ s32 unk20;
} EffSyuryouData; /* size 0x24 */

static const GXColor dat_804222f8 = {0xFF, 0xFF, 0xFF, 0x00};
static const Vec vec3_80301358 = {0.0f, 0.0f, 0.0f};
static const char kGroup0Name[] = "EFF_batug_tue";
static const char kGroup1Name[] = "MOBJ_EFF_baria";
static const char kEffTypeName[] = "Syuryou";

void effSyuryouMain(EffectEntry* effect);
void effSyuryouDisp(CameraId camId, void* param);
void animPoseDrawMtx(s32 poseId, Mtx matrix, s32 xluMode, f32 rotY, f32 scale);

EffectEntry* effSyuryouEntry(s32 type, s32 duration, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    EffSyuryouData* data;

    effect = effEntry();
    effect->type = kEffTypeName;
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(EffSyuryouData) * effect->count);
    effect->userdata = data;
    effect->callback = effSyuryouMain;
    effect->flags |= 2;

    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = 1.0f;
    data->alpha = 0xFF;
    if (duration <= 0) {
        data->timer = 0x3E8;
    } else {
        data->timer = duration;
    }
    data->alpha = 0xFF;
    data->animHandle = -1;

    return effect;
}

void effSyuryouMain(EffectEntry* effect) {
    Vec pos;
    Vec tempPos;
    EffSyuryouData* data;
    s32 isInBattle;

    data = effect->userdata;
    tempPos = vec3_80301358;
    tempPos.x = data->x;
    tempPos.y = data->y;
    tempPos.z = data->z;
    pos = tempPos;
    isInBattle = (s32)((u32)((-gp->inBattle) | gp->inBattle) >> 31);

    if (data->type != 1) {
        if (data->type < 1) {
            if (data->type == 0) {
                if (!animGroupBaseAsync(kGroup0Name, isInBattle, NULL)) return;
                if (data->animHandle == -1) {
                    data->animHandle = animPoseEntry(kGroup0Name, isInBattle);
                    animPoseSetAnim(data->animHandle, "A_1", 1);
                }
                if (animPoseGetLoopTimes(data->animHandle) >= 1.0f) {
                    animPoseRelease(data->animHandle);
                    effDelete(effect);
                    return;
                }
            }
        }
    } else {
        if (!animGroupBaseAsync(kGroup1Name, isInBattle, NULL)) return;
        if (data->animHandle == -1) {
            data->animHandle = animPoseEntry(kGroup1Name, isInBattle);
            animPoseSetAnim(data->animHandle, "A_1", 1);
        }
        if (animPoseGetLoopTimes(data->animHandle) >= 1.0f) {
            animPoseSetAnim(data->animHandle, "S_1", 0);
        }
    }

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->timer = 0x20;
    }
    if (data->timer < 0x3E8) {
        data->timer--;
    }
    if (data->timer < 0x20) {
        data->alpha = data->timer * 8;
    }
    if (data->timer < 0) {
        if (data->animHandle != -1) {
            animPoseRelease(data->animHandle);
        }
        effDelete(effect);
        return;
    }
    dispEntry(4, 2, effSyuryouDisp, effect, dispCalcZ(&pos));
}

void effSyuryouDisp(CameraId camId, void* param) {
    Mtx mtxTrans;
    Mtx mtxScale;
    Mtx mtxRot;
    GXColor colorArg;
    GXColor color;
    EffSyuryouData* data;

    data = ((EffectEntry*)param)->userdata;

    if (data->animHandle != -1) {
        PSMTXTrans(mtxTrans, data->x, data->y, data->z);
        PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
        PSMTXRotRad(mtxRot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
        PSMTXConcat(mtxTrans, mtxRot, mtxTrans);
        PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
        animPoseSetMaterialFlagOn(data->animHandle, 0x40);
        *(s32*)&color = *(s32*)&dat_804222f8;
        color.a = (u8)data->alpha;
        *(s32*)&colorArg = *(s32*)&color;
        animPoseSetMaterialEvtColor(data->animHandle, &colorArg);
        animPoseMain(data->animHandle);
        animPoseDrawMtx(data->animHandle, mtxTrans, 1, 0.0f, 10.0f);
        animPoseDrawMtx(data->animHandle, mtxTrans, 2, 0.0f, 10.0f);
        animPoseDrawMtx(data->animHandle, mtxTrans, 3, 0.0f, 10.0f);
    }
}
