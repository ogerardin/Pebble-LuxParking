//
//  areas.c
//  LuxParking
//
//  Created by Olivier on 05/01/2016.
//  Copyright © 2016 Olivier Gérardin. All rights reserved.
//

#include <inttypes.h>
#include <pebble.h>
#include "lib/pebble-assist.h"

#include "messaging.h"
#include "areas_ui.h"
#include "areas.h"

static Area *areas = NULL;
static uint8_t num_areas = 0;
static uint8_t current_area = 0;

static char *error = NULL;

void areas_init() {
  areas_ui_init();
}

void areas_finalize() {
  free_safe(error);
  free_safe(areas);
  areas_ui_finalize();
}

void areas_in_received_handler(DictionaryIterator *iter) {
	Tuple *method_tuple = dict_find(iter, KEY_METHOD);
	if (!method_tuple) return;
	free_safe(error);
    
  uint8_t method = method_tuple->value->uint8;
	switch (method) {
		case METHOD_REPLY_ERROR: {
			Tuple *error_tuple = dict_find(iter, KEY_ERROR);
			if (!error_tuple) break;
			error = malloc(error_tuple->length);
			strncpy(error, error_tuple->value->cstring, error_tuple->length);
			areas_reload_data_and_mark_dirty();
			break;
		}
		case METHOD_REPLY_COUNT:
			free_safe(areas);
			Tuple *count_tuple = dict_find(iter, KEY_COUNT);
			if (!count_tuple) break;
			num_areas = (uint8_t) count_tuple->value->int32;
			areas = malloc(sizeof(Area) * num_areas);
      if (areas == NULL) {
          num_areas = 0;
      }
			break;
		case METHOD_REPLY_ITEM: {
			if (!areas_count()) break;
			Tuple *index_tuple = dict_find(iter, KEY_INDEX);
			if (!index_tuple) break;
			uint8_t index = (uint8_t) index_tuple->value->int32;
			Area *area = areas_get(index);
      if (!area) break;
			area->index = index;
			Tuple *name_tuple = dict_find(iter, KEY_NAME);
			if (name_tuple) {
				strncpy(area->name, name_tuple->value->cstring, sizeof(area->name) - 1);
			}
			DEBUG("Area[%d] = '%s'", area->index, area->name);
			areas_reload_data_and_mark_dirty();
			break;
		}
	}
}

void areas_reload_data_and_mark_dirty() {
	areas_ui_reload_data_and_mark_dirty();
}

uint8_t areas_count() {
	return num_areas;
}

char* areas_get_error() {
	return (error == NULL && !areas_count()) ? "Loading..." : error;
}

Area* areas_get(uint8_t index) {
	return (index < areas_count()) ? &areas[index] : NULL;
}

Area* areas_get_current() {
	return &areas[current_area];
}

uint8_t areas_get_current_index() {
	return current_area;
}

void areas_set_current(uint8_t index) {
	current_area = index;
}

void areas_request() {
//	for (int i = 0; i < num_headlines; i++) {
//		memset(areas_get(i)->name, 0x0, sizeof(areas_get(i)->name));
//	}
	num_areas = 0;
	free_safe(areas);
	free_safe(error);
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, KEY_TYPE, TYPE_AREA);
	dict_write_uint8(iter, KEY_METHOD, METHOD_REQUEST_GET);
	dict_write_end(iter);
	app_message_outbox_send();
	areas_reload_data_and_mark_dirty();
}

