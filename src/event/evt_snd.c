#include "evt/evt_snd.h"
#include "pmario_sound.h"
#include "sound.h"
#include "mario/mariost.h"
#include "win/win_main.h"
#include "driver/camdrv.h"

extern SoundWork sound;
extern PaperSoundBGM psbgm[2];

void psndSetReverb(s32 reverb);
void L_psndBGM_stop(void);
void L_camDispOn(s32 camId);
void N_pouchUnEquipBadgeID(s32 badgeId);

void seq_gameOverInit(void) {
    winReInit();
    L_camDispOn(4);
    psndStopAllFadeOut();
    psndSetReverb(0);
    L_psndBGM_stop();
    psndClearFlag(0x80);
    psndClearFlag(0x20);
    psndClearFlag(0x40);
    N_pouchUnEquipBadgeID(0x143);
    marioStSystemLevel(4);
}

void seq_gameOverExit(void) {
    psndStopAllFadeOut();
    marioStSystemLevel(0);
}

USER_FUNC(N_evt_snd_bgm_unpause) {
    u32 streamId;
    streamId = (u32)psbgm[0].streamId;
    if (streamId != (u32)-1) {
        sound.streams[streamId].flags &= ~0x8000;
    }
    return EVT_RETURN_DONE;
}
