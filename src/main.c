#include <pebble.h>

#define TAP_NOT_DATA true
  
static Window *window;
static Window *weather;
static TextLayer *times;
static TextLayer *dates;
static TextLayer *condition;
static TextLayer *clocks;
static TextLayer *day;

int delta; 

static void data_handler(AccelData *data, uint32_t num_samples) {
  // Measures change in accelerometer
  delta = data[0].y - data[1].y;
  if (delta < 0) {
    delta *= -1; 
  }

  // Determines if change was user-intended, and loads calendar
  if (delta > 400) {
    if (!window_is_loaded(weather)) {
      window_stack_push(weather, true);
    } else {
      window_stack_pop(true);
    }
  }
}


// Select-button Handler 
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Loads event list on button click
  if (window_is_loaded(window)) {
    window_stack_push(weather, true);
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

static void update_weather_time() {
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
  text_layer_set_text(clocks, buffer);

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

//load the weather/clock window
static void weather_load(Window *weather) {
  //Clock Layer
  clocks = text_layer_create(GRect(0, 84, 144, 84));
  text_layer_set_background_color(clocks, GColorWhite);
  text_layer_set_text_color(clocks, GColorBlack);
  text_layer_set_text(clocks, "Loading..");
  
  //Date Layer
  day = text_layer_create(GRect(0, 84, 144, 84));
  text_layer_set_background_color(day, GColorClear);
  text_layer_set_text_color(day, GColorBlack);
  text_layer_set_text(day, "Loading..");
  
  //Weather Layer
  condition = text_layer_create(GRect(0, 0, 144, 84));
  text_layer_set_background_color(condition, GColorBlack);
  text_layer_set_text_color(condition, GColorWhite);
  text_layer_set_text(condition, "Loading..");
  
  
  //Formatting text
  text_layer_set_font(clocks, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(clocks, GTextAlignmentCenter);

  text_layer_set_font(day, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text_alignment(day, GTextAlignmentRight);
  
  //Insert the text_layers
  layer_add_child(window_get_root_layer(weather), text_layer_get_layer(clocks));
  layer_add_child(window_get_root_layer(weather), text_layer_get_layer(day));
  layer_add_child(window_get_root_layer(weather), text_layer_get_layer(condition));
  
  update_weather_time();
}

static void weather_unload(Window *weather) {
  
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  window = window_create();
  window_set_background_color(window, GColorBlack);

  weather = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_set_window_handlers(weather, (WindowHandlers) {
    .load = weather_load,
    .unload = weather_unload
  });


  window_set_fullscreen(window, true);
  window_set_fullscreen(weather, true);

  // Show the Window on the watch, with animated=true
  window_stack_push(window, true);
  
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Subscribe to the accelerometer data service
  // Choose update rate
   int num_samples = 3;
    accel_data_service_subscribe(num_samples, data_handler);

    // Choose update rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);}

static void deinit() {
  // Destroy Window
  window_destroy(window);
  window_destroy(weather);
  // Subscribe to the accelerometer data service
  // Choose update rate
  accel_data_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();

}
