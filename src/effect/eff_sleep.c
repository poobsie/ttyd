#include "eff/eff_sleep.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include <math.h>

extern GlobalWork* gp;

static const GXColor kSleepColor = { 255, 255, 255, 0 };

void effSleepDisp(CameraId camId, void* param);

EffectEntry* effSleepEntry(s32 a1, s32 a2, f32 x, f32 y, f32 z, f32 scale, f32 rotation) {
    EffectEntry* effect;
    effSleepData* data;
    f32 radians;
    s32 smth;
    s32 i;

    effect = effEntry();
    effect->type = "Sleep";
    effect->count = 4;
    data = __memAlloc(HEAP_EFFECT, effect->count * sizeof(effSleepData));
    effect->userdata = data;
    effect->callback = effSleepMain;
    effect->flags |= 2;
    data->field_0x0 = a1;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->scale = scale;
    data->field_0x14 = 0;
    if (a2 <= 0) {
        data->field_0x18 = 1000;
    } else {
        data->field_0x18 = a2;
    }
    data->field_0x1C = 0;
    radians = (6.2832f * rotation) / 360.0f;
    for (smth = 16, i = 1; i < effect->count; smth += 16, i++) {
        data[i].position.x = (16.0f * (f32)i) * (f32)cos(radians);
        data[i].position.y = (16.0f * (f32)i) * (f32)sin(radians);
        data[i].position.z = 0.0f;
        data[i].scale = 0.2f * (f32)i + 1.0f;
        data[i].field_0x14 = 0;
        data[i].field_0x20 = smth;
    }
    return effect;
}

void effSleepMain(EffectEntry* effect) {
    effSleepData* data;
    s32 i;
    Vec pos;
    Vec disp = { 0.0f, 0.0f, 0.0f };

    data = effect->userdata;
    disp.x = data->position.x;
    disp.y = data->position.y;
    disp.z = data->position.z;
    pos = disp;

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->field_0x18 = 16;
    }

    if (data->field_0x18 < 1000) {
        data->field_0x18--;
    }

    if (data->field_0x18 < 32) {
        data->field_0x14 = data->field_0x18 * 8;
    }

    if (data->field_0x18 < 0) {
        effDelete(effect);
        return;
    }

    data->field_0x1C++;

    if (data->field_0x1C < 32) {
        data->field_0x14 = data->field_0x1C * 8 + 7;
    }

    for (i = 1; i < effect->count; i++) {
        if (data[i].field_0x20 > 0) {
            data[i].field_0x20--;
        } else {
            data[i].field_0x14 += 8;
            if (data[i].field_0x14 > 255) {
                data[i].field_0x14 = 255;
            }
        }
    }

    dispEntry(CAMERA_3D, 2, effSleepDisp, effect, dispCalcZ(&pos));
}

void effSleepDisp(CameraId camId, void* param) {
    Mtx T, S, R, viewMtx;
    GXTexObj texObj;
    GXColor col;
    EffectEntry* effect;
    effSleepData* data;
    CameraEntry* camera;
    s32 parentAlpha;
    s32 i;
    s32 j;
    s32 timeOff;
    f32 sin_j;
    f32 sin_j1;

    effect = (EffectEntry*)param;
    data = (effSleepData*)effect->userdata;
    camera = camGetPtr(camId);
    parentAlpha = data->field_0x14;

    PSMTXTrans(T, data->position.x, data->position.y, data->position.z);
    PSMTXScale(S, data->scale, data->scale, data->scale);
    PSMTXRotRad(R, 'y', -(0.01745329238474369f * camGetPtr(camId)->viewYaw));
    PSMTXConcat(T, R, T);
    PSMTXConcat(T, S, viewMtx);
    PSMTXConcat(camera->view, viewMtx, viewMtx);

    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3c, 0, 0x7d);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOp(0, 0);
    effGetTexObj(0x8c, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xd, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);

    timeOff = 0x32;
    for (i = 1; i < effect->count; i++, timeOff += 0x32) {
        if (data[i].field_0x20 != 0) {
            continue;
        }

        col = kSleepColor;
        col.a = (u8)(data[i].field_0x14 * parentAlpha / 255);
        GXSetChanMatColor(4, col);

        PSMTXTrans(T, data[i].position.x, data[i].position.y, data[i].position.z);
        PSMTXScale(S, data[i].scale, data[i].scale, data[i].scale);
        PSMTXConcat(T, S, T);
        PSMTXConcat(viewMtx, T, T);
        GXLoadPosMtxImm(T, 0);
        GXSetCurrentMtx(0);

        for (j = 0; j < 8; j++) {
            sin_j  = 0.5f * (f32)sin(6.2832f * (f32)((s32)gp->retraceCount + timeOff + j * 10) * 0.015625f);
            sin_j1 = 0.5f * (f32)sin(6.2832f * (f32)((s32)gp->retraceCount + timeOff + (j + 1) * 10) * 0.015625f);

            GXBegin(0x80, 0, 4);
            GXWGFifo.f32 = -4.0f + sin_j1;
            GXWGFifo.f32 = (f32)(j + 1);
            GXWGFifo.f32 = 0.0f;
            GXWGFifo.f32 = 0.0f;
            GXWGFifo.f32 = 0.125f * (f32)(7 - j);
            GXWGFifo.f32 = 4.0f + sin_j1;
            GXWGFifo.f32 = (f32)(j + 1);
            GXWGFifo.f32 = 0.0f;
            GXWGFifo.f32 = 1.0f;
            GXWGFifo.f32 = 0.125f * (f32)(7 - j);
            GXWGFifo.f32 = 4.0f + sin_j;
            GXWGFifo.f32 = (f32)j;
            GXWGFifo.f32 = 0.0f;
            GXWGFifo.f32 = 1.0f;
            GXWGFifo.f32 = 0.125f * (f32)(8 - j);
            GXWGFifo.f32 = -4.0f + sin_j;
            GXWGFifo.f32 = (f32)j;
            GXWGFifo.f32 = 0.0f;
            GXWGFifo.f32 = 0.0f;
            GXWGFifo.f32 = 0.125f * (f32)(8 - j);
        }
    }
}
