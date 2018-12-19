#include <allegro5/allegro.h> 
#include <allegro5/allegro_native_dialog.h> 
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include<allegro5\allegro_audio.h>
#include <allegro5/allegro_color.h>
#include<allegro5\fullscreen_mode.h>
#include<stdio.h>



//global Variables
bool end = false, redraw = true;//end:ending the main loop
float height, width;//value of current window pannel
float max_height, max_width;//maximum values of screen
//int pieces[10][10] = { 0 }; test for phase 2
ALLEGRO_BITMAP *bg_image;
//ALLEGRO_BITMAP *nuts_images[10][10]; test for phase 2
//enum player_turn{black=-1,white=1}player; test for phase 2



void init_primitive() {
	al_init_primitives_addon();
}
int init_mouse() {
	if (!al_install_mouse())
		return -1;
	return 0;
}
void init_bitmap() {
	al_init_image_addon();
}
int init_display(float waittime) {

	if (!al_init())
	{
		al_show_native_message_box(al_get_current_display(), "Error", "Could not init", "Failed to initialize allegro!\n", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	al_set_new_display_flags(ALLEGRO_RESIZABLE);//set windows resizable
	
	ALLEGRO_DISPLAY* display = al_create_display(800, 800);
	al_set_window_constraints(display, 500, 500, max_width, max_height);//bound the window
	al_apply_window_constraints(display, true);//apply constraints
	al_clear_to_color(al_map_rgb(50, 0, 0));
	al_flip_display();
	//set height and width globall values
	height = al_get_display_height(display);
	width = al_get_display_width(display);
	al_rest(waittime); //code pause at here n seconds
	return 0;
}
void init_text() {
	al_init_font_addon();
	al_init_ttf_addon();

}
//show a text on screen
void print_text(char text[], int x, int y, int R = 255, int G = 255, int B = 255, const char font_name[] = "ARIALI.ttf") {
	ALLEGRO_FONT *font = al_load_ttf_font(font_name, 72, 0);
	al_draw_text(font, al_map_rgb(R, G, B), 400, 300, ALLEGRO_ALIGN_CENTER, text);
	al_flip_display();
}
//show a message box on screen
int message_box(const char* message = "No message given",
	const char* content_title = "Error",
	const char* title = "Error")
{
	switch (al_show_native_message_box(
		al_get_current_display(),
		title,
		content_title,
		message, NULL,
		ALLEGRO_MESSAGEBOX_YES_NO)
		) {
	case 0: return 0; // User pressed the Cancel button
	case 1: return 1; // User pressed the OK button
	}

}
//show an image on window and return bitmap
ALLEGRO_BITMAP *create_bitmap(char image_name[], float x, float y, int width, int height) {
	ALLEGRO_BITMAP *image = NULL;
	image = al_load_bitmap(image_name);
	al_draw_scaled_bitmap(image,
		0, 0, // source origin
		al_get_bitmap_width(image), // source width
		al_get_bitmap_height(image), // source height
		x, y, // target origin
		width, height, // target dimensions
		0 // flags
	);
	al_flip_display();
	return image;
}
//get max width and height of screen at start of program
void get_max_screen_size()
{
	al_init();
	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);//set windows fullscreen

	ALLEGRO_DISPLAY* display = al_create_display(800, 800);
	al_acknowledge_resize(al_get_current_display());//this tell gpu that windows size has changed
	//set max_width and max_height
	max_height = al_get_display_height(al_get_current_display());
	max_width= al_get_display_width(al_get_current_display());
	al_destroy_display(al_get_current_display());//destroy test display
}
//destroy bitmap for save memory
void destroy_bitmap(ALLEGRO_BITMAP *image)
{
	al_destroy_bitmap(image);
	al_flip_display();
}
//control events
void event_manager(ALLEGRO_EVENT ev)
{
	float cursor_x = 0, cursor_y = 0;//mouse coordinates
	//check mouse events
	if (ev.type == ALLEGRO_EVENT_MOUSE_AXES ||
		ev.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
		redraw = true;
		cursor_x = ev.mouse.x;
		cursor_y = ev.mouse.y;


	}
	//do the resizing process
	else if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
	{
		destroy_bitmap(bg_image);//destroy previos bg
		char name[] = "resources/go_bg2.png";
		al_acknowledge_resize(al_get_current_display());	//let gpu know that screen is resized 
		height = al_get_display_height(al_get_current_display());//height of window
		width = al_get_display_width(al_get_current_display());//width of window
		bg_image = create_bitmap(name, 0, 0, width, height);//create a new bg image
		al_flip_display();
		redraw = true;
	}
	//do close process
	else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
	{
		al_destroy_display(al_get_current_display());//close display. this save memory 
		end = true;
	}
	//keyboard input
	else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		//close process
		if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
		{
			al_destroy_display(al_get_current_display());
			end = true;
		}
	}
	if (redraw) {
		redraw = false;
		printf("mouse curosr location is updated : (%f, %f)", cursor_x, cursor_y);
		al_flip_display();
	}
}
//first inits
void inits()
{
	init_mouse();
	al_install_audio();
	init_primitive();
	init_bitmap();
	al_install_keyboard();
	init_display(0);
}
int main()
{

	get_max_screen_size();
	inits();
	char name[] = "resources/go_bg2.png";
	bg_image = create_bitmap(name, 0, 0, width, height);
	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	//registering event sources
	al_register_event_source(event_queue, al_get_display_event_source(al_get_current_display()));//display source
	al_register_event_source(event_queue, al_get_mouse_event_source());//mouse source
	al_register_event_source(event_queue, al_get_keyboard_event_source());//keyboard source
	//main loop
	while (!end)
	{

		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		event_manager(ev);

	}

	return 0;
}