#include "evt/evt_fade.h"
#include "driver/fadedrv.h"
#include "driver/camdrv.h"
#include "mario/mariost.h"
#include "manager/evtmgr_cmd.h"
#include <dolphin/gx/GXTransform.h>
#include <dolphin/gx/GXGet.h>

extern GlobalWork* gp;
void fadeSetAnimVirtualPos(f32 x, f32 y, f32 z);

USER_FUNC(evt_fade_in) {
    s32 duration;
    GXColor color;
    duration = evtGetValue(event, event->args[0]);
    if (isFirstCall) {
        color.r = 0;
        color.g = 0;
        color.b = 0;
        color.a = 0xFF;
        fadeEntry(9, duration, color);
    }
    if (fadeIsFinish() == 1) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(evt_fade_out) {
    s32 duration;
    GXColor color;
    duration = evtGetValue(event, event->args[0]);
    if (isFirstCall) {
        color.r = 0;
        color.g = 0;
        color.b = 0;
        color.a = 0xFF;
        fadeEntry(10, duration, color);
    }
    if (fadeIsFinish() == 1) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(evt_fade_entry) {
    s32* args;
    s32 type;
    s32 duration;
    u8 r, g, b;
    GXColor color;
    args = event->args;
    type = evtGetValue(event, args[0]);
    duration = evtGetValue(event, args[1]);
    r = (u8)evtGetValue(event, args[2]);
    g = (u8)evtGetValue(event, args[3]);
    b = (u8)evtGetValue(event, args[4]);
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = 0xFF;
    fadeEntry(type, duration, color);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_end_wait) {
    return !fadeIsFinish() ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_set_spot_pos) {
    f32 cameraProj[7];
    f32 viewport[6];
    f32 savedProj[7];
    f32 sx, sy, sz;
    s32* args;
    s32 wx, wy, wz;
    CameraEntry* camera;
    args = event->args;
    wx = evtGetValue(event, args[0]);
    wy = evtGetValue(event, args[1]);
    wz = evtGetValue(event, args[2]);
    camera = camGetPtr(4);
    GXGetProjectionv(savedProj);
    GXSetProjection(camera->projection, camera->type);
    GXGetProjectionv(cameraProj);
    GXGetViewportv(viewport);
    GXProject((f32)wx, (f32)wy, (f32)wz, camera->view, cameraProj, viewport, &sx, &sy, &sz);
    fadeSetSpotPos(sx + (f32)(-(s32)gp->fbWidth / 2), (f32)(gp->efbHeight / 2) - sy);
    GXSetProjectionv(savedProj);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_set_anim_ofs_pos) {
    s32* args;
    f32 x, y;
    args = event->args;
    x = evtGetFloat(event, args[0]);
    y = evtGetFloat(event, args[1]);
    fadeSetAnimOfsPos(x, y);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_set_anim_virtual_pos) {
    s32* args;
    f32 x, y, z;
    args = event->args;
    x = evtGetFloat(event, args[0]);
    y = evtGetFloat(event, args[1]);
    z = evtGetFloat(event, args[2]);
    fadeSetAnimVirtualPos(x, y, z);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_set_mapchange_type) {
    s32* args;
    s32 flag, val1, val2, val3, val4;
    args = event->args;
    flag = evtGetValue(event, args[0]);
    val1 = evtGetValue(event, args[1]);
    val2 = evtGetValue(event, args[2]);
    val3 = evtGetValue(event, args[3]);
    val4 = evtGetValue(event, args[4]);
    if (flag == 0) {
        if (val1 != -1) gp->mapFadeInType = val1;
        if (val2 != -1) gp->mapFadeInDuration = val2;
        if (val3 != -1) gp->mapFadeOutType = val3;
        if (val4 != -1) gp->mapFadeOutDuration = val4;
    } else {
        if (val1 != -1) gp->areaFadeInType = val1;
        if (val2 != -1) gp->areaFadeInDuration = val2;
        if (val3 != -1) gp->areaFadeOutType = val3;
        if (val4 != -1) gp->areaFadeOutDuration = val4;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_tec_onoff) {
    s32* args;
    s32 onoff;
    s32 softfocus;
    args = event->args;
    onoff = evtGetValue(event, args[0]);
    softfocus = evtGetValue(event, args[1]);
    if (onoff != 0) {
        if (softfocus != 0) {
            fadeTecSoftOn();
        } else {
            fadeTecOn();
        }
    } else {
        if (softfocus != 0) {
            fadeTecSoftOff();
        } else {
            fadeTecOff();
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_softfocus_onoff) {
    if (evtGetValue(event, event->args[0]) != 0) {
        fadeSoftFocusOn();
    } else {
        fadeSoftFocusOff();
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_reset) {
    fadeReset(evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}
