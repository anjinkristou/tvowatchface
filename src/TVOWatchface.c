#include <pebble.h>
#include <pebble_fonts.h>

static Window       *window;
static TextLayer    *text_layer;
static TextLayer    *now_playing_text_layer;
static TextLayer    *show_title_text_layer;
static GBitmap      *background;
static BitmapLayer  *background_layer;
static AppTimer     *timer;

/**
 * Button Events
 */
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    //
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    //
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    //
}

static void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

/**
 * AppMessage Events
 */
static void update_nowplaying() {
    layer_set_hidden(text_layer_get_layer(now_playing_text_layer), false);
    layer_set_hidden(text_layer_get_layer(show_title_text_layer), false);
    // Poke JS to let it know we're ready for an update.
    Tuplet poke_tuplet = TupletInteger(1,1);
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    if (iter == NULL) {
        return;
    }
    dict_write_tuplet(iter, &poke_tuplet);
    dict_write_end(iter);
    app_message_outbox_send();
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
    update_nowplaying();
}

static void app_timer_handler(void *data) {
    layer_set_hidden(text_layer_get_layer(now_playing_text_layer), true);
    layer_set_hidden(text_layer_get_layer(show_title_text_layer), true);
}

static void app_message_received_handler(DictionaryIterator *iter, void *context) {
    Tuple *message_string = dict_find(iter, 0);
    if (message_string) {
        text_layer_set_text(show_title_text_layer, message_string->value->cstring);
    }
    timer = app_timer_register(1500, app_timer_handler, NULL);
}

/**
 * Time Events
 */
static void update_time(struct tm *t) {
    static char time_text[] = "00:00 AM";

    char *format_str = clock_is_24h_style() ? "%H:%M" : "%I:%M %p";

    strftime(time_text, sizeof(time_text), format_str, t);
    text_layer_set_text(text_layer, time_text);
}

static void minute_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time(tick_time);
}

/**
 * App Lifecycle
 */
static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // Add the GBitmap
    background = gbitmap_create_with_resource(RESOURCE_ID_LOGO);
    background_layer = bitmap_layer_create(layer_get_frame(window_layer));
    bitmap_layer_set_bitmap(background_layer, background);
    layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));

    // Add the Text
    text_layer = text_layer_create((GRect) { .origin = { 0, 130 }, .size = { bounds.size.w, 28 } });
    text_layer_set_background_color(text_layer, GColorClear);
    text_layer_set_text_color(text_layer, GColorWhite);
    text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text(text_layer, "00:00");
    text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(text_layer));

    // Add the now_playing Text
    now_playing_text_layer = text_layer_create((GRect){ .origin = {0,0}, .size = {bounds.size.w, 40} });
    text_layer_set_background_color(now_playing_text_layer, GColorBlack);
    text_layer_set_text_color(now_playing_text_layer, GColorWhite);
    text_layer_set_font(now_playing_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text(now_playing_text_layer, " Now Playing ...");
    layer_add_child(window_layer, text_layer_get_layer(now_playing_text_layer));
    
    show_title_text_layer = text_layer_create((GRect){ .origin = {0,40}, .size = {bounds.size.w, 88} });
    text_layer_set_font(show_title_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text(show_title_text_layer, "...");
    text_layer_set_text_alignment(show_title_text_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(show_title_text_layer));

    // Hide the two layers
    layer_set_hidden(text_layer_get_layer(now_playing_text_layer), true);
    layer_set_hidden(text_layer_get_layer(show_title_text_layer), true);
}

static void window_unload(Window *window) {
    text_layer_destroy(text_layer);
    text_layer_destroy(now_playing_text_layer);
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

    // Force an update to time before subscribing to the MINUTE tick.
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    update_time(t);

    tick_timer_service_subscribe(MINUTE_UNIT, &minute_tick_handler);

    // Register app_message handlers
    accel_tap_service_subscribe(&tap_handler);
    app_message_register_inbox_received(&app_message_received_handler);
    app_message_open(64,64);
}

static void deinit(void) {
    bitmap_layer_destroy(background_layer);
    gbitmap_destroy(background);
    text_layer_destroy(text_layer);
    text_layer_destroy(now_playing_text_layer);
    accel_tap_service_unsubscribe();
    window_destroy(window);
}

int main(void) {
    init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

    app_event_loop();
    deinit();
}
