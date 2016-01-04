#include <pebble.h>
#include "messaging.h"

#define NUM_AREAS 4

static Window *s_main_window;
static MenuLayer *s_menu_layer;


static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return NUM_AREAS;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  switch(cell_index->row) {
    case 0:
      menu_cell_basic_draw(ctx, cell_layer, "Centre", NULL, NULL);
      break;
    case 1:
      menu_cell_basic_draw(ctx, cell_layer, "Gare", NULL, NULL);
      break;
    case 2:
      menu_cell_basic_draw(ctx, cell_layer, "Kirchberg", NULL, NULL);
      break;
    case 3:
      menu_cell_basic_draw(ctx, cell_layer, "P+R", NULL, NULL);
      break;
    default:
      break;
  }
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return PBL_IF_ROUND_ELSE(
                           menu_layer_is_index_selected(menu_layer, cell_index) ?
                           MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
                           /*CHECKBOX_WINDOW_CELL_HEIGHT*/ 32);
}


static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_uint8(iter, KEY_MESSAGE_TYPE, MESSAGE_TYPE_QUERY_AREA);
  dict_write_uint8(iter, KEY_AREA, cell_index->row);
  app_message_outbox_send();
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
#if defined(PBL_COLOR)
  menu_layer_set_normal_colors(s_menu_layer, GColorBlack, GColorWhite);
  menu_layer_set_highlight_colors(s_menu_layer, GColorRed, GColorWhite);
#endif
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
    .get_num_rows = get_num_rows_callback,
    .draw_row = draw_row_callback,
    .get_cell_height = get_cell_height_callback,
    .select_click = select_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
}


static void main_window_init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void main_window_close() {
  window_destroy(s_main_window);
}

void message_received(DictionaryIterator *iter, void *context) {
  Tuple *message_type = dict_find(iter, KEY_MESSAGE_TYPE);
  if(!message_type) {
    return;
  }
  if(message_type->value->uint8 == MESSAGE_TYPE_PARKING_STATUS) {
    uint16_t capacity = dict_find(iter, KEY_PARKING_CAPACITY)->value->uint16;
    uint16_t occupancy = dict_find(iter, KEY_PARKING_OCCUPANCY)->value->uint16;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "parking status received: capacity=%d, occupancy=%d", capacity, occupancy);
    //TODO show parking status
  }
  
}

static void messaging_init() {
  app_message_register_inbox_received(message_received);
//  app_message_register_inbox_dropped(message_dropped);
//  app_message_register_outbox_sent(message_sent);
//  app_message_register_outbox_failed(message_failed)
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

int main(void) {
  main_window_init();
  messaging_init();
  app_event_loop();
  main_window_close();
}


