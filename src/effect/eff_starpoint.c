#include "eff/eff_starpoint.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "memory.h"
#include <dolphin/mtx.h>

typedef struct EffStarPointData {
    /* 0x00 */ s32 type;
    /* 0x04 */ s32 life;
    /* 0x08 */ s32 age;
    /* 0x0C */ f32 x;
    /* 0x10 */ f32 y;
    /* 0x14 */ f32 z;
    /* 0x18 */ f32 vx;
    /* 0x1C */ f32 vz;
    /* 0x20 */ f32 speed;
    /* 0x24 */ f32 accel;
    /* 0x28 */ s32 pad28;
    /* 0x2C */ s32 alpha;
    /* 0x30 */ s32 delay;
} EffStarPointData; /* size 0x34 */

void effStarPointMain(EffectEntry* effect);
void effStarPointDisp(CameraId camId, void* param);

/* Defined before functions so MWCC can deduplicate anonymous literals */
const Vec vec3_802fdbe0 = { 0.0f, 0.0f, 0.0f };
const char str_StarPoint[] = "StarPoint";
static const f64 double_to_int_802fdbf8 = 4503599627370496.0;

EffectEntry* effStarPointEntry(s32 type, s32 numStars, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    EffStarPointData* data;
    EffStarPointData* star;
    s32 i;
    s32 delay;
    s32 angle_deg;
    f32 angle;
    f32 t;
    f32 t2;
    f32 sin_val;
    f32 cos_val;
    f32 c_sin;
    f32 c_cos;
    f32 c_one;

    effect = effEntry();
    effect->type = str_StarPoint;
    effect->count = numStars + 1;
    data = __memAlloc(HEAP_EFFECT, effect->count * sizeof(EffStarPointData));
    effect->userdata = data;
    effect->callback = effStarPointMain;
    data->type = type;
    data->life = 0x3C;
    data->age = 0;
    c_sin = 0.16605f;
    c_cos = 0.4967f;
    c_one = 1.0f;

    star = data + 1;
    delay = 4;
    for (i = 1; i < effect->count; i++) {
        angle_deg = rand() % 360;
        angle = 6.2831855f * (f32)angle_deg / 360.0f;

        star->x = 0.0f;
        star->y = 0.0f;
        star->z = 0.0f;

        /* sin(angle) polynomial approximation */
        if (angle <= 3.1415927f) {
            if (angle <= 1.5707964f) {
                t2 = angle * angle;
                sin_val = ((0.00761f * t2 - c_sin) * t2 + c_one) * angle;
            } else {
                t = 1.5707964f - (angle - 1.5707964f);
                t2 = t * t;
                sin_val = ((0.00761f * t2 - c_sin) * t2 + c_one) * t;
            }
        } else {
            if (angle < 4.712389f) {
                t = angle - 3.1415927f;
                t2 = t * t;
                sin_val = ((0.00761f * t2 - c_sin) * t2 + c_one) * t;
                sin_val = -sin_val;
            } else {
                t = 1.5707964f - (angle - 4.712389f);
                t2 = t * t;
                sin_val = ((0.00761f * t2 - c_sin) * t2 + c_one) * t;
                sin_val = -sin_val;
            }
        }
        star->vx = 2.0f * sin_val;

        /* cos(angle) polynomial approximation */
        if (angle <= 3.1415927f) {
            if (angle <= 1.5707964f) {
                t2 = angle * angle;
                cos_val = (0.03705f * t2 - c_cos) * t2 + c_one;
            } else {
                t = 1.5707964f - (angle - 1.5707964f);
                t2 = t * t;
                cos_val = (0.03705f * t2 - c_cos) * t2 + c_one;
                cos_val = -cos_val;
            }
        } else {
            if (angle < 4.712389f) {
                t = angle - 3.1415927f;
                t2 = t * t;
                cos_val = (0.03705f * t2 - c_cos) * t2 + c_one;
                cos_val = -cos_val;
            } else {
                t = 1.5707964f - (angle - 4.712389f);
                t2 = t * t;
                cos_val = (0.03705f * t2 - c_cos) * t2 + c_one;
            }
        }
        star->vz = 2.0f * cos_val;

        star->speed = 14.0f;
        star->accel = 2.0f;
        star->alpha = 0;
        star->delay = delay;
        delay += 3;
        star++;
    }
    data->x = x;
    data->y = y;
    data->z = z;
    return effect;
}

void effStarPointMain(EffectEntry* effect) {
    Vec tmp;
    Vec pos;
    EffStarPointData* data;
    s32 i;

    data = (EffStarPointData*)effect->userdata;
    pos = vec3_802fdbe0;
    pos.x = data->x;
    pos.y = data->y;
    pos.z = data->z;
    tmp = pos;

    data->life--;
    data->age++;
    if (data->life < 0) {
        effDelete(effect);
        return;
    }

    i = 1;
    data++;
    while (i < effect->count) {
        if (data->delay != 0) {
            data->delay--;
            data->alpha = 0;
        } else {
            data->speed -= data->accel;
            data->x += data->vx;
            data->y += data->speed;
            data->z += data->vz;
            if (data->speed < 0.0f) {
                data->alpha -= 2;
                if (data->alpha < 0) {
                    data->alpha = 0;
                }
            } else {
                data->alpha = 0xFF;
            }
        }
        i++;
        data++;
    }
    dispEntry(CAMERA_3D, 1, effStarPointDisp, effect, dispCalcZ(&tmp));
}

void effStarPointDisp(CameraId camId, void* param) {
    Mtx mtxTrans;
    Mtx mtxRot;
    Mtx mtxLocal;
    EffectEntry* effect;
    EffStarPointData* data;
    EffStarPointData* star;
    s32 i;

    effect = (EffectEntry*)param;
    data = (EffStarPointData*)effect->userdata;

    PSMTXTrans(mtxTrans, data->x, data->y, data->z);
    PSMTXRotRad(mtxRot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
    PSMTXConcat(mtxTrans, mtxRot, mtxTrans);

    for (i = 1, star = data + 1; i < effect->count; i++, star++) {
        if (star->alpha != 0) {
            PSMTXTrans(mtxLocal, star->x, star->y, star->z);
            PSMTXConcat(mtxTrans, mtxLocal, mtxLocal);
            iconDispGx2(mtxLocal, 0, 0x18c);
        }
    }
}
