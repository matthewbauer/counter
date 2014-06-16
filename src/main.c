#include <pebble.h>

#define COUNT_PKEY 1
#define COUNT_DEFAULT 0
#define REPEAT_INTERVAL 50
	
Window *window;
TextLayer *text_layer;

GBitmap *action_icon_plus;
GBitmap *action_icon_minus;
GBitmap *action_icon_reset;

ActionBarLayer *action_bar;

static int count = COUNT_DEFAULT;

void update() {
	static char buffer[10];
	snprintf(buffer, sizeof(buffer), "%i", count);
	text_layer_set_text(text_layer, buffer);
}

void increment() {
	count++;
}

void decrement() {
	count--;
}

void reset() {
	count = 0;
}

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	increment();
	update();
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	decrement();
	update();
}

void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	reset();
	update();
}

void click_config_provider(void *context) {
	window_single_repeating_click_subscribe(BUTTON_ID_SELECT, REPEAT_INTERVAL, (ClickHandler) select_click_handler);
	window_single_repeating_click_subscribe(BUTTON_ID_UP, REPEAT_INTERVAL, (ClickHandler) up_click_handler);
	window_single_repeating_click_subscribe(BUTTON_ID_DOWN, REPEAT_INTERVAL, (ClickHandler) down_click_handler);
}

void window_load(Window *window) {
	action_bar = action_bar_layer_create();
	action_bar_layer_add_to_window(action_bar, window);
	action_bar_layer_set_click_config_provider(action_bar, click_config_provider);

	action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, action_icon_reset);
	action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, action_icon_plus);
	action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, action_icon_minus);

	Layer *layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(layer);
	const int16_t width = layer_get_frame(layer).size.w - ACTION_BAR_WIDTH - 3;

	text_layer = text_layer_create(GRect(0, 44, width, bounds.size.h));
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_background_color(text_layer, GColorClear);
	update();
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	layer_add_child(layer, text_layer_get_layer(text_layer));
}

void window_unload(Window *window) {
	text_layer_destroy(text_layer);
	action_bar_layer_destroy(action_bar);
}

void init() {
	action_icon_plus = gbitmap_create_with_resource(RESOURCE_ID_PLUS_WHITE);
	action_icon_minus = gbitmap_create_with_resource(RESOURCE_ID_MINUS_WHITE);
	action_icon_reset = gbitmap_create_with_resource(RESOURCE_ID_RESET_WHITE);

	window = window_create();

	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});

	count = persist_exists(COUNT_PKEY) ? persist_read_int(COUNT_PKEY) : COUNT_DEFAULT;

	window_stack_push(window, true);
}

void deinit() {
	persist_write_int(COUNT_PKEY, count);
	window_destroy(window);
	gbitmap_destroy(action_icon_plus);
	gbitmap_destroy(action_icon_minus);
	gbitmap_destroy(action_icon_reset);
}

int main() {
	init();
	app_event_loop();
	deinit();
}
