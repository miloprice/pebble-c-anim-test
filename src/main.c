#include <pebble.h>

Window *window;
TextLayer *text_layer;
char buffer[] = "00:00";
InverterLayer *inv_layer;	//Inverter layer
GBitmap *future_bitmap, *past_bitmap;
BitmapLayer *future_layer, *past_layer;

void tick_handler(struct tm *tick_time, TimeUnits units_changed){
	//Format the buffer string using tick_time as the time source
	strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
	
	//Change the TextLayer text to show the new time
	text_layer_set_text(text_layer, buffer);
}

void window_load(Window *window)
{
	//Load font
	ResHandle font_handle = resource_get_handle(RESOURCE_ID_IMAGINE_42);
	
	text_layer = text_layer_create(GRect(0, 53, 144, 168));
	text_layer_set_background_color(text_layer, GColorClear);
	text_layer_set_text_color(text_layer, GColorBlack);
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	text_layer_set_font(text_layer, fonts_load_custom_font(font_handle));
	
	layer_add_child(window_get_root_layer(window), (Layer*) text_layer);
	
	//Arbitrary text:
	text_layer_set_text(text_layer, "Radscorpion sighted");	
	
	//Inverter layer
	inv_layer = inverter_layer_create(GRect(0,50,144,62));
	layer_add_child(window_get_root_layer(window), (Layer*) inv_layer);
	
	//Get time structure so face doesn't start blank
	struct tm *t;
	time_t temp;
	temp = time(NULL);
	t = localtime(&temp);
	
	//Manually call tick handler when window is loading
	tick_handler(t, MINUTE_UNIT);
	
	//Load bitmaps into GBitmap structures
	//The ID you chose when uploading is prefixed with 'RESOURCE_ID_'
	future_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FUTURE_ARROW);
	past_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PAST_ARROW);
	 
	//Create BitmapLayers to show GBitmaps and add to Window
	//Sample images are 144 x 50 pixels
	future_layer = bitmap_layer_create(GRect(0, 0, 144, 50));
	bitmap_layer_set_bitmap(future_layer, future_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(future_layer));
	 
	past_layer = bitmap_layer_create(GRect(0, 112, 144, 50));
	bitmap_layer_set_bitmap(past_layer, past_bitmap);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(past_layer));
}

void window_unload(Window *window)
{
	text_layer_destroy(text_layer);
	
	inverter_layer_destroy(inv_layer);
}


	
void handle_init(void) {
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	
	window_stack_push(window, true);
	
	tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tick_handler);
}

void handle_deinit(void) {
	window_destroy(window);
	tick_timer_service_unsubscribe();
	//Destroy GBitmaps
	gbitmap_destroy(future_bitmap);
	gbitmap_destroy(past_bitmap);
	 
	//Destroy BitmapLayers
	bitmap_layer_destroy(future_layer);
	bitmap_layer_destroy(past_layer);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
