#include <pebble.h>

/* Layout constants */
#if defined(PBL_PLATFORM_CHALK)
 #define PBL_X 180
 #define PBL_Y 180

 #define BASE_Y_SMALL 51
 #define BASE_X_SMALL 110
 #define LINE_HEIGHT_SMALL 18
 #define LAYER_Y_SMALL 23

 #define BASE_X_LARGE 30
 #define BASE_Y_LARGE 20
 #define LINE_HEIGHT_LARGE 55
 #define TIME_PADDING 8
#else
 #define PBL_X 144
 #define PBL_Y 168

 #define BASE_Y_SMALL 46
 #define BASE_X_SMALL 90
 #define LINE_HEIGHT_SMALL 18
 #define LAYER_Y_SMALL 23

 #define BASE_X_LARGE 10
 #define BASE_Y_LARGE 15
 #define LINE_HEIGHT_LARGE 55
 #define TIME_PADDING 8
#endif

/* App version */
#define APP_VERSION "1.5"

/* Percentage to change battery indicator color */
#define BATTERY_LOW 30

/* Leave for production code. Comment BOTH for debugging */
#undef APP_LOG
#define APP_LOG(...) 

static Window* s_main_window;
static TextLayer* s_hour_layer;
static TextLayer* s_minute_layer;
static TextLayer* s_date_layer_1;
static TextLayer* s_date_layer_2;
static TextLayer* s_step_layer;
static TextLayer* s_battery_layer;
static TextLayer* s_bt_layer;
static int battery_level;
static GFont s_time_font;
static GFont s_text_font;
static char hour12_fmt_string[8];
static char hour24_fmt_string[8];

static struct {
	char lang[8];
	char theme[8];
	bool leading_zero;
	bool bt_alarm;
	bool bt_vibrate;
} settings;

static struct {
	GColor color_front;
	GColor color_back;
	GColor color_debug;
	GColor color_steps;
	GColor color_battery;
	GColor color_battery_low;
	GColor color_date;
} theme;

/* New variables MUST BE ADDED BELOW all the existent ones, so the 
 * corresponding ones in all versions will be consistent */
static enum{
	K_VERSION = 0,
	K_LANG,
	K_THEME,
	K_BT_ALARM,
	K_LEADING_ZERO
} keys;

/* Set hour format according to the settings */
static void update_hour_format(){
	if(settings.leading_zero){
		strcpy(hour12_fmt_string,"%I");
		strcpy(hour24_fmt_string,"%H");
	}
	else{
		strcpy(hour12_fmt_string,"%l");
		strcpy(hour24_fmt_string,"%k");
	}
}

/* Function to update the time layer with the current time.
 * TODO: Implement update_hour and update_minute to save battery */
static void update_time() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering update_time");
	/* Get a tm structure */
	time_t temp = time(NULL);
	struct tm* tick_time = localtime(&temp);

	/* Write the current hours and minutes into a buffer */
	static char s_hour[8];
	static char s_minute[8];
	strftime(s_hour, sizeof(s_hour), clock_is_24h_style() ? hour24_fmt_string : hour12_fmt_string, tick_time);
	strftime(s_minute, sizeof(s_minute), "%M", tick_time);

	/* Display this time on the TextLayer */
	text_layer_set_text(s_hour_layer, s_hour);
	text_layer_set_text(s_minute_layer, s_minute);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting update_time");
}

/* Function to update the date layer with the current date */
static void update_date() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering update_date");
	/* Get a tm structure */
	time_t temp = time(NULL);
	struct tm* tick_time = localtime(&temp);

	/* Write the current date into a buffer */
	static char s_date_buffer_1[8];
	static char s_date_buffer_2[8];

	strftime(s_date_buffer_1, sizeof(s_date_buffer_1), "%a", tick_time);
	strftime(s_date_buffer_2, sizeof(s_date_buffer_2), "%d", tick_time);

	if(strcmp(settings.lang, "spa") == 0){
		if (strcmp(s_date_buffer_1, "Mon") == 0){
		strcpy(s_date_buffer_1, "LUN");
		}else if (strcmp(s_date_buffer_1, "Tue") == 0){
			strcpy(s_date_buffer_1, "MAR");
		}else if (strcmp(s_date_buffer_1, "Wed") == 0){
			strcpy(s_date_buffer_1, "MIE");
		}else if (strcmp(s_date_buffer_1, "Thu") == 0){
			strcpy(s_date_buffer_1, "JUE");
		}else if (strcmp(s_date_buffer_1, "Fri") == 0){
			strcpy(s_date_buffer_1, "VIE");
		}else if (strcmp(s_date_buffer_1, "Sat") == 0){
			strcpy(s_date_buffer_1, "SAB");
		}else if (strcmp(s_date_buffer_1, "Sun") == 0){
			strcpy(s_date_buffer_1, "DOM");
		}
	}
	else if (strcmp(settings.lang, "fr") == 0){
		if (strcmp(s_date_buffer_1, "Mon") == 0){
		strcpy(s_date_buffer_1, "LUN");
		}else if (strcmp(s_date_buffer_1, "Tue") == 0){
			strcpy(s_date_buffer_1, "MAR");
		}else if (strcmp(s_date_buffer_1, "Wed") == 0){
			strcpy(s_date_buffer_1, "MER");
		}else if (strcmp(s_date_buffer_1, "Thu") == 0){
			strcpy(s_date_buffer_1, "JEU");
		}else if (strcmp(s_date_buffer_1, "Fri") == 0){
			strcpy(s_date_buffer_1, "VEN");
		}else if (strcmp(s_date_buffer_1, "Sat") == 0){
			strcpy(s_date_buffer_1, "SAM");
		}else if (strcmp(s_date_buffer_1, "Sun") == 0){
			strcpy(s_date_buffer_1, "DIM");
		}
	} 
	else if (strcmp(settings.lang, "ger") == 0){
		if (strcmp(s_date_buffer_1, "Mon") == 0){
		strcpy(s_date_buffer_1, "MO");
		}else if (strcmp(s_date_buffer_1, "Tue") == 0){
			strcpy(s_date_buffer_1, "DI");
		}else if (strcmp(s_date_buffer_1, "Wed") == 0){
			strcpy(s_date_buffer_1, "MI");
		}else if (strcmp(s_date_buffer_1, "Thu") == 0){
			strcpy(s_date_buffer_1, "DO");
		}else if (strcmp(s_date_buffer_1, "Fri") == 0){
			strcpy(s_date_buffer_1, "FR");
		}else if (strcmp(s_date_buffer_1, "Sat") == 0){
			strcpy(s_date_buffer_1, "SA");
		}else if (strcmp(s_date_buffer_1, "Sun") == 0){
			strcpy(s_date_buffer_1, "SO");
		}
	} 
	else if (strcmp(settings.lang, "ita") == 0){
		if (strcmp(s_date_buffer_1, "Mon") == 0){
		strcpy(s_date_buffer_1, "LUN");
		}else if (strcmp(s_date_buffer_1, "Tue") == 0){
			strcpy(s_date_buffer_1, "MAR");
		}else if (strcmp(s_date_buffer_1, "Wed") == 0){
			strcpy(s_date_buffer_1, "MER");
		}else if (strcmp(s_date_buffer_1, "Thu") == 0){
			strcpy(s_date_buffer_1, "GIO");
		}else if (strcmp(s_date_buffer_1, "Fri") == 0){
			strcpy(s_date_buffer_1, "VEN");
		}else if (strcmp(s_date_buffer_1, "Sat") == 0){
			strcpy(s_date_buffer_1, "SAB");
		}else if (strcmp(s_date_buffer_1, "Sun") == 0){
			strcpy(s_date_buffer_1, "DOM");
		}
	}
	else{
		if (strcmp(s_date_buffer_1, "Mon") == 0){
		strcpy(s_date_buffer_1, "MON");
		}else if (strcmp(s_date_buffer_1, "Tue") == 0){
			strcpy(s_date_buffer_1, "TUE");
		}else if (strcmp(s_date_buffer_1, "Wed") == 0){
			strcpy(s_date_buffer_1, "WED");
		}else if (strcmp(s_date_buffer_1, "Thu") == 0){
			strcpy(s_date_buffer_1, "THU");
		}else if (strcmp(s_date_buffer_1, "Fri") == 0){
			strcpy(s_date_buffer_1, "FRI");
		}else if (strcmp(s_date_buffer_1, "Sat") == 0){
			strcpy(s_date_buffer_1, "SAT");
		}else if (strcmp(s_date_buffer_1, "Sun") == 0){
			strcpy(s_date_buffer_1, "SUN");
		}
	}

	/* Display date on the TextLayer */
	text_layer_set_text(s_date_layer_1, s_date_buffer_1);
	text_layer_set_text(s_date_layer_2, s_date_buffer_2);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting update_date");
}

/* Function to update the step layer with the current steps from Pebble Health */
static void update_step() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering update_step");
	int step_count;
	static char s_step_buffer[16];

	step_count = (int)health_service_sum_today(HealthMetricStepCount);
	/* Write the current steps into a buffer */
	if(step_count > 9999){
		snprintf(s_step_buffer, sizeof(s_step_buffer), "%dK", (int)step_count/1000);
	}else{
		snprintf(s_step_buffer, sizeof(s_step_buffer), "%d.%1dK", (int)step_count/1000, (step_count%1000)/100);
	}

	/* Display steps on the TextLayer */
	text_layer_set_text(s_step_layer, s_step_buffer);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting update_step");
}

/* Function to update theme colors according to settings */
static void update_theme() {
	if(strcmp(settings.theme,"light") == 0){
		theme.color_front   = GColorBlack;
		theme.color_back    = GColorWhite;
		theme.color_debug   = GColorVividCerulean;
		theme.color_steps   = GColorWindsorTan;
		theme.color_battery = GColorDarkGreen;
		theme.color_battery_low = GColorDarkCandyAppleRed;
		theme.color_date    = GColorBlack;
	}
	else{
		theme.color_front   = GColorWhite;
		theme.color_back    = GColorBlack;
		theme.color_debug   = GColorVividCerulean;
		theme.color_steps   = GColorChromeYellow;
		theme.color_battery = GColorBrightGreen;
		theme.color_battery_low = GColorRed;
		theme.color_date    = GColorWhite;
	}

	window_set_background_color(s_main_window, theme.color_back);

	text_layer_set_background_color(s_hour_layer, theme.color_back);
	text_layer_set_text_color(s_hour_layer, theme.color_front);

	text_layer_set_background_color(s_minute_layer, theme.color_back);
	text_layer_set_text_color(s_minute_layer, theme.color_front);

	text_layer_set_background_color(s_date_layer_1, theme.color_back);
	text_layer_set_text_color(s_date_layer_1, theme.color_date);

	text_layer_set_background_color(s_date_layer_2, theme.color_back);
	text_layer_set_text_color(s_date_layer_2, theme.color_date);

	text_layer_set_background_color(s_step_layer, theme.color_back);
	text_layer_set_text_color(s_step_layer, theme.color_steps);

	text_layer_set_background_color(s_battery_layer, theme.color_back);
	text_layer_set_text_color(s_battery_layer, theme.color_battery);

	text_layer_set_background_color(s_bt_layer, theme.color_back);
	text_layer_set_text_color(s_bt_layer, theme.color_front);
}

/* Function to set layout of the different layers */
void update_layout() {
	/* Get information about the Window */
	Layer* window_layer = window_get_root_layer(s_main_window);
	GRect bounds = layer_get_bounds(window_layer);

	/* Create the TextLayer with specific bounds. Only valid for Pebble/Pebble Time */
	s_hour_layer = text_layer_create(
		GRect(BASE_X_LARGE, BASE_Y_LARGE, bounds.size.w/2+TIME_PADDING, 72));
	s_minute_layer = text_layer_create(
		GRect(BASE_X_LARGE, BASE_Y_LARGE + LINE_HEIGHT_LARGE, bounds.size.w/2+TIME_PADDING, 72));
	s_date_layer_1 = text_layer_create(
		GRect(BASE_X_SMALL, BASE_Y_SMALL, PBL_X-BASE_X_SMALL, LAYER_Y_SMALL));
	s_date_layer_2 = text_layer_create(
		GRect(BASE_X_SMALL, BASE_Y_SMALL + LINE_HEIGHT_SMALL*1, PBL_X-BASE_X_SMALL, LAYER_Y_SMALL));
	s_step_layer = text_layer_create(
		GRect(BASE_X_SMALL, BASE_Y_SMALL + LINE_HEIGHT_SMALL*2, PBL_X-BASE_X_SMALL, LAYER_Y_SMALL));
	s_bt_layer = text_layer_create(
		GRect(bounds.size.w*0.33+2, 0, bounds.size.w*0.33-5, 30));
	#if defined(PBL_HEALTH)

		s_battery_layer = text_layer_create(
		GRect(BASE_X_SMALL, BASE_Y_SMALL + LINE_HEIGHT_SMALL*3, PBL_X-BASE_X_SMALL, LAYER_Y_SMALL));
	#else
		s_battery_layer = text_layer_create(
		GRect(BASE_X_SMALL, BASE_Y_SMALL + LINE_HEIGHT_SMALL*2, PBL_X-BASE_X_SMALL, LAYER_Y_SMALL));
	#endif

	/* Create GFont */
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_HEL_LIG_70));
	s_text_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_HEL_LIG_21));

	/* Layout for hour layer */
	text_layer_set_font(s_hour_layer, s_time_font);
	text_layer_set_text_alignment(s_hour_layer, GTextAlignmentRight);

	/* Layout for minute layer */
	text_layer_set_font(s_minute_layer, s_time_font);
	text_layer_set_text_alignment(s_minute_layer, GTextAlignmentRight);

	/* Layout for date layer 1*/
	text_layer_set_font(s_date_layer_1, s_text_font);
	text_layer_set_text_alignment(s_date_layer_1, GTextAlignmentLeft);

	/* Layout for date layer 2*/
	text_layer_set_font(s_date_layer_2, s_text_font);
	text_layer_set_text_alignment(s_date_layer_2, GTextAlignmentLeft);

	/* Layout for step layer */
	text_layer_set_font(s_step_layer, s_text_font);
	text_layer_set_text_alignment(s_step_layer, GTextAlignmentLeft);

	/* Layout for battery layer */
	text_layer_set_font(s_battery_layer, s_text_font);
	text_layer_set_text_alignment(s_battery_layer, GTextAlignmentLeft);

	/* Layout for bluetooth connection layer */
	text_layer_set_font(s_bt_layer, s_text_font);
	text_layer_set_text_alignment(s_bt_layer, GTextAlignmentCenter);
}

/* Function called every minute by TickTimerService */
static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
	if(units_changed & MINUTE_UNIT){
		update_time();
		#if defined(PBL_HEALTH)
			update_step();
		#endif
	}
	if(units_changed & DAY_UNIT){
		update_date();
	}
}

/* Function called when battery state changes */
static void battery_handler() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering battery_handler");
	BatteryChargeState battery_info;
	static char s_battery[8];

	battery_info = battery_state_service_peek();
	battery_level = battery_info.charge_percent;

	#if defined(PBL_COLOR)
	if(battery_level > BATTERY_LOW){
		text_layer_set_text_color(s_battery_layer, theme.color_battery);
	}else{
		text_layer_set_text_color(s_battery_layer, theme.color_battery_low);
	}
	#endif
	
	if(battery_info.is_charging) {
		snprintf(s_battery, sizeof(s_battery), "CHR");
	}
	else {
		if(battery_level == 100){
			/* Fix 100% battery issue when it is at 100% in pebble & pebble time */
			#if defined(PBL_PLATFORM_CHALK)
			snprintf(s_battery, sizeof(s_battery), "%d%%", battery_level);
			#else
			snprintf(s_battery, sizeof(s_battery), "%d", battery_level);
			#endif
		}
		else {
			snprintf(s_battery, sizeof(s_battery), "%d%%", battery_level);
		}
	}

	/* Display battery status on the TextLayer */
	text_layer_set_text(s_battery_layer, s_battery);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting battery_handler");
}

/* Function called when bluetooth connection status changes */
static void bt_handler(bool connected) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering bt_handler");
	bool bt_connected;
	static char s_bt[8];

	bt_connected = connected;

	if(!bt_connected) {
		if(settings.bt_alarm){
			snprintf(s_bt, sizeof(s_bt), "BT");
		}
		else{
			snprintf(s_bt, sizeof(s_bt), "  ");
		}
		if(settings.bt_vibrate){
			vibes_short_pulse();
		}
	}
	else {
		snprintf(s_bt, sizeof(s_bt), "  ");
		if(settings.bt_vibrate){
			vibes_double_pulse();
		}
	}

	/* Display connection status on the TextLayer */
	text_layer_set_text(s_bt_layer, s_bt);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting bt_handler");
}

/* Function called when user change settings on the companion app */
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering settings_handler");
	Tuple* set_language = dict_find(iter, MESSAGE_KEY_language);
	if(set_language) {
		strncpy(settings.lang, set_language->value->cstring, sizeof(settings.lang));
		update_date();
		persist_write_string(K_LANG, settings.lang);
	}

	Tuple* set_theme = dict_find(iter, MESSAGE_KEY_theme);
	if(set_theme) {
		strncpy(settings.theme, set_theme->value->cstring, sizeof(settings.theme));
		update_theme();
		persist_write_string(K_THEME, settings.theme);
	}

	Tuple* set_bt = dict_find(iter, MESSAGE_KEY_bt_alarm);
	if(set_bt) {
		settings.bt_alarm = set_bt->value->int32 == 1;
		persist_write_bool(K_BT_ALARM, settings.bt_alarm);
	}

	Tuple* set_leading_zero = dict_find(iter, MESSAGE_KEY_leading_zero);
	if(set_leading_zero) {
        settings.leading_zero = set_leading_zero->value->int32 == 1;
		persist_write_bool(K_LEADING_ZERO, settings.leading_zero);
		update_hour_format();
		update_time();
	}
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting settings_handler");
}

/* Function to open a connection with AppMesage to listen data from Clay */
static void prv_init(void) {
	app_message_register_inbox_received(prv_inbox_received_handler);
	app_message_open(128, 128);
}

/* Defaults settings for the watchface */
static void load_defaults(void) {
	/* Check first if there is a previous config in localStorage */
	if(persist_exists(K_VERSION)){
		char installed_version[8];
		persist_read_string(K_VERSION, installed_version, sizeof(installed_version));
		if(strcmp(installed_version, APP_VERSION ) != 0){
			/* Routine to update local storage if necessary. Need to be
			 * updated in each version */

			/* Once updated, update the version */
			persist_write_string(K_VERSION, installed_version);
		}
	}
	if(persist_exists(K_THEME)){
		persist_read_string(K_THEME, settings.theme, sizeof(settings.theme));
	}
	else{
		strcpy(settings.theme, "dark");
	}

	if(persist_exists(K_LANG)){
		persist_read_string(K_LANG, settings.lang, sizeof(settings.lang));
	}
	else{
		strcpy(settings.lang, "eng");
	}
	
	if(persist_exists(K_BT_ALARM)){
		settings.bt_alarm = persist_read_bool(K_BT_ALARM);
	}
	else{
		settings.bt_alarm = false;
	}
	
	if(persist_exists(K_LEADING_ZERO)){
		settings.leading_zero = persist_read_bool(K_LEADING_ZERO);
	}
	else{
		settings.leading_zero = true;
	}
	settings.bt_vibrate = false;
}

static void main_window_load(Window* window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering main_window_load");
	Layer* window_layer = window_get_root_layer(s_main_window);

	/* Set layout */
	update_layout();

	/* Set colors according to the theme */
	update_theme();
	
	/* Set hour format according to the settings */
	update_hour_format();

	/* Add the layers as a child layers to the Window's root layer. 
	 * The order here is important to control clipping */
	layer_add_child(window_layer, text_layer_get_layer(s_minute_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_hour_layer));	
	layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));
	#if defined(PBL_HEALTH)
	layer_add_child(window_layer, text_layer_get_layer(s_step_layer));
	#endif
	layer_add_child(window_layer, text_layer_get_layer(s_date_layer_2));
	layer_add_child(window_layer, text_layer_get_layer(s_date_layer_1));
	layer_add_child(window_layer, text_layer_get_layer(s_bt_layer));

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting main_window_load");
}

static void main_window_unload(Window* window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering main_window_unload");
	/* Destroy text layers on unload */
	text_layer_destroy(s_hour_layer);
	text_layer_destroy(s_minute_layer);
	text_layer_destroy(s_date_layer_1);
	text_layer_destroy(s_date_layer_2);
	text_layer_destroy(s_step_layer);
	text_layer_destroy(s_battery_layer);
	text_layer_destroy(s_bt_layer);

	/* Destroy text font */
	fonts_unload_custom_font(s_time_font);
	fonts_unload_custom_font(s_text_font);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting main_window_unload");
}

static void init() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering init");

	/* Load default theme, language, etc. */
	load_defaults();
	
	/* Suscribe to AppMessage to receive inputs from the user */
	prv_init();
	
	/* Register with TickTimerService once a minute to change time. 
	 * IMPORTANT: Apparently ONLY ONE TickTimerService can be active
	 * at a time. Newer calls to this function will overwrite this
	 * listener! */
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

	/* Register with BatteryStateService */
	battery_state_service_subscribe(battery_handler); 

	/* Register with ConnectionService */
	connection_service_subscribe((ConnectionHandlers) {
		.pebble_app_connection_handler = bt_handler
	});

	/* Create main window element and assign to pointer */
	s_main_window = window_create();

	/* Set handlers to manage the elements inside the window */
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	/* Show the Window on the watch, with animated=true */
	window_stack_push(s_main_window, true);

	/* Show the current data from the beggining */
	update_time();
	update_date();
	#if defined(PBL_HEALTH)
		update_step();
	#endif
	battery_handler();
	bt_handler(connection_service_peek_pebble_app_connection());
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting init");
}

static void deinit() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering deinit");
	window_destroy(s_main_window);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Exiting deinit");
}


int main(void) {
	init();
	settings.bt_vibrate = true; /* This avoid vibration every time we load the watchface */
	app_event_loop();
	deinit();
}
