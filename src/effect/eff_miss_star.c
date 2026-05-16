#include "eff/eff_miss_star.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

typedef struct EffMissStarStar {
    /* 0x00 */ f32 x;
    /* 0x04 */ f32 y;
    /* 0x08 */ f32 z;
    /* 0x0C */ f32 velHX;
    /* 0x10 */ f32 velHY;
    /* 0x14 */ f32 velHZ;
    /* 0x18 */ f32 velX;
    /* 0x1C */ f32 velY;
    /* 0x20 */ f32 velZ;
    /* 0x24 */ s32 timer;
    /* 0x28 */ s32 delay;
    /* 0x2C */ f32 angle;
    /* 0x30 */ f32 angleVel;
    /* 0x34 */ f32 camAngle;
    /* 0x38 */ f32 scale;
    /* 0x3C */ s32 life;
    /* 0x40 */ f32 alpha;
} EffMissStarStar;

void effMissStarMain(EffectEntry* effect);
void effMissStarDisp(CameraId camId, void* param);

/* Defined before functions so MWCC can deduplicate anonymous literals */
const Vec vec3_802f3678 = { 0.0f, 0.0f, 0.0f };
const char str_SmallStar[] = "SmallStar";



Vec spd_tbl[15] = {
    { 0.0f,  4.5f, 0.0f },
    { 1.0f,  4.0f, 0.0f },
    { 2.0f,  3.0f, 0.0f },
    { 3.0f,  2.0f, 0.0f },
    { 3.5f,  1.0f, 0.0f },
    { 4.0f,  0.0f, 0.0f },
    { 4.5f,  0.0f, 0.0f },
    { 5.0f,  0.0f, 0.0f },
    { 4.5f,  0.0f, 0.0f },
    { 4.0f,  0.0f, 0.0f },
    { 3.5f, -1.0f, 0.0f },
    { 3.0f, -2.0f, 0.0f },
    { 2.0f, -3.0f, 0.0f },
    { 1.0f, -4.0f, 0.0f },
    { 0.0f, -4.5f, 0.0f },
};

Vec scale_tbl[15] = {
    { 1.0f, 1.0f, 1.0f },
    { 0.8f, 0.8f, 0.8f },
    { 0.9f, 0.9f, 0.9f },
    { 1.1f, 1.1f, 1.1f },
    { 1.0f, 1.0f, 1.0f },
    { 0.8f, 0.8f, 0.8f },
    { 0.9f, 0.9f, 0.9f },
    { 1.1f, 1.1f, 1.1f },
    { 1.0f, 1.0f, 1.0f },
    { 0.8f, 0.8f, 0.8f },
    { 0.9f, 0.9f, 0.9f },
    { 1.1f, 1.1f, 1.1f },
    { 1.0f, 1.0f, 1.0f },
    { 0.8f, 0.8f, 0.8f },
    { 0.9f, 0.9f, 0.9f },
};

EffectEntry* effMissStarEntry(s32 count, s32 delay, s32 dir, f32 x, f32 y, f32 z) {
    EffectEntry* entry;
    EffMissStarStar* star;
    s32 i;
    f32 sizeFactor;
    f32 scaleFactor;
    s32 sign;

    entry = effEntry();
    sizeFactor = 0.8f;
    scaleFactor = 1.0f;
    if (count <= 0) {
        sizeFactor = 0.4f;
        count = 1;
        scaleFactor = 0.7f;
    }
    sign = 1;
    if (dir >= 0) goto wh;
    dir = -dir;
    sign = -1;
wh:
    if (dir > 5) {
        dir -= 5;
        goto wh;
    }
    entry->type = str_SmallStar;
    entry->count = count;
    star = (EffMissStarStar*)__memAlloc(HEAP_EFFECT, count * sizeof(EffMissStarStar));
    entry->userdata = star;
    entry->callback = effMissStarMain;
    for (i = 0; i < count; i++) {
        s32 spdIdx;
        spdIdx = dir % 8;
        dir++;
        star->x = x;
        star->y = y;
        star->z = z;
        star->velX = scaleFactor * ((f32)sign * (2.0f * spd_tbl[spdIdx].x));
        star->velY = scaleFactor * (2.0f * spd_tbl[spdIdx].y);
        star->velZ = scaleFactor * (2.0f * spd_tbl[spdIdx].z);
        star->velHX = scaleFactor * ((f32)sign * spd_tbl[spdIdx].x);
        star->velHY = scaleFactor * spd_tbl[spdIdx].y;
        star->velHZ = scaleFactor * spd_tbl[spdIdx].z;
        star->scale = sizeFactor * scale_tbl[i % 8].x;
        star->angle = 0.0f;
        star->angleVel = (f32)(sign * 107);
        star->camAngle = -camGetPtr(CAMERA_3D)->viewYaw;
        star->timer = 14;
        star->delay = delay;
        star->life = 10;
        star->alpha = 255.0f;
        star++;
    }
    return entry;
}

void effMissStarMain(EffectEntry* effect) {
    EffMissStarStar* data;
    s32 i;
    Vec tmp;
    Vec pos = vec3_802f3678;

    data = (EffMissStarStar*)effect->userdata;
    i = 0;
    pos.x = data->x;
    pos.y = data->y;
    pos.z = data->z;
    tmp = pos;

    while (i < effect->count) {
        s32 delay;
        delay = data->delay;
        if (delay != 0) {
            data->delay = delay - 1;
            if (data->delay == 0) {
                break;
            }
        }
        if (data->timer >= 0) {
            data->x += data->velX;
            data->y += data->velY;
            data->z += data->velZ;
        }
        data->camAngle = -camGetPtr(CAMERA_3D)->viewYaw;
        data->angle += data->angleVel;
        data->angle = reviseAngle(data->angle);
        data->angleVel *= 0.8f;
        if (data->timer < 10) {
            data->velHX *= 0.5f;
            data->velHY *= 0.5f;
            data->velHZ *= 0.5f;
            data->velX = data->velHX;
            data->velY = data->velHY;
            data->velZ = data->velHZ;
        }
        data->timer--;
        if (data->timer < 0) {
            data->life--;
            if (data->life < 0) {
                effDelete(effect);
                return;
            }
        }
        i++;
        data++;
    }
    dispEntry(CAMERA_3D, 1, effMissStarDisp, effect, dispCalcZ(&tmp));
}

void effMissStarDisp(CameraId camId, void* param) {
    GXTexObj texObj;
    Mtx mtxTrans;
    Mtx mtxScale;
    Mtx mtxY;
    Mtx mtxZ;
    EffectEntry* entry;
    EffMissStarStar* star;
    s32 i;
    f32 deg2rad;

    entry = (EffectEntry*)param;
    star = (EffMissStarStar*)entry->userdata;
    effGetTexObj(0x18, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_TEX_ST, GX_RGBA6, 0);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_RGBA6, 0);
    deg2rad = 0.017453292f;
    i = 0;
    while (i < entry->count) {
        if (star->delay != 0) {
            break;
        }
        PSMTXTrans(mtxTrans, star->x, star->y, star->z);
        PSMTXRotRad(mtxY, 'y', deg2rad * -camGetPtr(CAMERA_3D)->viewYaw);
        PSMTXRotRad(mtxZ, 'z', deg2rad * star->angle);
        PSMTXScale(mtxScale, star->scale, star->scale, star->scale);
        PSMTXConcat(mtxY, mtxTrans, mtxTrans);
        PSMTXConcat(mtxTrans, mtxZ, mtxTrans);
        PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
        PSMTXConcat(camGetPtr(CAMERA_3D)->view, mtxTrans, mtxTrans);
        GXLoadPosMtxImm(mtxTrans, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        i++;
        star++;
        GXPosition3f32(-16.0f, 16.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(16.0f, 16.0f, 0.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(16.0f, -16.0f, 0.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(-16.0f, -16.0f, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    }
}
