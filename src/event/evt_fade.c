#include "evt/evt_fade.h"
#include "driver/camdrv.h"
#include "driver/fadedrv.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include <dolphin/gx.h>

extern GlobalWork* gp;

static const GXColor sFadeInColor = {0, 0, 0, 0xFF};
static const GXColor sFadeOutColor = {0, 0, 0, 0xFF};

/* Functions in REVERSE binary order (highest address first in source) */

USER_FUNC(evt_fade_in) {
    GXColor color;
    s32 duration;
    duration = evtGetValue(event, event->args[0]);
    if (isFirstCall) {
        color = sFadeInColor;
        fadeEntry(FADE_IN_BLACK, duration, color);
    }
    if (fadeIsFinish() != 1) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_out) {
    GXColor color;
    s32 duration;
    duration = evtGetValue(event, event->args[0]);
    if (isFirstCall) {
        color = sFadeOutColor;
        fadeEntry(FADE_OUT_BLACK, duration, color);
    }
    if (fadeIsFinish() != 1) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_entry) {
    s32* args = event->args;
    FadeType type;
    s32 duration;
    u8 r, g, b;
    type = evtGetValue(event, *args++);
    duration = evtGetValue(event, *args++);
    r = evtGetValue(event, *args++);
    g = evtGetValue(event, *args++);
    b = evtGetValue(event, *args++);
    fadeEntry(type, duration, (GXColor){r, g, b, 0xFF});
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_end_wait) {
    return (fadeIsFinish() != 1) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_set_spot_pos) {
    f32 proj[7];
    f32 viewport[6];
    f32 origProj[7];
    f32 projX, projY, projZ;
    CameraEntry* cam;
    s32* args;
    s32 z, y, x;
    s32 halfW;
    args = event->args;
    x = evtGetValue(event, args[0]);
    y = evtGetValue(event, args[1]);
    z = evtGetValue(event, args[2]);
    cam = camGetPtr(4);
    GXGetProjectionv(origProj);
    GXSetProjection(cam->projection, cam->type);
    GXGetProjectionv(proj);
    GXGetViewportv(viewport);
    GXProject((f32)x, (f32)y, (f32)z, cam->view, proj, viewport, &projX, &projY, &projZ);
    halfW = -(s32)gp->fbWidth / 2;
    fadeSetSpotPos((f32)halfW + projX, (f32)((s32)gp->efbHeight >> 1) - projY);
    GXSetProjectionv(origProj);
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
    s32 isArea, fadeInType, fadeInDuration, fadeOutType, fadeOutDuration;
    args = event->args;
    isArea = evtGetValue(event, args[0]);
    fadeInType = evtGetValue(event, args[1]);
    fadeInDuration = evtGetValue(event, args[2]);
    fadeOutType = evtGetValue(event, args[3]);
    fadeOutDuration = evtGetValue(event, args[4]);
    if (isArea == 0) {
        if (fadeInType != -1) gp->mapFadeInType = fadeInType;
        if (fadeInDuration != -1) gp->mapFadeInDuration = fadeInDuration;
        if (fadeOutType != -1) gp->mapFadeOutType = fadeOutType;
        if (fadeOutDuration != -1) gp->mapFadeOutDuration = fadeOutDuration;
    } else {
        if (fadeInType != -1) gp->areaFadeInType = fadeInType;
        if (fadeInDuration != -1) gp->areaFadeInDuration = fadeInDuration;
        if (fadeOutType != -1) gp->areaFadeOutType = fadeOutType;
        if (fadeOutDuration != -1) gp->areaFadeOutDuration = fadeOutDuration;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_tec_onoff) {
    s32* args;
    s32 tec_on, soft;
    args = event->args;
    tec_on = evtGetValue(event, args[0]);
    soft = evtGetValue(event, args[1]);
    if (tec_on != 0) {
        if (soft != 0) { fadeTecSoftOn(); } else { fadeTecOn(); }
    } else {
        if (soft != 0) { fadeTecSoftOff(); } else { fadeTecOff(); }
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
