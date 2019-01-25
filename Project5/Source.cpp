#include <allegro5/allegro.h> 
#include <allegro5/allegro_native_dialog.h> 
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include<allegro5\allegro_audio.h>
#include <allegro5/allegro_color.h>
#include<allegro5\fullscreen_mode.h>
//#include<allegro5\bitmap_draw.h>

//#include<stdio.h>
#include<math.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
//#include<string.h>
#include"GameRules.h"
#include"Audio.h"
//#include "button.h"
//#include <time.h>
#include <stdlib.h>
#include "Group.h"
#include"SaveAndLoad.h"
#include<allegro5\timer.h>
#include"Timer.h"
#include<exception>
//#include<allegro5\timer.h>


//struct button that holds our buttons properties
struct Button
{
	float sti, endi;
	float stj, endj;
};
Button _button[10];//an array of button in game note that you can increse it size base on buttons you have
//button enum for beauti and easy acces to struct array
enum buttons
{
	Startbtn=0,aboutbtn=1,exitbtn=2,singleplayerbtn=3,twoplayerbtn=4,backbtn=5,loadbtn=6
};

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

//global Variables
int aidebug[10][10] = { 0 };
int ib = 0, jb = 0, iw = 0, jw = 0;
int array[10][10] = { 0 }; // 0 empty 1 white -1 black
int whitescore = 0, blackscore = 0;
int aicounter = 0;
//int score[2]; //0 for black and 1 for white 
bool end = false, redraw = true;//end:ending the main loop
float height, width;//value of current window pannel
					//player_turn playerturn = white; // we can change who will start the game first here
int passcounterw = 0, passcounterb = 0; //pass counters for saving turn and etc.
float max_height, max_width;//maximum values of screen
							//int pieces[10][10] = { 0 };
ALLEGRO_BITMAP *bg_image;
ALLEGRO_BITMAP *nuts_images[10][10];
enum player_turn { black = -1, white = 1 };//enum player_turn { black = -1, white = 1 };
player_turn playerturn = white;
enum gamestate { menu, singleplayer, twoplayer, about, startbranch }_gamestate;//mix
gamestate gs = menu;
ALLEGRO_BITMAP *turnshow = NULL;

//functions prototype
ALLEGRO_BITMAP *create_bitmap(char[], float, float, int, int);
ALLEGRO_BITMAP *create_bitmap(const char image_name[], float x, float y, int width, int height);
void destroy_bitmap(ALLEGRO_BITMAP*);
void ai(int [][10], int &, int &);
ALLEGRO_BITMAP *create_circle(float, float, float);
void buttonSence();
void configureBtn();
bool isinrange(int &, int &);
void print_text(const char text[], int x, int y, int R=255 , int G=255 , int B=255 , const char font_name[]="resources/arial.ttf" );
int message_box(const char*,
	const char*,
	const char*);
void resize_screen();
void Redraw(); //mix
void changeScreen(gamestate gs);



void turnShow()
{
	destroy_bitmap(turnshow);
	switch (playerturn)
	{
	case black:
		turnshow= create_bitmap("resources/blackPiece.png", 0, 0, width / 21 * 1, height / 20 * 1);
		break;
	case white:
		turnshow = create_bitmap("resources/whitePiece.png", 0, 0, width / 21 * 1, height / 20 * 1);
		break;
	default:
		break;
	}

}

void deadones(int array[][10],player_turn playerturn) { // mohre haye morde
	int cpyarr[10][10];
	for(int i=0;i<=9;i++)
		for (int j = 0; j <= 9; j++) {
			cpyarr[i][j] = array[i][j];
			if (cpyarr[i][j] == int(playerturn)|| cpyarr[i][j] == 0)
				cpyarr[i][j] = int(playerturn)*-1;
		}

}
void komi(int &whitescore,int &blackscore, int playerturn) { //meghdar komi baraye shoroo konande
	if (playerturn == white)
		whitescore += 6.5;
	else
		blackscore += 6.5;
	
	
}
void turn() { //changing turns it should be called each time @ event manager
	if (playerturn == white)
		playerturn = black;
	else
		playerturn = white;
	turnShow();
	//startTimer(playerturn);
}
char *ftoa(float number,int deghatP,int deghati)
{
	char *buffer = NULL;
	int temp1, temp2;
	temp1 = (int)(number * pow(10, deghatP)) % ((int)pow(10,deghatP));
	temp2 = number;
	buffer = new char[deghati+deghatP+2];
	int counter = 0;
	while (temp2!=0)
	{
		buffer[counter] = temp2 % 10 + '0';
		temp2 /= 10;
		counter++;
	}
	buffer[counter] = '.';
	while (temp1!=0)
	{
		buffer[counter] = temp1 % 10 + '0';
		temp1 /= 10;
		counter++;
	}
	return buffer;
}

void pass(player_turn playerturn) { // this function will add the pass rule to the game 
	
	if (playerturn == white)
	{
		passcounterw++;
		turn();
		
	}
	else
	{
		passcounterb++;
		turn();
	}
	stopTimer();
	startTimer(playerturn);
	if (passcounterb == 1 && passcounterw == 1)
		end = true;

	if (passcounterb > 1 && passcounterw == 0)
	{
		passcounterb = 0;
		passcounterw = 0;
	}
	if (passcounterw > 1 && passcounterb == 0)
	{
		passcounterw = 0;
		passcounterb = 0;
	}
}

int overflow(int parameter, int min, int max) { //false for overflow
	if (parameter > max || parameter < min)
		return false;
	else
		return true;
}

void scoring(int array[][10],int &blackscore,int &whitescore)
{
	int i = 0;
	int j = 0;
	deadones(array,playerturn);
	for (i = 0; i <= 9; i++)
	{
		for (j = 0; j <= 9; j++)
		{
			if (array[i][j] == 1) whitescore++;
			if (array[i][j] == -1) blackscore++;

		}
	}

}


void Territory() // n for neutral 1for white and -1 for black
{}




int checkwhiteenemy(int i, int j, int array[][10], int &ib, int &jb) {
	if (overflow(i, 0, 9) && overflow(j, 0, 9))
	{
		if (array[i + 1][j] == -1) {
			ib = i + 1;
			jb = j;
			return true;
		}
		else if (array[i][j - 1] == -1) {
			ib = i;
			jb = j - 1;
			return true;
		}
		else if (array[i][j + 1] == -1) {
			ib = i;
			jb = j + 1;
			return true;
		}
		else if (array[i - 1][j] == -1) {
			ib = i - 1;
			jb = j;
			return true;
		}
		else
			return 0;
	}
	else return 0;


}


void eyemaker(int array[][10], int &ib, int &jb, int initiali, int initialj) {
	int i = 0, j = 0, sw = 0, check0 = 0;
	sw = 0;
	
	for (initiali; initiali <= 9; initiali++)
	{
		if (initialj > 9) initialj = 0;
		for (initialj ; initialj <= 9; initialj++) {
			if (array[initiali][initialj] == 0)
			{
				sw = 1;
				break;
			}

		}
		if (sw == 1) break;
	}
	sw = 0;
	if (overflow(initiali + 1, 0, 9)) {
		if (array[initiali + 1][initialj] == 0 || array[initiali + 1][initialj] == -1) { check0 = 1; }
		else check0 = 0;
		//else if (array[i + 1][j] == 1) sw = 1;

	}
	if (overflow(initiali - 1, 0, 9) && check0 == 1) {
		if (array[initiali - 1][initialj] == 0 || array[initiali - 1][initialj] == -1) { check0 = 1; }
		else check0 = 0;
	}
	if (overflow(initialj + 1, 0, 9) && check0==1) {
		if (array[initiali][initialj + 1] == 0 || array[initiali][initialj+1] == -1) { check0 = 1; }
		else check0 = 0;
	}
	if (overflow(initialj - 1, 0, 9) && check0 == 1) {
		if (array[initiali][initialj - 1] == 0 || array[initiali][initialj - 1] == -1) { check0 = 1; }
		else check0 = 0;
	}

	if((!overflow(initiali + 1, 0, 9)|| array[initiali + 1][initialj] == -1)&&(!overflow(initiali - 1, 0, 9)|| array[initiali - 1][initialj] == -1)&&(!overflow(initialj + 1, 0, 9)||array[initiali][initialj + 1] == -1)&& (!overflow(initialj - 1, 0, 9)|| array[initiali][initialj - 1] == -1))
		eyemaker(array, ib, jb, initiali, initialj + 1);


	if (!check0) {
		eyemaker(array, ib, jb, initiali, initialj + 1);
	}
	else if (check0) {
		if (overflow(initiali - 1, 0, 9))
			if (array[initiali - 1][initialj] == 0) 
			{
				ib = initiali - 1;
				jb = initialj;
			}
		if (overflow(initialj + 1, 0, 9))
			if (array[initiali][initialj + 1] == 0)
			{
				ib = initiali ;
				jb = initialj+1;
			}
		if (overflow(initialj - 1, 0, 9))
			if (array[initiali][initialj - 1] == 0)
			{
				ib = initiali;
				jb = initialj - 1;
			}
		if (overflow(initiali + 1, 0, 9))
			if (array[initiali + 1][initialj] == 0) {
				ib = initiali+1;
				jb = initialj ;
			}
		/*if (ib == initiali&&jb == initialj) {
			eyemaker(array, ib, jb, initiali, initialj + 1);
		}*/
	}

}




int findwhitegp(int &iw,int &jw,int startpointi,int startpointj ){
	int sw = 0;
	int i = 0;
	int j = 0;
	for ( i = startpointi; i <= 9; i++) {
		for ( j = startpointj; j <= 9; j++)
		{
			if (getColorOfGroup(i, j) == 1)
			{
				sw = 1;
				break;
			}
			}
		if (sw == 1) break;
	}

	if (i > 9 || j > 9) {
		iw = rand() % 10;
		jw = rand() % 10;
	//	pass(playerturn);
		return 0;
	}
	else
	{
		iw = i;
		jw = j;
		return gettoken(i, j);
	}
		
}

int offrecresive(int i,int j,int array[][10]) { // mire mohre ro capture mikone  
	int swo=0;
	if (overflow(i + 1, 0, 9)) 
		if (array[i + 1][j] == 0)
		{
			ib = i + 1;
			jb = j;
			return 1;
		}
	
	 if (overflow(i - 1, 0, 9)) 
		if (array[i - 1][j] == 0)
		{
			ib = i - 1;
			jb = j;
			return 1;
		}
	
	if (overflow(j - 1, 0, 9)) 
		if (array[i][j - 1] == 0) {

			ib = i;
			jb = j - 1;
			return 1;
		}
	
	if (overflow(j + 1, 0, 9)) 
		if (array[i][j + 1] == 0)
		{
			ib = i;
			jb = j + 1;
			return 1;
		}
	
	if(true)
	{
		//ib = rand() % 10;
		//jb = rand() % 10;
		return 0;

	}
	
}

void offensive(int array[][10], int &ib, int &jb, int startpointi, int startpointj)
{
	int i, j,y=0,x=0;
	int token;
	token = findwhitegp(iw, jw,startpointi,startpointj);
	/*for (i = 0; i <= 9; i++) {
		for (j = 0; j <= 9; j++)
		{
			if (gettoken(i, j) == token) {
				sw = 1;
				break;
			}
		}
		if (sw == 1) break;
	}*/
	y=offrecresive(iw, jw, array);
	if (!y) {
		x = jw + 1;
		offensive(array, iw, jw,iw,x);
	}

}

int neighbourprio(int &ib, int &jb, int array[][10]) {
	int counterbala = 0, counterpayin = 0, counterrast = 0, counterchap = 0, ibala = 0, ipayin = 0, ichap = 0, irast = 0;
	int jbala = 0, jpayin = 0, jchap = 0, jrast = 0;
	int ibavaliye = ib;
	int jbavaliye = jb;
	int sw1 = 0;//unfortunetly its necessary for next lines
	if (overflow(ib, 0, 9) && overflow(jb + 1, 0, 9) && sw1 == 0) {
		if (array[ib][jb + 1] == 0)
		{
			irast = ib;
			jrast = jb + 1;


			if (array[ib - 1][jb + 1] == 0)//&& sw1 == 0)
			{
				//irast--;
				counterrast++;
				//sw1 = 1;
			}
			if (array[ib + 1][jb + 1] == 0)//&& sw1 == 0)
			{
				//irast++;
				counterrast++;
				//sw1 = 1;
			}
			if (array[ib][jb + 2] == 0)//&& sw1 == 0)
			{
				//jrast++;
				counterrast++;
				//sw1 = 1;
			}
			else
			{
				//	jb++;
			}
		}
	}
	int sw2 = 0;
	if (overflow(ib, 0, 9) && overflow(jb - 1, 0, 9) && sw2 == 0) {
		if (array[ib][jb - 1] == 0)
		{
			ichap = ib;
			jchap = jb - 1;
			if (array[ib - 1][jb - 1] == 0)//&& sw2 == 0)
			{
				//ichap--;
				counterchap++;
				//sw2 = 1;
			}
			if (array[ib + 1][jb - 1] == 0)//&& sw2 == 0)
			{
				//ichap++;
				counterchap++;
				//sw2 = 1;
			}
			if (array[ib][jb - 2] == 0)//&& sw2 == 0)
			{
				//jchap--;
				counterchap++;
				//sw2 = 1;
			}
			else
			{
				//jb--;
			}
		}
	}
	int sw3 = 0;
	if (overflow(ib + 1, 0, 9) && overflow(jb, 0, 9) && sw3 == 0) {
		if (array[ib + 1][jb] == 0)
		{
			ipayin = ib + 1;
			jpayin = jb;
			if (array[ib + 1][jb + 1] == 0)//&& sw3 == 0)
			{

				//	jpayin++;
				counterpayin++;
				//sw3 = 1;
			}
			if (array[ib + 1][jb - 1] == 0)//&& sw3 == 0)
			{
				//ib++;
				//jpayin--;
				counterpayin++;
				//sw3 = 1;
			}
			if (array[ib + 2][jb] == 0)//&& sw3 == 0)
			{
				//ipayin++;
				counterpayin++;
				//sw3 = 1;
			}
			else
			{
				//ib++;
			}
		}
	}
	int sw4 = 0;
	if (overflow(ib - 1, 0, 9) && overflow(jb + 1, 0, 9) && sw4 == 0) {
		if (array[ib - 1][jb] == 0)
		{
			ibala = ib - 1;
			jbala = jb;
			if (array[ib - 1][jb + 1] == 0) //&& sw4 == 0)
			{

				//jbala++;
				counterbala++;
				//sw4 = 1;
			}
			else if (array[ib - 1][jb - 1] == 0) //&& sw4 == 0)
			{

				//jbala--;
				counterbala++;
				//sw4 = 1;
			}
			else if (array[ib - 2][jb] == 0)//&& sw4 == 0)
			{
				//ibala--;
				counterbala++;
				//sw4 = 1;
			}
			else
			{
				//ib--;
			}
		}
	}
	int sw = 0;//require for next lines
	if ((counterbala == 0 && counterchap == 0 && counterpayin == 0 && counterrast == 0)) {// in if va else mitoonan cm beshan
		if (overflow(ib - 1, 0, 9) && overflow(jb, 0, 9) && sw == 0)
			if (array[ib - 1][jb] == 0) { ib--; sw = 1; }

		if (overflow(ib + 1, 0, 9) && overflow(jb, 0, 9) && sw == 0)
			if (array[ib + 1][jb] == 0) { ib++; sw = 1; }

		if (overflow(ib, 0, 9) && overflow(jb + 1, 0, 9) && sw == 0)
			if (array[ib][jb + 1] == 0) { jb++; sw = 1; }

		if (overflow(ib, 0, 9) && overflow(jb - 1, 0, 9) && sw == 0)

			if (array[ib][jb - 1] == 0) { jb--; sw = 1; }
		if (ibavaliye == ib&&jbavaliye == jb) {
			ai(array, ib, jb);

			//ib = rand() % 10;
			//jb = rand() % 10;
			//offensive(array, ib, jb);
			//eyemaker(array, ib, jb, 1, 1);
			
		}
		if (ibavaliye == ib&&jbavaliye == jb) {
			//ib = rand() % 10;
			//jb = rand() % 10;
			offensive(array, ib, jb,0,0);
		}
	}
	else
	{

		if (counterbala > counterchap&&counterbala > counterrast&&counterbala > counterrast)
		{
			ib = ibala;
			jb = jbala;
		}
		else if (counterchap > counterpayin&&counterchap > counterrast&&counterchap > counterbala) {
			ib = ichap;
			jb = jchap;
		}
		else if (counterrast > counterbala&&counterrast > counterchap&&counterrast > counterpayin) {
			ib = irast;
			jb = jrast;
		}
		else if (counterpayin > counterbala&&counterpayin > counterchap&&counterpayin > counterrast) {
			ib = ipayin;
			jb = jpayin;
		}
		//else if()
	
		else { // masaln countere bala va payin dotashoon 1beshan
			//ib = rand() % 10;
			//jb = rand() % 10;
			offensive(array, ib, jb, 0, 0);
			return 1;
			
			
			//eyemaker(array, ib, jb, 1, 1 );
			
		}
		//if (counterbala == counterchap) {}
	}
}

void checkpriority(int array[][10], int i, int j, int &ib, int &jb) {
	int x,y;
	x = checkwhiteenemy(i - 1, j - 1, array, ib, jb);// ib va jb dakhel array ro mide
	if (x == 0) {
		//ib = rand() % 10;
		//jb = rand() % 10;
		
		offensive(array, ib, jb, 0, 0);
	//	eyemaker(array, ib, jb, 1, 1);
		
	}
	else
	{
		if (overflow(ib, 0, 9) && overflow(jb, 0, 9))
		y=	neighbourprio(ib, jb, array);// ib jayi ke roosh mishe keshido nide
		
		else
		{
			//ib = rand() % 10;
			//jb = rand() % 10;
			offensive(array, ib, jb, 0, 0);
			
		//	eyemaker(array, ib, jb, 1, 1);
			
		}
	}

}
int charjahat(int i1, int j1,int array[][10]) { //in tabe check mikone bebine kenare khoone i j khali voojooddarad ya na
	int sw=0;
	if (aidebug[i1][j1] == 1) return 0; // bastas va ghablan check shode
	if (overflow(i1 + 1, 0, 9))
	{
		if (array[i1 + 1][j1] == 0)
		{
			sw = 1;
		}
	}
	else if (overflow(i1 - 1, 0, 9) && sw == 0) {
		if (array[i1 - 1][j1] == 0)
		{
			sw = 1;
		}
	}
	else if (overflow(j1 - 1, 0, 9) && sw == 0) {
		if (array[i1][j1 - 1] == 0) {

			sw = 1;
		}
	}
	else if (overflow(j1 + 1, 0, 9) && sw == 0) {
		if (array[i1][j1 + 1] == 0)
		{
			sw = 1;
		}
	}
	if (sw == 1) return 1; //kenaresh yek khoone khali vojood darad
	else {
		aidebug[i1][j1] = 1;
		return 0;//4tarafash baste ast
	}
	}



void ai(int array[][10], int &ib, int &jb) {
	int token,sw1=0;
	int i1, j1;
	token = gettoken(ib, jb);
	for (i1 = 0; i1 <= 9; i1++)
	{
		for (j1 = 0; j1 <= 9; j1++) {

			if (gettoken(i1, j1) == token)
			{
				if (charjahat(i1, j1, array))
				{
					sw1 = 1;
					break;
				}
				else {
					continue;
				}
			
			}
		}
		if (sw1 == 1) break;
	}
	if(overflow(i1 + 1,0,9))
	if (array[i1 + 1][j1] == 0)
	{
		ib = i1 + 1;
		jb = j1;
	}
	else if(overflow(i1 - 1, 0, 9))
		if (array[i1 - 1][j1] == 0)
	{
		ib = i1 - 1;
		jb = j1;
	}
	else if (overflow(j1- 1, 0, 9))
		if (array[i1][j1 - 1] == 0) {

		ib = i1;
		jb = j1 - 1;
	}
	else if (overflow(j1 + 1, 0, 9))
		if (array[i1][j1 + 1] == 0)
	{
		ib = i1;
		jb = j1 + 1;
	}
	else
		ai(array, ib, jb);

}


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


int check(int i, int j, player_turn playerturn, int hosti, int hostj, int debug[][10]) {
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
			setGroupBlock(i + 1, j, blocktype::ONE, true); //mix
			result[0] = check(i + 1, j, playerturn, i, j, debug);

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
		if (i >= 10 || j + 1 >= 10)
		{
			result[1] = int(playerturn)*(-1);
		}
		else if (debug[i][j + 1] != 0)
		{
			result[1] = int(playerturn)*(-1);

		}
		else if (array[i][j + 1] == int(playerturn))
		{
			debug[i][j + 1] = 1;
			setGroupBlock(i, j + 1, blocktype::ONE, true); //mix
			result[1] = check(i, j + 1, playerturn, i, j, debug);
		}
		else if (array[i][j + 1] == int(playerturn)*(-1))
		{
			debug[i][j + 1] = 1;
			result[1] = int(playerturn)*(-1);
		}
		else
			debug[i][j + 1] = 1;;
	}
	else
	{
		result[1] = 2;
	}
	if (!(i == hosti && j - 1 == hostj))

	{
		if (i >= 10 || j - 1 <= -1)
		{
			result[2] = int(playerturn)*(-1);
		}
		else if (debug[i][j - 1] != 0)
		{
			result[2] = int(playerturn)*(-1);

		}
		else if (array[i][j - 1] == int(playerturn))
		{
			debug[i][j - 1] = 1;
			result[2] = check(i, j - 1, playerturn, i, j, debug);
		}
		else if (array[i][j - 1] == int(playerturn)*(-1))
		{
			debug[i][j - 1] = 1;
			setGroupBlock(i, j - 1, blocktype::ONE, true); //mix
			result[2] = int(playerturn)*(-1);
		}
		else
			debug[i][j + 1] = 1;;
	}
	else
	{
		result[2] = 2;
	}
	if (!(i - 1 == hosti && j == hostj))
	{
		if (i - 1 <= -1 || j >= 10)
		{
			result[3] = int(playerturn)*(-1);
		}
		else if (debug[i - 1][j] != 0)
		{
			result[3] = int(playerturn)*(-1);

		}
		else if (array[i - 1][j] == int(playerturn))
		{
			debug[i - 1][j] = 1;
			setGroupBlock(i - 1, j, blocktype::ONE, true); //mix
			result[3] = check(i - 1, j, playerturn, i, j, debug);
		}
		else if (array[i - 1][j] == int(playerturn)*(-1))
		{
			debug[i - 1][j] = 1;
			result[3] = int(playerturn)*(-1);
		}
		else
			debug[i - 1][j] = 1;;
	}
	else
	{
		result[3] = 2;
	}
	if (result[0] * result[1] * result[2] * result[3] == 0)
		return 0;//return this side is free
	else return playerturn;
}



/*void showmsg(player_turn playerturn) {
	int value;
	if (playerturn == 1)//white
	{
		value = message_box("do you want to continue?", "white won", "captured");
		if (value == 2)
			end = true;

	}
	else
	{
		value = message_box("do you want to continue?", "BLACK won", "captured");
		if (value == 2)
			end = true;
	}
}*/
bool checkenemy(int i, int j, player_turn playerturn) {
	bool capture = false;
	player_turn pt = playerturn == white ? black : white;
	int debug[10][10] = { 0 };
	int suicide[4] = { 0 };
	int test[4] = { 0 };// 0 payin 1chap 2rast 3bala
	if (i + 1 <= 9) {
		if (array[i + 1][j] == int(playerturn)*(-1))
		{
			setGroupBlock(i + 1, j, blocktype::ONE, true); //mix
			test[0] = 1;
			suicide[0] = check(i + 1, j, pt, i, j, debug);//check side is capttured?
			if (suicide[0] != 0) {// dakhel if mix
				deleteGroup(i + 1, j, array);
				//Redraw();
				//resize_screen();
				capture = true;
				printf("captured.\n");
				//showmsg(playerturn);
			}
			else
				setGroupBlock(i + 1, j, blocktype::ALL, false);
		}
	}
	for (int i = 0; i <= 9; i++)
		for (int j = 0; j <= 9; j++)
			debug[i][j] = 0;
	if (j - 1 >= 0)
	{
		if (array[i][j - 1] == int(playerturn)*(-1)) {
			setGroupBlock(i, j - 1, blocktype::ONE, true);
			test[1] = 1;
			suicide[1] = check(i, j - 1, pt, i, j, debug);
			if (suicide[1] != 0)//dakhel if mix
			{
				deleteGroup(i, j - 1, array);
				//Redraw();
				//resize_screen();
				capture = true;
				printf("captured.\n");
				//showmsg(playerturn);
			}
			else
				setGroupBlock(i, j + 1, blocktype::ALL, false);
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
			{
				deleteGroup(i, j + 1, array);
				//Redraw();
				//resize_screen();
				capture = true;
				printf("captured.\n");
				//showmsg(playerturn);
			}
			else
				setGroupBlock(i, j + 1, blocktype::ALL, false);
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
			{
				deleteGroup(i - 1, j, array);
				//Redraw();
				//resize_screen();
				capture = true;

				printf("captured.\n");
				//showmsg(playerturn);
			}
			else
				setGroupBlock(i - 1, j, blocktype::ALL, false);
		}
	}
	return capture;
	/*if (!capture)
	{
		int temp=0;
		for (int i = 0; i < 4; i++)
			temp += test[i];
		if (temp == 4)
		{
			printf("suicide.\n");
		}
	}*/
	//bool issuicide = false;
	//for (int i = 0; i < 4; i++)
	//	if (suicide[i] != 0)
	//		issuicide = true;
	//if (issuicide)
	//	//printf("not suicide.\n");
	//if (!issuicide)
	//	//printf("suicide. :D\n");

}
bool suicide(int i, int j, player_turn playerturn)
{
	int debug[10][10] = { 0 };
	if (check(i, j, playerturn, 0, 0, debug))
	{
		setGroupBlock(i, j, blocktype::ALL, false);
		return true;
	}
	else
	{
		setGroupBlock(i, j, blocktype::ALL, false);
		return false;
	}
	/*int test[4] = { 0 };
	if (overflow(i + 1, 0, 9))
	{
		test[0] = check(i+1, j, playerturn,i,j,debug);
	}
	for (int i = 0; i <= 9; i++)
		for (int j = 0; j <= 9; j++)
			debug[i][j] = 0;
	if (overflow(i - 1, 0, 9))
	{
		test[1] = check(i-1, j, playerturn,i,j,debug);
	}
	for (int i = 0; i <= 9; i++)
		for (int j = 0; j <= 9; j++)
			debug[i][j] = 0;
	if (overflow(j+1, 0, 9))
	{
		test[2] = check(i, j+1, playerturn,i,j,debug);
	}
	for (int i = 0; i <= 9; i++)
		for (int j = 0; j <= 9; j++)
			debug[i][j] = 0;
	if (overflow(j- 1, 0, 9))
	{
		test[3] = check(i, j-1, playerturn,i,j,debug);
	}
	for (int i = 0; i <= 9; i++)
		for (int j = 0; j <= 9; j++)
			debug[i][j] = 0;
	if (test[0] * test[1] * test[2] * test[3] != 0)
		return true;
	return false;*/
	/*player_turn pt = playerturn == white ? black : white;
	checkenemy(i, j, pt);*/
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
//	try
	{
		int i, j;
		setAndis(i, j);//caculate which coordinate has been clicked
		iw = i - 1;
		jw = j - 1;
		if (_gamestate == gamestate::singleplayer)
		{
			if (playerturn == player_turn::black)
			{
				//ai(playerturn, ib, jb);
				/*ib++;
				jb++;*/
				checkpriority(array, i, j, ib, jb);
				if (suicide(ib, jb, black))
				{
					ib = rand() % 10;
					jb = rand() % 10;
				}
				i = ib + 1;
				j = jb + 1;
			}
		}
		if (overflow(i, 1, 10))
		{
			if (overflow(j, 1, 10))
			{
				setCircle(i, j);

				//check that click is trigger range
				if (playerturn == white ? isinrange(i, j) : true)
				{

					//set coordinate for array limits
					i--;
					j--;


					//is this home filled ?
					if (arraycheck(array, i, j) == 0)
					{

						bool test = false;

						//save image into an array

						//checktime(playerturn);

						if (checkenemy(i, j, playerturn))
						{
							//Redraw();
							//resize_screen();
							test = true;
						}
						if (!suicide(i, j, playerturn))
						{
							nuts_images[i][j] = create_circle(circle.center_x, circle.center_y, circle.r, playerturn);
							arrayset(array, i, j);
							checkGrouping(i, j, playerturn, array);
							if (test)
							{
								Redraw();
								resize_screen();
							}
							turn();
							stopTimer();
							startTimer(playerturn);

						}
						else {
							//();
							//resize_screen();
							printf("suicide.\n");
						}
						//print_text("hi", 0, 0, 255, 255, 255);
						/*arrayset(array, i, j);
						checkGrouping(i, j, playerturn, array);
						checkenemy(i, j, playerturn);
						if (!suicide(i, j, playerturn))
						{
							turn();
							stopTimer();
							startTimer(playerturn);
						}
						else
							printf("suicide.\n");*/

							//change player turn
							//int checkresult=check(i, j, playerturn, i, j);
							/*	if (checkresult == 0)
							printf("free \n");
							else
							printf("capture.\n");*/

					}
					else
					{
						//	ai(playerturn, ib, jb);
					}
				}
			}
		}
		if (_gamestate == gamestate::singleplayer)
		{
			if (playerturn == black)
			{
				//al_rest(.5f);
				putPieces();
			}
		}
	}
	//catch (const std::exception&)
	//{
		//end = true;
	//}
	
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
void print_text(const char text[], int x, int y, int R , int G , int B , const char font_name[] ) {
	ALLEGRO_FONT *font = al_load_ttf_font(font_name, width/21, 0);
	al_draw_text(font, al_map_rgb(R, G, B), x, y, ALLEGRO_ALIGN_CENTER, text);
	al_flip_display();
}
//void ()
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
ALLEGRO_BITMAP *create_bitmap(const char image_name[], float x, float y, int width, int height) {
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
	char name[30];
	switch (_gamestate)
	{
	case menu:
		strcpy(name, "resources/Mainmenu.png");
		break;
	case singleplayer:
		strcpy(name, "resources/go_bg2.png");
		break;
	case twoplayer:
		strcpy(name, "resources/go_bg2.png");
		break;
	case about:
		strcpy(name, "resources/About.png");
		break;
	case startbranch:
		strcpy(name, "resources/startbranch.png");
		break;
	default:
		break;
	}
	
	//create_bitmap("sfa", 0, 0, 0, 0);
	al_acknowledge_resize(al_get_current_display());	//let gpu know that screen is resized 
	height = al_get_display_height(al_get_current_display());//height of window
	width = al_get_display_width(al_get_current_display());//width of window
	bg_image = create_bitmap(name, 0, 0, width, height);//create a new bg image
	if(_gamestate==gamestate::singleplayer||_gamestate==gamestate::twoplayer)
		Redraw();
	al_flip_display();
	configureBtn();
}
void savedata()
{
	FILE *infile = NULL;
	infile = fopen("data", "w+b");

	int x = int(playerturn);
	fwrite(&playerturn, sizeof(player_turn), 1, infile);
	for (int i = 0; i < 10; i++)
	{
		fwrite(array[i], sizeof(int), 10, infile);
		
	}
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++) {
			bool temp = getGroupBlock(i, j);
			int tm = gettoken(i, j);
			fwrite(&temp, sizeof(bool), 1, infile);
			fwrite(&tm, sizeof(int), 1, infile);
		}
	}
	fclose(infile);
	printf("saved.\n");
}
void loaddata()
{
	FILE *infile = NULL;
	infile = fopen("data", "r+b");
	fread(&playerturn, sizeof(player_turn), 1, infile);
	for (int i = 0; i < 10; i++)
	{
		fread(array[i], sizeof(int), 10, infile);
		/*for(int j=0;j<10;j++)
			nuts_images[i][j]=crea*/
	}
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++) {
			bool temp;
			int tm;
			fread(&temp, sizeof(bool), 1, infile);
			fread(&tm, sizeof(int), 1, infile);
			settoken(i, j, tm);
			setGroupBlock(i, j, blocktype::ONE, temp);
		}
	}
	//Redraw();
	fclose(infile);
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
			//printf("x:%f\n", mouse.posx);
			//printf("y:%f\n", mouse.posy);
			buttonSence();
			if(_gamestate==gamestate::singleplayer||_gamestate==gamestate::twoplayer)
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
	else if ((_gamestate==gamestate::twoplayer||_gamestate==gamestate::singleplayer)&&ev.type == ALLEGRO_EVENT_TIMER) {
		if (checkEnd(playerturn))
			end = true;
		printf("time is %.2f\n", ev.timer.count / 10.0f);
		//float temp = getTime() / 10;
		//print_text(ftoa(temp, 1, 2), 40, 24);
		//al_flip_display();
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
		else if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE)
		{


			pass(playerturn);

		}
		else if (ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE)
		{
			int temp = message_box("warning", "Do you want to save?", "");
			if (temp == 0)
			{
				savedata();
				
				changeScreen(gamestate::menu);
			}
			else
			{
				changeScreen(gamestate::menu);
			}
		}
		else if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
			savedata();
		}
		//else if(ev.keyboard.keycode==ALLEGRO_KEY_F1) //al_stop_sample()
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
	init_text();
}


//test proces
void changeScreen(gamestate gs)
{
	char bgname[30] = { 0 };
	destroy_bitmap(bg_image);
	switch (gs)
	{
	case menu:
		
		strcpy(bgname, "resources/Mainmenu.png");
		bg_image = create_bitmap(bgname, 0, 0, width, height);
		break;
	case singleplayer:
		strcpy(bgname, "resources/go_bg2.png");
		bg_image = create_bitmap(bgname, 0, 0, width, height);
		startTimer(playerturn);
		break;
	case twoplayer:
		strcpy(bgname, "resources/go_bg2.png");
		bg_image = create_bitmap(bgname, 0, 0, width, height);
		startTimer(playerturn);
		break;
	case about:
		strcpy(bgname, "resources/About.png");
		bg_image = create_bitmap(bgname, 0, 0, width, height);
		break;
	case startbranch:
		strcpy(bgname, "resources/startbranch.png");
		bg_image = create_bitmap(bgname, 0, 0, width, height);
		break;
	default:
		break;
	}
	_gamestate = gs;

	resize_screen();
}//mix

//check taht button is pressed
bool isButtonPressed(Button btn)
{
	if (mouse.posx >= btn.sti&&mouse.posx <= btn.endi)//check x position
	{
		if (mouse.posy >= btn.stj&&mouse.posy <= btn.endj)//check y position
			return true;
	}
	return false;
}
//button manager :D
void buttonSence()
{
	switch (_gamestate)//make sure that buttons in the exact scene is checked
	{
	case menu:
		if (isButtonPressed(_button[buttons::Startbtn]))
		{
			changeScreen(gamestate::startbranch);
			printf("Start.\n");
		}
		else if (isButtonPressed(_button[buttons::aboutbtn]))
		{
			changeScreen(gamestate::about);
			printf("About.\n");
		}
		else if (isButtonPressed(_button[buttons::exitbtn]))
		{
			int temp;
			temp=message_box("", "Are you Sure ?", "Warning");
			if (temp ==1)
				end = true;
			printf("Exit.\n");
		}
		
		break;

	case startbranch:
		if (isButtonPressed(_button[buttons::singleplayerbtn]))
		{
			changeScreen(gamestate::singleplayer);
			printf("single.\n");
		}
		else if (isButtonPressed(_button[buttons::twoplayerbtn]))
		{
			changeScreen(gamestate::twoplayer);
			printf("twopla\n");
		}
		else if (isButtonPressed(_button[buttons::backbtn]))
		{
			changeScreen(gamestate::menu);
			printf("About.\n");
		}
		else if (isButtonPressed(_button[buttons::loadbtn]))
		{
			loaddata();
			printf("game is loading.\n");
		}
		break;
	case singleplayer:
		
			break;
	case twoplayer:
		
		break;
	case about:
		 if (isButtonPressed(_button[buttons::backbtn]))
		{
			 changeScreen(gamestate::menu);
		}
		break;
	default:
		break;
	}
}
//configure button position based on screen size :D
void configureBtn()
{
	for (int i = 0; i < 3; i++)//main menu buttons
	{
		_button[i].sti = width / 21 * 8;
		_button[i].endi = width / 21 * 13;
		_button[i].stj = height / 20 * (4 * (i + 1) + 1);//these are formuls that we create buttons based on
		_button[i].endj = height / 20 * (4 * (i + 2) - 1) - 5;//these are formuls that we create buttons based on
	}
	_button[buttons::backbtn].sti = 0;				// this 
	_button[buttons::backbtn].endi = width / 21 * 6;	// is 
	_button[buttons::backbtn].stj = height / 20 * 16.5;//back btn
	_button[buttons::backbtn].endj = height;		//configure

	_button[buttons::singleplayerbtn].sti = width / 21 * 4.6f;	//this is 
	_button[buttons::singleplayerbtn].endi = width / 21 * 16.56f;//single 
	_button[buttons::singleplayerbtn].stj = height / 20 * 1;	//player btn
	_button[buttons::singleplayerbtn].endj = height / 20 * 8.3f;//configure

	_button[buttons::twoplayerbtn].sti = width / 21 * 6.4f;	//this is
	_button[buttons::twoplayerbtn].endi = width / 21 * 14.5f;//two
	_button[buttons::twoplayerbtn].stj = height / 20 * 10.5f;//player btn
	_button[buttons::twoplayerbtn].endj = height / 20 * 16.7f;//configure

	_button[buttons::loadbtn].sti = width / 21 * 8.6f;	//this is
	_button[buttons::loadbtn].endi = width / 21 * 12.38;//load
	_button[buttons::loadbtn].stj = height / 20 * 17;	//btn
	_button[buttons::loadbtn].endj = height / 20 * 18.33f;//configure	
}
int main()
{
	//al_set_new_display_option(ALLEGRO_SINGLE_BUFFER, 1, 1);
	_gamestate = gamestate::menu;
	komi(whitescore,blackscore, playerturn);
	//init seed
	srand(time(NULL));
	get_max_screen_size();
	inits();
	configureBtn();
	char name[] = "resources/Mainmenu.png";
	char audioname[] = "resources/bgf.wav";

	if (loadAuido(audioname))
	printf("Audio init.\n");
	if (playAudio(ALLEGRO_PLAYMODE_LOOP))
	printf("audio played.\n");

	bg_image = create_bitmap(name, 0, 0, width, height);
	initTimer();
	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	//registering event sources
	al_register_event_source(event_queue, al_get_display_event_source(al_get_current_display()));//display source
	al_register_event_source(event_queue, al_get_mouse_event_source());//mouse source
	al_register_event_source(event_queue, al_get_keyboard_event_source());//keyboard source	
	al_register_event_source(event_queue, al_get_timer_event_source(getTimer()));
	
	
	//printf("%f",al_get_timer_speed(getTimer()));
	//al_start_timer(getTimer());
	ALLEGRO_EVENT ev;
	//startTimer();
	//ev.timer.count = 0;
	/*if(al_get_timer_started(getTimer()))
	printf("%d\n",12 );
	else
	{
		printf("%d\n", 13);
	}*/
	//al_start_timer(getTimer());
	//changeScreen(gamestate::about);
	//main loop
	while (!end)
	{
		
		al_wait_for_event(event_queue, &ev);
		//printf("%d\n", getTime());
		event_manager(ev);
		

	}

	return 0;
}
