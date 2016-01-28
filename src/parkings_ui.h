#pragma once

#include <pebble.h>

void parkings_ui_init();
void parkings_ui_push();
void parkings_ui_finalize();
void parkings_ui_reload_data_and_mark_dirty();
void parkings_ui_set_timestamp(time_t t);