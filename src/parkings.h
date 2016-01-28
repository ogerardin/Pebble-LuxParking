//
//  parkings.h
//  LuxParking
//
//  Created by Olivier on 06/01/2016.
//  Copyright © 2016 Olivier Gérardin. All rights reserved.
//

#pragma once

#include <pebble.h>

typedef struct {
  uint8_t index;
  uint16_t id;
  char name[16];
  char area[16];
  int8_t open;
  int16_t capacity;
  int16_t free;
  int8_t trend;
} Parking;

void parkings_init();
void parkings_finalize();
void parkings_in_received_handler(DictionaryIterator *iter);
void parkings_reload_data_and_mark_dirty();
uint8_t parkings_count();
char* parkings_get_error();
Parking* parkings_get(uint8_t index);
Parking* parkings_get_current();
uint8_t parkings_get_current_index();
void parkings_set_current(uint8_t index);
void parkings_request();