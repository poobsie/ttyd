/* Spark effect */

#include "eff/eff_spark.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>

typedef struct EffSparkSub {
    /* 0x00 */ s32 unused;
    /* 0x04 */ f32 x;
    /* 0x08 */ f32 y;
    /* 0x0C */ f32 z;
    /* 0x10 */ f32 velX;
    /* 0x14 */ f32 velY;
    /* 0x18 */ f32 accelX;
    /* 0x1C */ f32 accelY;
    /* 0x20 */ s32 seq;
    /* 0x24 */ f32 angle;
    /* 0x28 */ s32 unk28;
    /* 0x2C */ s32 unk2C;
    /* 0x30 */ s32 alpha;
    /* 0x34 */ f32 scale;
} EffSparkSub; /* size = 0x38 */

typedef struct EffSparkData {
    /* 0x00 */ s32 type;
    /* 0x04 */ f32 posX;
    /* 0x08 */ f32 posY;
    /* 0x0C */ f32 posZ;
    /* 0x10 */ u8 unk10[0x18];
    /* 0x28 */ s32 lifetime;
    /* 0x2C */ s32 frame;
    /* 0x30 */ s32 alpha;
    /* 0x34 */ f32 scale;
    /* 0x38 */ EffSparkSub sparks[5];
} EffSparkData; /* size = 0x150 */

static const Vec vec3_802f8e20 = {0.0f, 0.0f, 0.0f};
static const f64 double_to_int = 4503601774854144.0;

static s32 max_seq_num[2] = {0x18, 0x0C};

static u8 a_data[24] = {
    0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

static f32 scale_data2[12] = {
    0.9f, 1.0f, 1.0f, 0.9f, 0.8f, 0.7f,
    0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f,
};

static u8 a_data2[12] = {
    0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

static const GXColor dat_8041e270 = {0xFF, 0xFF, 0xFF, 0x00};

static f32 scale_data[24] = {
    0.9f, 1.0f, 1.0f, 0.9f, 0.85f, 0.8f, 0.75f, 0.7f,
    0.65f, 0.6f, 0.55f, 0.5f, 0.45f, 0.4f, 0.35f, 0.3f,
    0.25f, 0.2f, 0.15f, 0.1f, 0.05f, 0.03f, 0.02f, 0.01f,
};

void effSparkMain(EffectEntry* effect);
void effSparkDisp(CameraId camId, void* param);

EffectEntry* effSparkEntry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    EffSparkData* data;
    f32 halfSeq;

    effect = effEntry();
    effect->type = "Spark";
    effect->count = 6;
    data = (EffSparkData*)__memAlloc(HEAP_EFFECT, sizeof(EffSparkData));
    effect->userdata = data;
    effect->callback = effSparkMain;
    effect->flags |= 2;

    data->type = type;
    data->frame = 0;
    data->lifetime = 0x3E8;
    data->alpha = 0xFF;
    data->posX = x;
    data->posY = y;
    data->posZ = z;
    data->scale = 0.9f;

    halfSeq = (f32)max_seq_num[type] * 0.5f;

    data->sparks[0].seq = (s32)-((halfSeq * (f32) 1) - -1.0f);
    data->sparks[1].seq = (s32)-((halfSeq * (f32) 0) - -1.0f);
    data->sparks[2].seq = (s32)-((halfSeq * (f32) 1) - -1.0f);
    data->sparks[3].seq = (s32)-((halfSeq * (f32) 0) - -1.0f);
    data->sparks[4].seq = (s32)-((halfSeq * (f32) 1) - -1.0f);

    return effect;
}

void effSparkMain(EffectEntry* effect) {
    Vec pos;
    Vec tempPos;
    s32 oddParity;
    EffSparkData* data;
    EffSparkSub* spark;
    s32 i;
    s32 type;
    u8 alphaU8;
    f32 velScale;
    f32 lerpScale;
    f32 rad;
    f32 angleStep;
    s32 renderMode;

    data = effect->userdata;

    *(s32*)&tempPos.x = *(s32*)&vec3_802f8e20.x;
    *(s32*)&tempPos.y = *(s32*)&vec3_802f8e20.y;
    *(s32*)&tempPos.z = *(s32*)&vec3_802f8e20.z;
    tempPos.x = data->posX;
    tempPos.y = data->posY;
    tempPos.z = data->posZ;
    *(s32*)&pos.x = *(s32*)&tempPos.x;
    *(s32*)&pos.y = *(s32*)&tempPos.y;
    *(s32*)&pos.z = *(s32*)&tempPos.z;

    type = data->type;
    alphaU8 = (u8)data->alpha;

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->lifetime = 0x10;
    }
    if (data->lifetime < 0x3E8) {
        data->lifetime--;
    }
    if (data->lifetime < 0) {
        effDelete(effect);
        return;
    }
    if (data->lifetime < 0x10) {
        data->alpha = data->lifetime << 4;
    }

    data->frame++;

    if (type != 0) {
        velScale = 0.2f;
        lerpScale = velScale;
    } else {
        velScale = 0.1f;
        lerpScale = 0.2f;
    }

    oddParity = data->frame & 1;
    spark = &data->sparks[0];

    i = 1;
    while (i < effect->count) {
        spark->seq++;
        if (spark->seq >= max_seq_num[type]) {
            spark->seq = 0;
        }
        if (spark->seq >= 0) {
            if (spark->seq == 0) {
                angleStep = 360.0f * (f32)i / (f32)(effect->count - 1);
                rad = 6.2831855f * angleStep / 360.0f;
                spark->x = 16.0f * (f32)sin(rad);
                spark->y = 16.0f * (f32)cos(rad);
                spark->velX = spark->x * velScale;
                spark->velY = spark->y * velScale;
                spark->accelX = 0.0f;
                spark->accelY = 0.0f;
                spark->angle = -angleStep;
                spark->scale = 1.0f;
            }
            if (type == 0) {
                spark->alpha = (s32)a_data[spark->seq];
                spark->scale = scale_data[spark->seq];
            } else {
                spark->alpha = (s32)a_data2[spark->seq];
                spark->scale = scale_data2[spark->seq];
            }
            if (oddParity == (i & 1)) {
                spark->x += spark->velX;
                spark->y += spark->velY;
            } else {
                spark->x += 0.2f * spark->velX;
                spark->y += 0.2f * spark->velY;
            }
            spark->velX += lerpScale * (spark->accelX - spark->velX);
            spark->velY += lerpScale * (spark->accelY - spark->velY);
        }
        i++;
        spark++;
    }

    renderMode = 1;
    if (alphaU8 != 0xFF) {
        renderMode = 2;
    }
    dispEntry(CAMERA_3D, renderMode, effSparkDisp, effect, dispCalcZ(&pos));
}

void effSparkDisp(CameraId camId, void* param) {
    Mtx mtxFinal;
    Mtx mtxTrans;
    Mtx mtxRot;
    Mtx mtxScale;
    Mtx mtxLocal;
    GXTexObj texObj;
    GXColor spC;
    GXColor sp8;
    EffSparkData* data;
    s32 mainAlpha;
    s32 i;
    EffSparkSub* spark;
    CameraEntry* camera;
    f32 neg16;

    data = ((EffectEntry*)param)->userdata;
    camera = camGetPtr(camId);
    mainAlpha = data->alpha;

    effGetTexObj(0x1C, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, FALSE, GX_PTIDENTITY);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    PSMTXTrans(mtxTrans, data->posX, data->posY, data->posZ);
    PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
    PSMTXRotRad(mtxRot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
    PSMTXConcat(mtxTrans, mtxRot, mtxLocal);
    PSMTXConcat(mtxLocal, mtxScale, mtxLocal);

    neg16 = -16.0f;
    i = 1;
    spark = &data->sparks[0];
    while (i < ((EffectEntry*)param)->count) {
        if (spark->seq >= 0) {
            PSMTXRotRad(mtxRot, 'z', 0.017453292f * spark->angle);
            PSMTXScale(mtxScale, spark->scale, spark->scale, spark->scale);
            PSMTXTrans(mtxTrans, spark->x, spark->y, 0.0f);
            PSMTXConcat(mtxTrans, mtxRot, mtxFinal);
            PSMTXConcat(mtxFinal, mtxScale, mtxFinal);
            PSMTXConcat(mtxLocal, mtxFinal, mtxFinal);
            PSMTXConcat(camera->view, mtxFinal, mtxFinal);
            GXLoadPosMtxImm(mtxFinal, 0);
            GXSetCurrentMtx(0);

            sp8 = dat_8041e270;
            sp8.a = (u8)((mainAlpha * spark->alpha) >> 8);
            spC = sp8;
            GXSetChanMatColor(GX_COLOR0A0, spC);

            GXBegin(GX_QUADS, GX_VTXFMT0, 4);
            *(f32*)0xCC008000 = neg16 * 0.5f;
            *(f32*)0xCC008000 = 8.0f;
            *(f32*)0xCC008000 = 0.0f;
            *(f32*)0xCC008000 = 0.0f;
            *(f32*)0xCC008000 = 0.0f;
            *(f32*)0xCC008000 = 8.0f;
            *(f32*)0xCC008000 = 8.0f;
            *(f32*)0xCC008000 = 0.0f;
            *(f32*)0xCC008000 = 1.0f;
            *(f32*)0xCC008000 = 0.0f;
            *(f32*)0xCC008000 = 8.0f;
            *(f32*)0xCC008000 = neg16 * 0.5f;
            *(f32*)0xCC008000 = 0.0f;
            *(f32*)0xCC008000 = 1.0f;
            *(f32*)0xCC008000 = 1.0f;
            *(f32*)0xCC008000 = neg16 * 0.5f;
            *(f32*)0xCC008000 = neg16 * 0.5f;
            *(f32*)0xCC008000 = 0.0f;
            *(f32*)0xCC008000 = 0.0f;
            *(f32*)0xCC008000 = 1.0f;
        }
        i++;
        spark++;
    }
}
