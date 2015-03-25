#include <pebble.h>

static Window *window;
static TextLayer *weight_layer;
static TextLayer *device_name_layer;
static int device_number = 0;

static char* devices[3] = {"A1", "B1", "A2"};
static char* weights[3] = {"30 kg", "69 kg", "50 kg"};


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  device_number++;
  if (device_number > 2) device_number = 0;
  text_layer_set_text(device_name_layer, devices[device_number]);
  text_layer_set_text(weight_layer, weights[device_number]);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  device_number--;
  if (device_number < 0) device_number = 2;
  text_layer_set_text(device_name_layer, devices[device_number]);
  text_layer_set_text(weight_layer, weights[device_number]);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  device_name_layer = text_layer_create((GRect) { .origin = { 0, 20 }, .size = { bounds.size.w, 28 } });
    weight_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });

  text_layer_set_font(device_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text(weight_layer, weights[0]);
  text_layer_set_text(device_name_layer, devices[0]);
  text_layer_set_text_alignment(weight_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(device_name_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(weight_layer));
  layer_add_child(window_layer, text_layer_get_layer(device_name_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(weight_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}