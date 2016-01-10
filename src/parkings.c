//
//  parkings.c
//  LuxParking
//
//  Created by Olivier on 06/01/2016.
//  Copyright © 2016 Olivier Gérardin. All rights reserved.
//

#include <inttypes.h>
#include <pebble.h>
#include "lib/pebble-assist.h"

#include "generated/appinfo.h"
#include "generated/keys.h"

#include "areas.h"
#include "parkings_ui.h"
#include "parkings.h"

static Parking *parkings = NULL;
static uint8_t num_parkings = 0;
static uint8_t current_parking = 0;

static char *error = NULL;

void parkings_init() {
  parkings_ui_init();
}

void parkings_finalize() {
  free_safe(error);
  free_safe(parkings);
  parkings_ui_finalize();
}

void parkings_in_received_handler(DictionaryIterator *iter) {
	Tuple *method_tuple = dict_find(iter, APP_KEY_METHOD);
	if (!method_tuple) return;
	free_safe(error);
    
  uint8_t method = method_tuple->value->uint8;
	switch (method) {
		case KEY_METHOD_REPLY_ERROR: {
			Tuple *error_tuple = dict_find(iter, APP_KEY_ERROR);
			if (!error_tuple) break;
			error = malloc(error_tuple->length);
			strncpy(error, error_tuple->value->cstring, error_tuple->length);
			parkings_reload_data_and_mark_dirty();
			break;
		}
		case KEY_METHOD_REPLY_COUNT:
			free_safe(parkings);
			Tuple *count_tuple = dict_find(iter, APP_KEY_COUNT);
			if (!count_tuple) break;
			num_parkings = (uint8_t) count_tuple->value->int32;
			parkings = malloc(sizeof(Parking) * num_parkings);
      if (parkings == NULL) {
          num_parkings = 0;
      }
			break;
		case KEY_METHOD_REPLY_ITEM: {
			if (!parkings_count()) break;
			Tuple *index_tuple = dict_find(iter, APP_KEY_INDEX);
			if (!index_tuple) break;
			uint8_t index = (uint8_t) index_tuple->value->int32;
			Parking *parking = parkings_get(index);
      if (!parking) break;
			parking->index = index;
			Tuple *name_tuple = dict_find(iter, APP_KEY_NAME);
			if (name_tuple) {
				strncpy(parking->name, name_tuple->value->cstring, sizeof(parking->name) - 1);
			}
      Tuple *open_tuple = dict_find(iter, APP_KEY_OPEN);
      if (open_tuple) parking->open = (int8_t) open_tuple->value->int32;
      Tuple *capacity_tuple = dict_find(iter, APP_KEY_CAPACITY);
      if (capacity_tuple) parking->capacity = (int16_t) capacity_tuple->value->int32;
      Tuple *free_tuple = dict_find(iter, APP_KEY_FREE);
      if (free_tuple) parking->free = (int16_t) free_tuple->value->int32;
      Tuple *trend_tuple = dict_find(iter, APP_KEY_TREND);
      if (trend_tuple) parking->trend = (int8_t) trend_tuple->value->int32;
			DEBUG("Parking[%d] = '%s' %d %d/%d (%d)", parking->index, parking->name, parking->open, parking->free, parking->capacity, parking->trend);
			parkings_reload_data_and_mark_dirty();
			break;
		}
	}
}

void parkings_reload_data_and_mark_dirty() {
	parkings_ui_reload_data_and_mark_dirty();
}

uint8_t parkings_count() {
	return num_parkings;
}

char* parkings_get_error() {
	return (error == NULL && !parkings_count()) ? "Loading..." : error;
}

Parking* parkings_get(uint8_t index) {
	return (index < parkings_count()) ? &parkings[index] : NULL;
}
Parking* parkings_get_current() {
	return &parkings[current_parking];
}

uint8_t parkings_get_current_index() {
	return current_parking;
}

void parkings_set_current(uint8_t index) {
	current_parking = index;
}

void parkings_request() {
//	for (int i = 0; i < num_headlines; i++) {
//		memset(parkings_get(i)->name, 0x0, sizeof(parkings_get(i)->name));
//	}
	num_parkings = 0;
	free_safe(parkings);
	free_safe(error);

    // send request to get all parkings for selected area
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, APP_KEY_TYPE, KEY_TYPE_PARKING);
    dict_write_uint8(iter, APP_KEY_METHOD, KEY_METHOD_REQUEST_GET);
    dict_write_cstring(iter, APP_KEY_AREA, areas_get_current()->name);
    DEBUG("sending request");
    app_message_outbox_send();

	parkings_reload_data_and_mark_dirty();
    
    
}

