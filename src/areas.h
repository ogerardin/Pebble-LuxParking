//
//  areas.h
//  LuxParking
//
//  Created by Olivier on 05/01/2016.
//  Copyright © 2016 Olivier Gérardin. All rights reserved.
//

#pragma once

#include <pebble.h>

typedef struct {
    uint8_t index;
    char name[32];
    uint16_t capacity;
    uint16_t occupancy;
} Area;

void areas_init();
void areas_finalize();
void areas_in_received_handler(DictionaryIterator *iter);
void areas_reload_data_and_mark_dirty();
uint8_t areas_count();
char* areas_get_error();
Area* areas_get(uint8_t index);
Area* areas_get_current();
uint8_t areas_get_current_index();
void areas_set_current(uint8_t index);
void areas_request();