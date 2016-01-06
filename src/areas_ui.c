#include <pebble.h>
#include "lib/pebble-assist.h"

#include "messaging.h"
#include "areas.h"
#include "areas_ui.h"

#define NUM_AREAS 4

static Window *window;
static MenuLayer *menu_layer;


static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
    return areas_count() ? areas_count() : 1;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
	//graphics_context_set_text_color(ctx, GColorBlack);
	if (areas_get_error()) {
		graphics_draw_text(ctx, areas_get_error(), fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), 
                       GRect(4, 2, 136, 128), GTextOverflowModeFill, GTextAlignmentLeft, NULL);
	} else {
        menu_cell_basic_draw(ctx, cell_layer, areas_get(cell_index->row)->name, NULL, NULL);
	}
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
    return PBL_IF_ROUND_ELSE(menu_layer_is_index_selected(menu_layer, cell_index) ?
                             MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
                             /*CHECKBOX_WINDOW_CELL_HEIGHT*/ 32);
}


static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
if (!areas_count()) return;
	areas_set_current(cell_index->row);
	//parking_ui_push();
  
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_uint8(iter, KEY_TYPE, TYPE_PARKING);
  dict_write_uint8(iter, KEY_METHOD, METHOD_REQUEST_GET);
  app_message_outbox_send();
}



static void window_load(Window *window) {
    menu_layer = menu_layer_create_fullscreen(window);
    menu_layer_set_click_config_onto_window(menu_layer, window);
#if defined(PBL_COLOR)
    menu_layer_set_normal_colors(menu_layer, GColorBlack, GColorWhite);
    menu_layer_set_highlight_colors(menu_layer, GColorRed, GColorWhite);
#endif
    menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
        .get_num_rows = get_num_rows_callback,
        .draw_row = draw_row_callback,
        .get_cell_height = get_cell_height_callback,
        .select_click = select_callback,
    });
    menu_layer_add_to_window(menu_layer, window);
}

static void window_unload(Window *window) {
    menu_layer_destroy_safe(menu_layer);
}


void areas_ui_init() {
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    window_stack_push(window, true);
    areas_request();
}

void areas_ui_push() {
	window_stack_push(window, true);
	areas_request();
}

void areas_ui_finalize() {
  window_destroy_safe(window);
}

void areas_ui_reload_data_and_mark_dirty(void) {
	if (!window_is_loaded(window)) return;
	menu_layer_reload_data_and_mark_dirty(menu_layer);
}