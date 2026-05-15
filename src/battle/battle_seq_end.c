#include "battle/battle_seq_end.h"
#include "driver/camdrv.h"

// Constants shared with battle_ac_help.c
const GXColor dat_80420d60 = {0xFF, 0xFF, 0xFF, 0x00};
const f32 float_0_80420d64 = 0.0f;
const f32 float_20_80420d68 = 20.0f;
const f32 float_neg145_80420d6c = -145.0f;
const f32 float_10_80420d70 = 10.0f;
const f32 float_neg148_80420d74 = -148.0f;
const f32 float_498_80420d78 = 498.0f;

//.data
RankUpData _rank_up_data[] = {
    {1, 0, "msg_menu_mario_name_0", "butai_01"},
    {10, 1, "msg_menu_mario_name_1", "butai_01"},
    {20, 2, "msg_menu_mario_name_2", "butai_02"},
    {30, 3, "msg_menu_mario_name_3", "butai_03"},
    {0xFFFF, 0, NULL, NULL},
};

RankUpData* _get_rank_data(s32 level) {
    RankUpData* rank;

    for (rank = _rank_up_data; level != rank->startLevel; rank++) {
        if (!rank->rankUpMessage) {
            return NULL;
        }
    }
    return rank;
}

void _lvup_select_object_disp(CameraId camId, void* param) {

}
