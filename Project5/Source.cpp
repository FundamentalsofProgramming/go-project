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
#include<math.h>
#include<string.h>
#include"GameRules.h"
#include"Audio.h"

//structs
struct Circle
{
	float center_x;
	float center_y;
	float r;
}circle;
struct Mouse
{
	float posx;
	float posy;
}mouse;



//functions prototype
ALLEGRO_BITMAP *create_bitmap(char[], float, float, int, int);
void destroy_bitmap(ALLEGRO_BITMAP*);
ALLEGRO_BITMAP *create_circle(float, float, float);
bool isinrange(int &, int &);
int message_box(const char*,
	const char*,
	const char*);
void resize_screen();


//global Variables
int array[10][10] = { 0 }; // column 0 for white and 1 for black
bool end = false, redraw = true;//end:ending the main loop
float height, width;//value of current window pannel
float max_height, max_width;//maximum values of screen
//int pieces[10][10] = { 0 };
ALLEGRO_BITMAP *bg_image;
ALLEGRO_BITMAP *nuts_images[10][10];

enum player_turn { black = -1, white = 1 };
//enum player_turn { black = -1, white = 1 };
player_turn playerturn = white; // we can change who will start the game first here
void turn() { //changing turns it should be called each time @event manager
	if (playerturn == white)
		playerturn = black;
	else
		playerturn = white;
}

//void capture(int i,int j,player_turn playerturn) {
//	int k=0;
//	if (playerturn == white)
//	{
//		capturestatus[i][j][0] = (-1);
//		capturestatus[i][j][1] = (-1);
//		capturestatus[i][j - 1][1]--;
//		capturestatus[i][j + 1][1]--;
//		capturestatus[i-1][j][1]--;
//		capturestatus[i+1][j][1]--;
//	}
//	else if (playerturn == black)
//	{
//		capturestatus[i][j][1] = (-1);
//		capturestatus[i][j][0] = (-1);
//		capturestatus[i][j - 1][0]--;
//		capturestatus[i][j + 1][0]--; 
//		capturestatus[i - 1][j][0]--;
//		capturestatus[i + 1][j][0]--;
//	}
//}


int arraycheck(int array[][10], int i, int j) { // i and j will be defined @event manager (they declare coordinates)
	if (array[i][j] == 0) return 0;
	else if (array[i][j] == 1) return 1;
	else return -1;
}
void arrayset(int array[][10], int i, int j) {// i and j will be defined @event manager (they declare coordinates)
	if (playerturn == white)
	{
		if (arraycheck(array, i, j) == 0)
			array[i][j] = 1;
	}
	else
	{
		if (arraycheck(array, i, j) == 0)
			array[i][j] = -1;
	}
}








//draw a circle bitmap 
ALLEGRO_BITMAP *create_circle(float x, float y, float r, player_turn P_turn)
{
	ALLEGRO_BITMAP *image;
	char name[30];
	if (P_turn == black)
	{
		strcpy(name, "resources/blackPiece.png");
	}
	else {
		strcpy(name, "resources/whitePiece.png");
	}
	//set circle center poin in position
	x -= r;
	y -= r;
	r *= 2;
	image = create_bitmap(name, x, y, r, r);

	return image;
}

int check(int i, int j, player_turn playerturn, int hosti, int hostj,int debug[][10]) {
	int result[4] = { 0 };
	if (!(i + 1 == hosti && j == hostj))
	{
		if (i + 1 >= 10 || j >= 10)
		{
			result[0] = int(playerturn)*(-1);
		}
		else if (debug[i + 1][j] != 0)
		{
			result[0] = int(playerturn)*(-1);
			
		}
		else if (array[i + 1][j] == int(playerturn))
		{
			debug[i + 1][j] = 1;
			result[0] = check(i + 1, j, playerturn, i, j,debug);
			
		}
		else if (array[i + 1][j] == int(playerturn)*(-1))
		{
			debug[i + 1][j] = 1;
			result[0] = int(playerturn)*(-1);
		}
		
		else
			debug[i + 1][j] = 1;;


	}

	else
	{
		result[0] = 2;
	}
	if (!(i == hosti && j + 1 == hostj))
	{
		if (i  >= 10 || j+1 >= 10)
		{
			result[1] = int(playerturn)*(-1);
		}
		else if (debug[i ][j+1] != 0)
		{
			result[1] = int(playerturn)*(-1);

		}
		else if (array[i][j + 1] == int(playerturn))
		{
			debug[i ][j+1] = 1;
			result[1] = check(i, j + 1, playerturn, i, j, debug);
		}
		else if (array[i][j + 1] == int(playerturn)*(-1))
		{
			debug[i ][j+1] = 1;
			result[1] = int(playerturn)*(-1);
		}
		else
			debug[i ][j+1] = 1;;
	}
	else
	{
		result[1] = 2;
	}
	if (!(i == hosti && j - 1 == hostj))

	{
		if (i  >= 10 || j-1 <= -1)
		{
			result[2] = int(playerturn)*(-1);
		}
		else if (debug[i ][j-1] != 0)
		{
			result[2] = int(playerturn)*(-1);

		}
		else if (array[i][j - 1] == int(playerturn))
		{
			debug[i ][j-1] = 1;
			result[2] = check(i, j - 1, playerturn, i, j, debug);
		}
		else if (array[i][j - 1] == int(playerturn)*(-1))
		{
			debug[i][j-1] = 1;
			result[2] = int(playerturn)*(-1);
		}
		else
			debug[i ][j+1] = 1;;
	}
	else
	{
		result[2] = 2;
	}
	if (!(i - 1 == hosti && j == hostj))
	{
		if (i -1 <= -1 || j >= 10)
		{
			result[3] = int(playerturn)*(-1);
		}
		else if (debug[i -1][j] != 0)
		{
			result[3] = int(playerturn)*(-1);

		}
		else if (array[i - 1][j] == int(playerturn))
		{
			debug[i -1][j] = 1;
			result[3] = check(i - 1, j, playerturn, i, j, debug);
		}
		else if (array[i - 1][j] == int(playerturn)*(-1))
		{
			debug[i-1][j] = 1;
			result[3] = int(playerturn)*(-1);
		}
		else
			debug[i -1][j] = 1;;
	}
	else
	{
		result[3] = 2;
	}
	if (result[0] * result[1] * result[2] * result[3] == 0)
		return 0;
	else return playerturn;
}
void checkenemy(int i, int j, player_turn playerturn) {
	player_turn pt = playerturn == white ? black : white;
	int debug[10][10] = { 0 };
	int suicide[4] = { 0 };
	int test[4] = { 0 };
	if (i + 1 <= 9) {
		if (array[i + 1][j] == int(playerturn)*(-1))
		{
			test[0] = 1;
			suicide[0] = check(i + 1, j, pt, i, j, debug);
			if (suicide[0] != 0)
				printf("captured.\n");
			printf("enemy spotted: (%d, %d) type=%d \n", i + 1, j, int(playerturn)*(-1));
		}
	}
	for (int i = 0; i <= 9; i++)
		for (int j = 0; j <= 9; j++)
			debug[i][j] = 0;
	if (j - 1 >= 0)
	{
		if (array[i][j - 1] == int(playerturn)*(-1)) {
			test[1] = 1;
			suicide[1] = check(i, j - 1, pt, i, j, debug);
			if (suicide[1] != 0)
				printf("captured.\n");
			printf("enemy spotted: (%d, %d) type=%d \n", i, j - 1, int(playerturn)*(-1));
		}
	}
	for (int i = 0; i <= 9; i++)
		for (int j = 0; j <= 9; j++)
			debug[i][j] = 0;
	if (j + 1 <= 9)
	{
		if (array[i][j + 1] == int(playerturn)*(-1))
		{
			test[2] = 1;
			suicide[2] = check(i, j + 1, pt, i, j, debug);
			if (suicide[2] != 0)
				printf("captured.\n");
			printf("enemy spotted: (%d, %d) type=%d \n", i, j + 1, int(playerturn)*(-1));
		}
	}
	for (int i = 0; i <= 9; i++)
		for (int j = 0; j <= 9; j++)
			debug[i][j] = 0;
	if (i - 1 >= 0)
	{
		if (array[i - 1][j] == int(playerturn)*(-1))
		{
			test[3] = 1;
			suicide[3] = check(i - 1, j, pt, i, j, debug);
			if (suicide[3] != 0)
				printf("captured.\n");
			printf("enemy spotted: (%d, %d) type=%d \n", i - 1, j, int(playerturn)*(-1));
		}
	}
	bool issuicide = false;
	for (int i = 0; i < 4; i++)
		if (suicide[i] != 0)
			issuicide = true;
	if (issuicide)
		printf("not suicide.\n");
	if (!issuicide)
		printf("suicide. :D\n");

}


void setCircle(int &i, int &j)
{
	circle.r = width / 11 > height / 11 ? height / 11 : width / 11;//this will sure that our r is not greater than our square
	circle.r *= .5f;
	circle.center_x = j * (width / 11);
	circle.center_y = i * (height / 11);
}
//check mouse is in circle range
bool isinrange(int &i, int &j)
{

	if (powf((mouse.posx - circle.center_x), 2) + powf((mouse.posy - circle.center_y), 2) <= powf(circle.r, 2))//equal of circle
	{
		return true;
	}

	return false;
}
//caculate which coordinate has been clicked
void setAndis(int &i, int &j)
{
	//+.5f will round it
	j = (int)((mouse.posx / (width / 11)) + .5f);
	i = (int)((mouse.posy / (height / 11)) + .5f);
}
//redraw previous drew circles
void Redraw()
{

	for (int i = 1; i < 11; i++)
	{
		for (int j = 1; j < 11; j++)
		{
			if (arraycheck(array, i - 1, j - 1) == 0)
				continue;
			else if (arraycheck(array, i - 1, j - 1) == 1)
			{
				setCircle(i, j);//set circle properties
				destroy_bitmap(nuts_images[i - 1][j - 1]);//destroy previous circle image for memory managment
				nuts_images[i - 1][j - 1] = create_circle(circle.center_x, circle.center_y, circle.r, white);
				
			}
			else
			{
				destroy_bitmap(nuts_images[i - 1][j - 1]);
				setCircle(i, j);
				nuts_images[i - 1][j - 1] = create_circle(circle.center_x, circle.center_y, circle.r, black);
				
			}
		}
	}
}
void putPieces()
{
	int i, j;
	setAndis(i, j);//caculate which coordinate has been clicked
	if (i >= 1 && i <= 10)
	{
		if (j >= 1 && j <= 10)
		{
			setCircle(i, j);
			//check that click is trigger range
			if (isinrange(i, j))
			{

				//set coordinate for array limits
				i--;
				j--;
				//is this home filled ?
				if (arraycheck(array, i, j) == 0)
				{

					//save image into an array
					nuts_images[i][j] = create_circle(circle.center_x, circle.center_y, circle.r, playerturn);
					
					arrayset(array, i, j);
					checkenemy(i, j, playerturn);
					turn();//change player turn
						   //int checkresult=check(i, j, playerturn, i, j);
						   /*	if (checkresult == 0)
						   printf("free \n");
						   else
						   printf("capture.\n");*/

				}
			}
		}
	}
}

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

	ALLEGRO_DISPLAY* display = al_create_display(max_width / 2, max_height / 2);
	al_set_window_constraints(display, max_width / 3, max_height / 3, max_width, max_height);//bound the window
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
	max_width = al_get_display_width(al_get_current_display());
	al_destroy_display(al_get_current_display());//destroy test display
}
//destroy bitmap for save memory
void destroy_bitmap(ALLEGRO_BITMAP *image)
{
	al_destroy_bitmap(image);
	al_flip_display();
}
/*//this in test progress
void undo()
{
int i, j;
printf("Test");
setAndis(i, j);//caculate which coordinate has been clicked
if (i >= 1 && i <= 10)
{
if (j >= 1 && j <= 10)
{
//check that click is trigger range
if (isinrange(i, j))
{

if (arraycheck(array, i - 1, j - 1))
{
//destroy_bitmap(nuts_images[i - 1][j - 1]);
array[i - 1][j - 1] = 0;
al_resize_display(al_get_current_display(), width, height);
resize_screen();
//Redraw();
al_flip_display();
}

}
}
}
}*/
void resize_screen()
{

	destroy_bitmap(bg_image);//destroy previos bg
	char name[] = "resources/go_bg2.png";
	al_acknowledge_resize(al_get_current_display());	//let gpu know that screen is resized 
	height = al_get_display_height(al_get_current_display());//height of window
	width = al_get_display_width(al_get_current_display());//width of window
	bg_image = create_bitmap(name, 0, 0, width, height);//create a new bg image

	Redraw();
	al_flip_display();
}
//control events
void event_manager(ALLEGRO_EVENT ev)
{
	//float cursor_x = 0, cursor_y = 0;//mouse coordinates
	//check mouse events
	if (ev.type == ALLEGRO_EVENT_MOUSE_AXES ||
		ev.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
		redraw = true;
		//	mouse.posx = ev.mouse.x;
		//	mouse.posy = ev.mouse.y;


	}
	//do the resizing process
	else if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
	{
		resize_screen();
		redraw = true;
	}
	//check mouse clicks
	else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
	{
		//check left click
		if (ev.mouse.button == 1)
		{
			mouse.posx = ev.mouse.x;
			mouse.posy = ev.mouse.y;
			putPieces();//call put function
		}
		/*else if (ev.mouse.button == 2)//this in test progress
		{
		//printf("test1");
		mouse.posx = ev.mouse.x;
		mouse.posy = ev.mouse.y;
		undo();
		}*/
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
		//printf("mouse curosr location is updated : (%f, %f)", mouse.posx, mouse.posy);
		//printf("max height = \n %f", max_height);
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
	char audioname[] = "resources/bg.wav";
	
	if (loadAuido(audioname))
		printf("Audio init.\n");
	if (playAudio(ALLEGRO_PLAYMODE_LOOP))
		printf("audio played.\n");
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