#include <inttypes.h>
#include <pebble.h>
#include "lib/pebble-assist.h"

#include "messaging.h"
#include "parkings.h"
#include "parkings_ui.h"


static Window *window;
static MenuLayer *menu_layer;

static GBitmap *bitmap_trend_up;
static GBitmap *bitmap_trend_flat;
static GBitmap *bitmap_trend_down;

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
    return parkings_count() ? parkings_count() : 1;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
	//graphics_context_set_text_color(ctx, GColorBlack);
	if (parkings_get_error()) {
		graphics_draw_text(ctx, parkings_get_error(), fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), 
                       GRect(4, 2, 136, 128), GTextOverflowModeFill, GTextAlignmentLeft, NULL);
	} else {
//    DEBUG("rendering row %d", cell_index->row);
    Parking *parking = parkings_get(cell_index->row);
#if defined(PBL_COLOR)
    if (parking->free >= 0) {
      // the number of free spaces is available: set color to red if full, green otherwise
      graphics_context_set_text_color(ctx,
                                      (parking->free == 0) ?
                                        ColorRed :
                                        (menu_cell_layer_is_highlighted(cell_layer) ? GColorGreen : GColorDarkGreen)
                                      );
    }
#endif
    char subtitle[32];
    if (parking->free >= 0) {
      snprintf(subtitle, sizeof(subtitle), "%" PRId16 "/%" PRId16 "", parking->free, parking->capacity);
    }
    else {
      snprintf(subtitle, sizeof(subtitle), "---/%" PRId16 "", parking->capacity);
    }
    GBitmap *bitmap_trend = NULL;
    switch (parking->trend) {
      case 1:
        bitmap_trend = bitmap_trend_down;
      break;
      case 0:
        bitmap_trend = bitmap_trend_flat;
      break;
      case -1:
        bitmap_trend = bitmap_trend_up;
      break;
    }
    menu_cell_basic_draw(ctx, cell_layer, parking->name, subtitle, bitmap_trend);
	}
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
    return PBL_IF_ROUND_ELSE(menu_layer_is_index_selected(menu_layer, cell_index) ?
                             MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
                             MENU_CELL_BASIC_CELL_HEIGHT);
}


static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  if (!parkings_count()) return;
	parkings_set_current(cell_index->row);
  // do something 
}



static void window_load(Window *window) {
    menu_layer = menu_layer_create_fullscreen(window);
    menu_layer_set_click_config_onto_window(menu_layer, window);
#if defined(PBL_COLOR)
//    menu_layer_set_normal_colors(menu_layer, GColorBlack, GColorWhite);
//    menu_layer_set_highlight_colors(menu_layer, GColorRed, GColorWhite);
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


void parkings_ui_init() {
    bitmap_trend_up = gbitmap_create_with_resource(RESOURCE_ID_IMG_TREND_UP);
    bitmap_trend_flat = gbitmap_create_with_resource(RESOURCE_ID_IMG_TREND_FLAT);
    bitmap_trend_down = gbitmap_create_with_resource(RESOURCE_ID_IMG_TREND_DOWN);
  
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
//    window_stack_push(window, true);
}

void parkings_ui_push() {
	window_stack_push(window, true);
	parkings_request();
}

void parkings_ui_finalize() {
  window_destroy_safe(window);
}

void parkings_ui_reload_data_and_mark_dirty(void) {
	if (!window_is_loaded(window)) return;
	menu_layer_reload_data_and_mark_dirty(menu_layer);
}
