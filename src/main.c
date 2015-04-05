#include <pebble.h>

static Window *window;
static TextLayer *weight_layer;
static TextLayer *device_name_layer;
static int device_number = 0;
static int standard_weight_number = 0;
static bool weight_change_mode = false;

static char* devices[3] = {"A1", "B1", "A2"};
static int weights[3] = {2, 5, 6};
static int standard_weights[7] = {10, 20, 30, 40, 50, 60, 70};

static char s_buffer_weight[64];

static void display_device(int number) {
  text_layer_set_text(device_name_layer, devices[number]);
  snprintf(s_buffer_weight, sizeof(s_buffer_weight), "%d kg", standard_weights[weights[number]]);
  text_layer_set_text(weight_layer, s_buffer_weight);
}

static void display_standard_weight(int number) {
  snprintf(s_buffer_weight, sizeof(s_buffer_weight), "%d kg", standard_weights[number]);
  text_layer_set_text(weight_layer, s_buffer_weight);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (weight_change_mode) {
    persist_write_int(device_number, standard_weight_number);
    weights[device_number] = standard_weight_number;
    snprintf(s_buffer_weight, sizeof(s_buffer_weight), "%d kg", standard_weights[weights[device_number]]);
    text_layer_set_text_alignment(weight_layer, GTextAlignmentCenter);
    weight_change_mode = false;
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (weight_change_mode) {
    standard_weight_number++;
    if (standard_weight_number > 6) standard_weight_number = 0;
    display_standard_weight(standard_weight_number);
  } else {
    device_number++;
    if (device_number > 2) device_number = 0;
    display_device(device_number);
  }
}


static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (weight_change_mode) {
    standard_weight_number--;
    if (standard_weight_number < 0) standard_weight_number = 6;
    display_standard_weight(standard_weight_number);
  } else {
    device_number--;
    if (device_number < 0) device_number = 2;
    display_device(device_number);
  }
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  weight_change_mode = true;
  standard_weight_number = weights[device_number];
  text_layer_set_text_alignment(weight_layer, GTextAlignmentLeft);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, select_long_click_handler, NULL);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  device_name_layer = text_layer_create((GRect) { .origin = { 0, 20 }, .size = { bounds.size.w, 28 } });
    weight_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });

  text_layer_set_font(device_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  snprintf(s_buffer_weight, sizeof(s_buffer_weight), "%d kg", standard_weights[weights[0]]);
  text_layer_set_text(weight_layer, s_buffer_weight);
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
  for (int i = 0; i < 3; i++) {
    if (persist_exists(i)) {
      weights[i] = persist_read_int(i);
    }
  }
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