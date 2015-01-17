#include <pebble.h>

#define NUM_MENU_ITEMS 5
#define TAP_NOT_DATA true
static Window *window;
static Window *event_list;
static TextLayer *times;
static TextLayer *dates;
static TextLayer *caltext;
static MenuLayer *menu_layer;
static TextLayer *static_time;

static void tap_handler(AccelAxisType axis, int32_t direction) {
  switch (axis) {
  case ACCEL_AXIS_X:
    break;
  case ACCEL_AXIS_Y:
    if (direction > 0)
        if (window_is_loaded(window))
          window_stack_push(event_list, true);
    break;
  case ACCEL_AXIS_Z:
    break;
  }
}


static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return NUM_MENU_ITEMS;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
    switch (cell_index->section) {
    case 0:
      // Use the row to specify which item we'll draw
        switch (cell_index->row) {
        case 0:
          // Use to show data in each cell
          menu_cell_basic_draw(ctx, cell_layer, "Cell 1 Title", "Subtitle", NULL);
          break;
        case 1:
          menu_cell_basic_draw(ctx, cell_layer, "Cell 2 Title", "Subtitle", NULL); 
          break;
        case 2:
          menu_cell_basic_draw(ctx, cell_layer, "Cell 3 Title", "Subtitle", NULL); 
          break;
        case 3:
          menu_cell_basic_draw(ctx, cell_layer, "Cell 4 Title", "Subtitle", NULL); 
          break;
        case 4:
          menu_cell_basic_draw(ctx, cell_layer, "Cell 5 Title", "Subtitle", NULL); 
          break;
        }
      break;
    }
}



// Here we capture when a user selects a menu item
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  switch (cell_index->row) {
    // This is the menu item with the cycling icon
    case 1:
      // After changing the icon, mark the layer to have it updated
      layer_mark_dirty(menu_layer_get_layer(menu_layer));
      break;
  }

}



// Select-button Handler 
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (window_is_loaded(window)) {
    window_stack_push(event_list, true);
  }
}

      
static void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}
static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 24h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(times, buffer);

}

static void update_date() {
  time_t temp = time(NULL); 
  static char buffer[] = "Loading text";
  struct tm *tick_time = localtime(&temp);
  strftime(buffer, sizeof(buffer), "%a. %d", tick_time);
  text_layer_set_text(dates, buffer);
}

static void update_static_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 24h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(static_time, buffer);
}



static void main_window_load(Window *window) {
  // Create time TextLayer
  times = text_layer_create(GRect(0,70, 144, 100));
  text_layer_set_background_color(times, GColorClear);
  text_layer_set_text_color(times, GColorWhite);
  text_layer_set_text(times, "00:00");
  
  // Create date TextLayer
  dates = text_layer_create(GRect(-30, 50, 144, 50));
  text_layer_set_background_color(dates, GColorClear);
  text_layer_set_text_color(dates, GColorWhite);
  text_layer_set_text(dates, "Loading text");

  // Improve the layout to be more like a watchface
  text_layer_set_font(times, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(times, GTextAlignmentCenter);

  text_layer_set_font(dates, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(dates, GTextAlignmentRight);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(times));
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(dates));
  
  // Add click handler
  window_set_click_config_provider(window, click_config_provider);

  
  // Make sure the time is displayed from the start
  update_time();
  
  //Update date
  update_date();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(times);
  text_layer_destroy(dates);
}

static void cal_window_load(Window *event_list) {
  //The static time layer
  static_time = text_layer_create(GRect(0, 125, 144, 55));
  text_layer_set_background_color(static_time, GColorBlack);
  text_layer_set_text_color(static_time, GColorWhite);
  text_layer_set_text_alignment(static_time, GTextAlignmentCenter);
  text_layer_set_font(static_time, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text(static_time, "00:00");
  
  
  Layer *window_layer = window_get_root_layer(event_list);
  //GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(GRect(0, 0, 144, 125));
  
  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_rows = menu_get_num_rows_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, event_list);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
  layer_add_child(window_get_root_layer(event_list), text_layer_get_layer(static_time));
  
  //Set the time correctly
  update_static_time();
  
  //Set the date

} 

static void cal_window_unload(Window *calendar) {
  text_layer_destroy(caltext);
  menu_layer_destroy(menu_layer);
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  window = window_create();
  window_set_background_color(window, GColorBlack);

  event_list = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_set_window_handlers(event_list, (WindowHandlers) {
    .load = cal_window_load,
    .unload = cal_window_unload
  });


  window_set_fullscreen(window, true);
  window_set_fullscreen(event_list, true);

  // Show the Window on the watch, with animated=true
  window_stack_push(event_list, true);
  window_stack_push(window, true);
  
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
      // Use tap service? If not, use data service
  if (TAP_NOT_DATA) {
    // Subscribe to the accelerometer tap service
    accel_tap_service_subscribe(tap_handler);
  } else {
    // Subscribe to the accelerometer data service
    // Choose update rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_25HZ);
  }


}

static void deinit() {
  // Destroy Window
  window_destroy(window);
  window_destroy(event_list);
  
      // Use tap service? If not, use data service
  if (TAP_NOT_DATA) {
    // Subscribe to the accelerometer tap service
    accel_tap_service_subscribe(tap_handler);
  } else {
    // Subscribe to the accelerometer data service
    // Choose update rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_25HZ);
  }


}

int main(void) {
  init();

  app_event_loop();
  deinit();

}
