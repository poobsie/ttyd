#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"

EffectEntry* effSmallStarN64Entry(s32 type, s32 count, s32 arg2, f32 x, f32 y, f32 z, f32 arg6, f32 arg7, f32 arg8);
void effSmallStarN64CamChg(EffectEntry* entry, s32 camId);
