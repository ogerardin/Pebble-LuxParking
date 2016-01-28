#include <inttypes.h>
#include <pebble.h>
#include "lib/pebble-assist.h"

#include "generated/appinfo.h"
#include "generated/keys.h"

#include "areas.h"
#include "parkings.h"
#include "areas_ui.h"

static char *error = NULL;

char* get_error() {
  return error;
}

void reload_data_and_mark_dirty() {
  areas_reload_data_and_mark_dirty();
  parkings_reload_data_and_mark_dirty();
}

static void message_received(DictionaryIterator *iter, void *context) {
  DEBUG("message received");

  Tuple *message_type = dict_find(iter, APP_KEY_TYPE);
  if (!message_type) return;

  free_safe(error);
  uint8_t type = message_type->value->uint8;
  switch (type) {
    case KEY_TYPE_ERROR: {
      Tuple* error_tuple = dict_find(iter, APP_KEY_ERROR);
      if (!error_tuple) break;
      error = malloc(error_tuple->length);
      strncpy(error, error_tuple->value->cstring, error_tuple->length);
      reload_data_and_mark_dirty();
      break;
    }
    case KEY_TYPE_PARKING:
        DEBUG("Parking message received");
        parkings_in_received_handler(iter);
      break;
    case KEY_TYPE_AREA:
        DEBUG("Area message received");
        areas_in_received_handler(iter);
      break;
    default:
      ERROR("UNKNOWN MESSAGE TYPE: %d", type);
      break;
  }
}

static void send_message_failed(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  ERROR("SEND MESSAGE FAILED: %d", reason);
}

static void init() {
  app_message_register_inbox_received(message_received);
//  app_message_register_inbox_dropped(message_dropped);
//  app_message_register_outbox_sent(message_sent);
  app_message_register_outbox_failed(send_message_failed);
//  app_message_open_max();
  app_message_open(2048, 2048);
  
  areas_init();
  parkings_init();
}

static void finalize() {
  free_safe(error);
  areas_finalize();
  parkings_finalize();
}


int main(void) {
  init();
  app_event_loop();
  finalize();
}


