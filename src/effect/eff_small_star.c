#include "eff/eff_small_star.h"
#include "eff/n64/eff_small_star_n64.h"

EffectEntry* effSmallStarEntry(s32 type, s32 count, s32 arg2, f32 x, f32 y, f32 z, f32 arg6, f32 arg7, f32 arg8) {
    return effSmallStarN64Entry(type, count, arg2, x, y, z, arg6, arg7, arg8);
}

void effSmallStarCamChg(EffectEntry* entry, s32 camId) {
    effSmallStarN64CamChg(entry, camId);
}
