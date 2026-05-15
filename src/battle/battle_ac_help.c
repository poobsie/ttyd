#include "battle/battle_ac_help.h"
#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "driver/dispdrv.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "manager/fontmgr.h"

extern BattleWork* _battleWorkPointer;

// Float constants defined in battle_seq_end.c's sdata2
extern const GXColor dat_80420d60;
extern const f32 float_0_80420d64;
extern const f32 float_20_80420d68;
extern const f32 float_neg145_80420d6c;
extern const f32 float_10_80420d70;
extern const f32 float_neg148_80420d74;
extern const f32 float_498_80420d78;

extern s32 FontGetMessageWidthLine(const char* msg, s16* outLineCount);
extern void FontDrawMessage(s32 x, s32 y, const char* str);
void BattleAudience_WinSetActive(u8 active);

const char str_802FE1D8[] = "\202\337\202\301\202\271\201[\202\266\202\252\202\335\202\302\202\251\202\350\202\334\202\271\202\361";
const double double_to_int_802fe1f8 = 4503601774854144;

static void _ac_help_disp(CameraId camId, void* param);

void BattleAcHelpSetHelp(const char* msg) {
    _battleWorkPointer->acHelpMsg = msg;
}

void BattleAcHelpSetDispType(s32 type) {
    _battleWorkPointer->acHelpDispType = type;
    if (type == 0) {
        BattleAudience_WinSetActive(1);
    } else {
        BattleAudience_WinSetActive(0);
    }
}

void BattleAcHelpInit(void) {
}

void BattleAcHelpMain(void) {
    BattleWork* bw;
    bw = _battleWorkPointer;
    if (!bw->acHelpMsg) return;
    if (!bw->acHelpDispType) return;
    if (bw->flags & 2) return;
    dispEntry(8, 1, _ac_help_disp, NULL, float_498_80420d78);
}

static void _ac_help_disp(CameraId camId, void* param) {
    const char* msg;
    s32 alpha;
    s32 pix_width;
    s16 line_count;
    f32 x_center;
    GXColor color;

    alpha = 0xFF;
    if (_battleWorkPointer->acHelpDispType == 1) {
        alpha = 0x80;
    }

    msg = msgSearch(_battleWorkPointer->acHelpMsg);
    if (!msg) {
        msg = str_802FE1D8;
    }

    pix_width = FontGetMessageWidthLine(msg, &line_count);

    x_center = float_0_80420d64 - (f32)((s32)(u16)pix_width / 2);

    color = dat_80420d60;
    color.a = (u8)alpha;

    windowDispGX_Waku_col(
        0,
        color,
        x_center - float_20_80420d68,
        float_neg145_80420d6c + (f32)((s32)line_count * 29),
        (f32)((s32)(u16)pix_width + 40),
        (f32)(((s32)line_count + 1) * 29 + 1),
        float_10_80420d70
    );

    FontDrawStart_alpha((u8)alpha);
    FontDrawMessage(
        (s32)x_center,
        (s32)float_neg148_80420d74 + (s32)line_count * 29,
        msg
    );
}
