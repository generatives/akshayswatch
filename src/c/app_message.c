#include <pebble.h>
static Window *s_window;	
	
// Keys for AppMessage Dictionary
// These should correspond to the values you defined in appinfo.json/Settings
enum {
	NOTIFY_1 = 0,
	NOTIFY_2 = 1,
	NOTIFY_3 = 2,
	NOTIFY_4 = 3,
	EVENT = 4,
};
uint32_t patterns[][10] = {
  {250},
  {150, 75, 150},
  {150, 75, 150, 75, 150},
  {150, 75, 150, 75, 150, 75, 150},
  {500, 200, 500, 200, 500, 200, 500},
};
static void vibrate(int eventType) {
  if(eventType < (int)ARRAY_LENGTH(patterns)) {
    VibePattern pat = {
      .durations = patterns[eventType],
      .num_segments = ARRAY_LENGTH(patterns[eventType]),
    };
    vibes_enqueue_custom_pattern(pat);
  }
}
// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
    Tuple *vibrationTuple = dict_find(received, 0);
    if(vibrationTuple) {
      int32_t index = vibrationTuple->value->int32;
      vibrate(index);
    }
}
// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}
// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}
int count = 0;
void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  vibrate(count);
  count++;
  if(count >= 5) {
    count = 0;
  }
}
void config_provider(Window *window) {
 // single click / repeat-on-hold config:
  window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
}
static void init(void) {
	s_window = window_create();
	window_stack_push(s_window, true);
	
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
  // Initialize AppMessage inbox and outbox buffers with a suitable size
  const int inbox_size = 128;
  const int outbox_size = 128;
	app_message_open(inbox_size, outbox_size);
  
  //window_set_click_config_provider(s_window, (ClickConfigProvider) config_provider);
}
static void deinit(void) {
	app_message_deregister_callbacks();
	window_destroy(s_window);
}
int main( void ) {
	init();
	app_event_loop();
	deinit();
}
