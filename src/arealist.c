#include <pebble.h>
#include "arealist.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static MenuLayer *s_area_menu;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  // s_area_menu
  s_area_menu = menu_layer_create(GRect(0, 0, 144, 168));
  menu_layer_set_click_config_onto_window(s_area_menu, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_area_menu);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  menu_layer_destroy(s_area_menu);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_arealist(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_arealist(void) {
  window_stack_remove(s_window, true);
}
