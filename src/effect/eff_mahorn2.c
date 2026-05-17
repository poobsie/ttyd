/* Mahon = Wizzerd */

#include "eff/eff_mahorn2.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_hit.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>
#include <string.h>

static const Vec vec3_80301948[3] = {
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}
};

static const f32 dat_8030196c[6] = {0.0f, 2.0f, 2.0f, 12.0f, 12.0f, 14.0f};
static const f32 dat_80301984[6] = {0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f};
static const GXColor dat_80422598 = {0x00, 0xDA, 0x00, 0x00};

void effMahorn2Main(EffectEntry *effect);
void effMahorn2Disp(CameraId camId, void *param);

EffectEntry *effMahorn2Entry(BOOL unused, f32 posX, f32 posY, f32 posZ,
                              f32 dstX, f32 dstY, f32 dstZ, f32 scale, f32 radius) {
    EffectEntry *effect;
    effMahorn2Data *data;
    f32 ang_num;
    s32 i;

    effect = effEntry();
    effect->type = "Mahorn2";
    effect->count = 14;
    data = __memAlloc(HEAP_EFFECT, sizeof(effMahorn2Data) * 14);
    effect->userdata = data;
    effect->callback = effMahorn2Main;

    data->unused = unused;
    data->unused2 = 0;
    data->intplStep = 0;
    data->position.x = posX;
    data->position.y = posY;
    data->position.z = posZ;
    data->target.x = dstX;
    data->target.y = dstY;
    data->target.z = dstZ;
    data->scale = scale;
    data->hitDistance = radius;
    data->alpha = 255;
    data->type = 0;
    data->hasHit = FALSE;
    data++;

    for (i = 1; i < 4; i++, data++) {
        memset(data, 0, sizeof(effMahorn2Data));
        data->type = 1;
        data->position.x = 0.0f;
        data->position.y = 0.0f;
        data->position.z = 0.0f;
        data->scale = 6.0f;
        data->alpha = 0;
        data->counter = (i - 1) * 15;
    }

    memset(data, 0, sizeof(effMahorn2Data));
    data->type = 2;
    data->position.x = 0.0f;
    data->position.y = 0.0f;
    data->position.z = 0.0f;
    data->scale = 0.0f;
    data->alpha = 153;
    data->counter = 15;
    data++;

    memset(data, 0, sizeof(effMahorn2Data));
    data->type = 3;
    data->position.x = 0.0f;
    data->position.y = 0.0f;
    data->position.z = 0.0f;
    data->scale = 0.0f;
    data->alpha = 153;
    data->counter = 60;
    data++;

    for (i = 6; i < effect->count; i++, data++) {
        memset(data, 0, sizeof(effMahorn2Data));
        data->type = 4;
        data->position.x = dstX - posX;
        data->position.y = dstY - posY;
        data->position.z = dstZ - posZ;
        ang_num = 6.2831855f * (f32)(i - 6);
        data->target.x = 6.0f * (f32)sin(ang_num / (f32)(effect->count - 6));
        data->target.y = 6.0f * (f32)cos(ang_num / (f32)(effect->count - 6));
        data->target.z = 0.0f;
        data->scale = 0.0f;
        data->alpha = 255;
        data->counter = 0;
    }

    return effect;
}

void effMahorn2Main(EffectEntry *effect) {
    Vec sp38, sp2C, sp20, sp14, sp8;
    effMahorn2Data *data;
    effMahorn2Data *data0;
    f32 hitDist, mainScale;
    f32 angle;
    s32 i;

    data = (effMahorn2Data *)effect->userdata;

    sp20 = vec3_80301948[0];
    sp20.x = data->position.x;
    sp20.y = data->position.y;
    sp20.z = data->position.z;
    sp38 = sp20;

    sp14 = vec3_80301948[1];
    sp14.x = data->target.x;
    sp14.y = data->target.y;
    sp14.z = data->target.z;
    sp2C = sp14;

    hitDist = data->hitDistance;
    mainScale = data->scale;

    i = 0;
    while (i < effect->count) {
        if (data->counter != 0) {
            data->counter--;
        } else {
            switch (data->type) {
            case 0:
                break;
            case 1:
                data->alpha += 4;
                if (data->alpha > 255) {
                    data->alpha = 255;
                }
                data->scale = (-data->scale * 0.0625f) + data->scale;
                break;
            case 2:
                if (data->intplStep < 90) {
                    data->scale = intplGetValue(11, ++data->intplStep, 45, 0.0f, 3.0f);
                }
                break;
            case 3: {
                sp8 = vec3_80301948[2];
                sp8.x = sp38.x + data->position.x;
                sp8.y = sp38.y + data->position.y;
                sp8.z = sp38.z + data->position.z;
                if (PSVECDistance(&sp2C, &sp8) > hitDist) {
                    angle = 6.2831855f * angleABf(sp38.x + data->position.x,
                                                   sp38.y + data->position.y,
                                                   sp2C.x, sp2C.y) / 360.0f;
                    data->position.x += hitDist * (f32)sin(angle);
                    data->position.y += hitDist * (f32)cos(angle);
                    data->scale = (4.0f - data->scale) * 0.125f + data->scale;
                } else {
                    data->scale = (-data->scale * 0.125f) + data->scale;
                    data0 = (effMahorn2Data *)effect->userdata;
                    if (!data0->hasHit) {
                        data0->hasHit = TRUE;
                        effHitEntry(0, 4, sp2C.x, sp2C.y, sp2C.z, mainScale);
                    }
                }
                break;
            }
            case 4: {
                data0 = (effMahorn2Data *)effect->userdata;
                if (data0->hasHit) {
                    data->position.x += data->target.x;
                    data->position.y += data->target.y;
                    data->position.z += data->target.z;
                    data->target.x *= 0.88f;
                    data->target.y *= 0.88f;
                    data->target.z *= 0.88f;
                    data->scale = 1.0f;
                    data->alpha -= 4;
                    if (data->alpha < 0) {
                        data->alpha = 0;
                        effDelete(effect);
                        return;
                    }
                }
                break;
            }
            }
        }
        i++;
        data++;
    }

    dispEntry(CAMERA_3D, 2, (DispCallback)effMahorn2Disp, effect, dispCalcZ(&sp38));
}

void effMahorn2Disp(CameraId camId, void *param) {
    Mtx spF0;
    Mtx spC0;
    Mtx sp90;
    Mtx sp60;
    GXTexObj sp40;
    f32 ha[6];
    f32 ua[6];
    GXColor spC;
    GXColor col;
    CameraEntry *camera;
    EffectEntry *effect;
    effMahorn2Data *data;
    s32 mainAlpha;
    f32 mainScale;
    f32 w, h, tu, tv;
    s32 i;
    f32 *hp;
    f32 *up;
    s32 n;

    effect = (EffectEntry *)param;
    data = (effMahorn2Data *)effect->userdata;
    camera = camGetPtr(camId);
    mainAlpha = data->alpha;
    mainScale = data->scale;

    PSMTXTrans(sp60, data->position.x, data->position.y, data->position.z);

    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    i = 1;
    data++;
    while (i < effect->count) {
        if (data->counter == 0) {
            if (data->type == 1) {
                effGetTexObj(0x8F, &sp40);
                GXLoadTexObj(&sp40, GX_TEXMAP0);
            } else if (data->type >= 2 && data->type < 5) {
                effGetTexObj(0x90, &sp40);
                GXLoadTexObj(&sp40, GX_TEXMAP0);
            }

            col = dat_80422598;
            col.a = (u8)((data->alpha * mainAlpha) / 255);
            spC = col;
            GXSetChanMatColor(GX_COLOR0A0, spC);

            PSMTXTrans(spF0, data->position.x, data->position.y, data->position.z);
            {
                f32 s;
                s = data->scale * mainScale;
                PSMTXScale(sp90, s, s, s);
            }
            PSMTXConcat(spF0, sp90, spF0);
            PSMTXRotRad(spC0, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
            PSMTXConcat(spF0, spC0, spF0);
            PSMTXConcat(sp60, spF0, spF0);
            PSMTXConcat(camera->view, spF0, spF0);
            GXLoadPosMtxImm(spF0, GX_PNMTX0);
            GXSetCurrentMtx(GX_PNMTX0);

            w = 0.0f; h = 0.0f; tu = 0.0f; tv = 0.0f;
            switch (data->type) {
            case 1:
                w = 16.0f; tu = 2.0f; h = 16.0f; tv = 2.0f;
                break;
            case 2:
                w = 16.0f; tu = 2.0f; h = 16.0f; tv = 2.0f;
                break;
            case 3:
                h = 2.0f; w = 16.0f; tu = 2.0f; tv = 2.0f;
                break;
            case 4:
                w = 8.0f; tu = 2.0f; h = 8.0f; tv = 2.0f;
                break;
            }

            if (data->type != 3) {
                GXBegin(GX_QUADS, GX_VTXFMT0, 4);
                GXPosition3f32(w * -0.5f, h * 0.5f, 0.0f);
                GXTexCoord2f32(0.0f, 0.0f);
                GXPosition3f32(w * 0.5f, h * 0.5f, 0.0f);
                GXTexCoord2f32(tu, 0.0f);
                GXPosition3f32(w * 0.5f, h * -0.5f, 0.0f);
                GXTexCoord2f32(tu, tv);
                GXPosition3f32(w * -0.5f, h * -0.5f, 0.0f);
                GXTexCoord2f32(0.0f, tv);
            } else {
                f32 hhalf;
                f32 nhalf;
                hhalf = h * 0.5f;
                nhalf = h * -0.5f;
                hp = ha;
                up = ua;
                n = 0;
                do {
                    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
                    memcpy(ha, dat_8030196c, sizeof(ha));
                    memcpy(ua, dat_80301984, sizeof(ua));
                    GXPosition3f32(hp[0], hhalf, 0.0f);
                    GXTexCoord2f32(2.0f * up[0], 0.0f);
                    GXPosition3f32(hp[1], hhalf, 0.0f);
                    GXTexCoord2f32(2.0f * up[1], 0.0f);
                    GXPosition3f32(hp[1], nhalf, 0.0f);
                    GXTexCoord2f32(2.0f * up[1], tv);
                    GXPosition3f32(hp[0], nhalf, 0.0f);
                    GXTexCoord2f32(2.0f * up[0], tv);
                    hp += 2;
                    up += 2;
                    n++;
                } while (n < 3);
            }
        }
        i++;
        data++;
    }
}
