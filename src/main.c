#include <pebble.h>
#include "messaging.h"
#include "areas.h"


static void message_received(DictionaryIterator *iter, void *context) {
  Tuple *message_type = dict_find(iter, KEY_MESSAGE_TYPE);
  if(!message_type) {
    return;
  }
  switch (message_type->value->uint8) {

    case MESSAGE_TYPE_PARKING_INFO: {
        int32_t capacity = dict_find(iter, KEY_CAPACITY)->value->int32;
        int32_t occupancy = dict_find(iter, KEY_OCCUPANCY)->value->int32;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "parking status received: capacity=%ld, occupancy=%ld", capacity, occupancy);
        //TODO do something with it
      }
      break;
    case MESSAGE_TYPE_AREA_INFO: {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Area status received");
        //TODO do something with it
      }
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "UNKNOWN MESSAGE TYPE: %d", message_type->value->uint8);
      break;
  }
  
}

static void send_message_failed(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "SEND MESSAGE FAILED: %d", reason);
}

static void messaging_init() {
  app_message_register_inbox_received(message_received);
//  app_message_register_inbox_dropped(message_dropped);
//  app_message_register_outbox_sent(message_sent);
  app_message_register_outbox_failed(send_message_failed);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

int main(void) {
  messaging_init();
  areas_window_init();
  app_event_loop();
  areas_window_close();
}


