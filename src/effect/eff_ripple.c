#include <string.h>
#include "eff/eff_ripple.h"
#include "eff/n64/eff_ripple_n64.h"

typedef struct effRippleData {
    u8 _0x0[0x4];
    f32 x;          // 0x4
    f32 y;          // 0x8
    f32 z;          // 0xC
    u8 _0x10[0x1C]; // 0x10
    f32 rx;         // 0x2C
    f32 rz;         // 0x30
    s32 camId;      // 0x34
} effRippleData;

EffectEntry* effRippleEntry(s32 type, f32 x, f32 y, f32 z) {
    return effRippleN64Entry(type, x, y, z, 0.0f);
}

void effRippleSetRxRz(EffectEntry* entry, f32 rx, f32 rz) {
    effRippleN64SetRxRz(entry, rx, rz);
}

void effRippleSetCamId(EffectEntry* entry, s32 camId) {
    ((effRippleData*)entry->userdata)->camId = camId;
}

void effRippleSetPosition(EffectEntry* entry, f32 x, f32 y, f32 z) {
    if (strcmp(entry->type, "RippleN64") != 0) {
        return;
    }
    ((effRippleData*)entry->userdata)->x = x;
    ((effRippleData*)entry->userdata)->y = y;
    ((effRippleData*)entry->userdata)->z = z;
}
