#include <pebble.h>
  
static Window *window;
static TextLayer *times;
static TextLayer *date;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(times, buffer);
}

static void update_date() {
  text_layer_set_text(date, "penis");
}

static void main_window_load(Window *window) {
  // Create time TextLayer
  times = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(times, GColorClear);
  text_layer_set_text_color(times, GColorWhite);
  text_layer_set_text(times, "00:00");
  
  // Create date TextLayer
  date = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(date, GColorClear);
  text_layer_set_text_color(date, GColorWhite);
  text_layer_set_text(date, "w00t");

  // Improve the layout to be more like a watchface
  text_layer_set_font(times, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(times, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(times));
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(date));
  
  // Make sure the time is displayed from the start
  update_time();
  
  //Update date
  update_date();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(times);
  text_layer_destroy(date);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  window = window_create();
  window_set_background_color(window,GColorBlack);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_set_fullscreen(window,true);

  // Show the Window on the watch, with animated=true
  window_stack_push(window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
