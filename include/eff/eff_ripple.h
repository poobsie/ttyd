#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"

EffectEntry* effRippleEntry(s32 type, f32 x, f32 y, f32 z);
void effRippleSetRxRz(EffectEntry* entry, f32 rx, f32 rz);
void effRippleSetCamId(EffectEntry* entry, s32 camId);
void effRippleSetPosition(EffectEntry* entry, f32 x, f32 y, f32 z);
