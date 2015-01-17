#include <pebble.h>
  
static Window *window;
static Window *calendar;
static TextLayer *times;
static TextLayer *dates;
static TextLayer *caltext;

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



static void main_window_load(Window *window) {
  // Create time TextLayer
  times = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(times, GColorClear);
  text_layer_set_text_color(times, GColorWhite);
  text_layer_set_text(times, "00:00");
  
  // Create date TextLayer
  dates = text_layer_create(GRect(-30, 96, 144, 50));
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
  
  // Make sure the time is displayed from the start
  update_time();
  
  //Update date
  update_date();
}

static void cal_window_load(Window *calendar) {
  caltext = text_layer_create(GRect(0, 0, 144, 144));
  text_layer_set_background_color(caltext, GColorClear);
  text_layer_set_text_color(caltext, GColorBlack);
  text_layer_set_font(caltext, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text(caltext, "Cliche lo-fi deep v, semiotics Williamsburg gastropub meggings farm-to-table. Bitters Williamsburg forage, chillwave Pinterest cornhole tilde chia migas yr irony PBR meditation. Cornhole mumblecore flannel, pour-over Pinterest wayfarers blog forage swag Helvetica DIY beard. Occupy vinyl leggings, flannel +1 VHS Helvetica chambray deep v cronut. Readymade yr pork belly, trust fund ennui polaroid raw denim. Brunch Schlitz Portland fingerstache, Williamsburg disrupt gentrify PBR&B mumblecore pork belly four dollar toast. Pinterest salvia raw denim 3 wolf moon pour-over, mustache direct trade American Apparel fap slow-carb hoodie.");
  layer_add_child(window_get_root_layer(calendar), text_layer_get_layer(caltext));
}

static void cal_window_unload(Window *calendar) {
  text_layer_destroy(caltext);
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(times);
  text_layer_destroy(dates);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  window = window_create();
  window_set_background_color(window,GColorBlack);

  calendar = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_set_window_handlers(calendar, (WindowHandlers) {
    .load = cal_window_load,
    .unload = cal_window_unload
  });

  window_set_fullscreen(window,true);
  window_set_fullscreen(calendar,true);

  // Show the Window on the watch, with animated=true
  window_stack_push(window, true);
  window_stack_push(calendar, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(window);
  window_destroy(calendar);
}

int main(void) {
  init();

  app_event_loop();
  deinit();

}
