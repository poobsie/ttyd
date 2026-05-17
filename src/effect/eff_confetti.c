#include "eff/eff_confetti.h"
#include "eff/n64/eff_confetti_n64.h"

EffectEntry* effConfettiEntry(s32 type, f32 x, f32 y, f32 z) {
    return effConfettiN64Entry(type, x, y, z, 1.0f);
}
