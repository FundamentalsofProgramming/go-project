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
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include<string.h>
#include"GameRules.h"
#include"Audio.h"
#include <stdlib.h>
#include "Group.h"
//#include"SaveAndLoad.h"//we are not using this lib because of some resons
#include<allegro5\timer.h>
#include"Timer.h"
#include<exception>//we used this for some tests
#include<conio.h>
#include<stdlib.h>

//struct button that holds our buttons properties
struct Button
{
	float sti, endi;//x coordinates
	float stj, endj;//y coordinates
};
Button _button[10];//an array of button in game note that you can increse it size base on buttons you have
//button enum for beauti and easy acces to struct array
enum buttons
{
	Startbtn=0,aboutbtn=1,exitbtn=2,singleplayerbtn=3,twoplayerbtn=4,backbtn=5,loadbtn=6
};
struct keyboard {//coordinates for hover and etc
	int posx;
	int posy;
}makan;
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
int aidebug[10][10] = { 0 };//used in charjahat
int ai_try = 0;//used in putPieces  suicide to limit ai try for random numbers
int ib = 0, jb = 0, iw = 0, jw = 0;//where ai want to put his decision
int array[10][10] = { 0 }; // 0 empty 1 white -1 black
int whitenuts = 40, blacknuts = 41;
int whitescore = 0, blackscore = 0;//keep scores of players
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
enum gamestate { menu, singleplayer, twoplayer, about, startbranch,GameOver }_gamestate;//mix
gamestate gs = menu;
ALLEGRO_BITMAP *turnshow = NULL;
ALLEGRO_BITMAP *hovershow = NULL;


//functions prototype
ALLEGRO_BITMAP *create_bitmap(char[], float, float, int, int);									//create an image
ALLEGRO_BITMAP *create_bitmap(const char image_name[], float x, float y, int width, int height);//on screen
void destroy_bitmap(ALLEGRO_BITMAP*);//destroy a bitmap
void ai(int [][10], int &, int &);//one of function of ai
//ALLEGRO_BITMAP *create_circle(float, float, float);
void buttonSence();//button manager
void configureBtn();//configure button coordinates based on screen size
bool isinrange(int &, int &);//check if mouse is in our board or not
void print_text(const char text[], int x, int y, int R=255 , int G=255 , int B=255 , const char font_name[]="arial.ttf" );
int message_box(const char*,
	const char*,
	const char*);
void resize_screen();//will resize screen and call redraw function
void Redraw(); //redraw the screen with array
void changeScreen(gamestate gs);//change the game state
void scoring(int array[][10], int &blackScore, int &whiteScore);//calculate players's scores whenever game over
void putPieces();//almost function and put white and black pieces on the board on game state

//show the gameover scene when game is ended
void gameover()
{
	playAudio(Audios::Gameover, ALLEGRO_PLAYMODE_ONCE);
	stopAudio(Audios::bgAu);
	char *temp=new char[8];
	char win[20] = "winner is ";
	changeScreen(gamestate::GameOver);
	scoring(array, blackscore, whitescore);
	char blacktext[20] = "BlackScore is:", whitetext[20] = "WhiteScore is:";
	itoa(blackscore, temp, 10);
	strcat(blacktext,temp );
	itoa(whitescore, temp, 10);
	strcat(whitetext, temp);
	strcat(whitetext, ".5");
	print_text(blacktext, width / 21 * 10, height / 20 * 10);
	print_text(whitetext, width / 21 * 10, height / 20 * 13);
	if (blackscore > whitescore) {
		strcat(win, "black");
	}
	else {
		strcat(win, "white");
	}
	
	print_text(win, width / 21 * 10, height / 20 * 15);

}
//update the console 
void UpdateConsole()
{
	system("CLS");
	printf("BlackPacks:%d\n", getPacks(black));
	printf("WhitePacks:%d\n", getPacks(white));
	printf("WhiteNuts:%d\n", whitenuts);//debug process
	printf("BlackNuts:%d\n", blacknuts);//test process
	playAudio(Audios::clockAu, ALLEGRO_PLAYMODE::ALLEGRO_PLAYMODE_ONCE);
	printf("Timer Count:%.1f\n", getTime()/10.0f);
}
//show the turn player on the top left
void turnShow()
{
	destroy_bitmap(turnshow);
	switch (playerturn)
	{
	case black:
		turnshow= create_bitmap("blackPiece.png", 0, 0, width / 21 * 1, height / 20 * 1);
		break;
	case white:
		turnshow = create_bitmap("whitePiece.png", 0, 0, width / 21 * 1, height / 20 * 1);
		break;
	default:
		break;
	}
}
//not working yet
void deadones(int array[][10],player_turn playerturn) { // mohre haye morde
	int cpyarr[10][10];
	for(int i=0;i<=9;i++)
		for (int j = 0; j <= 9; j++) {
			cpyarr[i][j] = array[i][j];
			if (cpyarr[i][j] == int(playerturn)|| cpyarr[i][j] == 0)
				cpyarr[i][j] = int(playerturn)*-1;
		}
}
//give an extra score to player who plays first
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
		gameover();

	if (passcounterb > 1 && passcounterw == 0)//black pass but white doesn't
	{
		passcounterb = 0;
		passcounterw = 0;
	}
	if (passcounterw > 1 && passcounterb == 0)//white pass but black doesn't
	{
		passcounterw = 0;
		passcounterb = 0;
	}
	if (_gamestate == gamestate::singleplayer)//this part will be activate in single mode
	{
		if (playerturn == player_turn::white)
		{
			scoring(array, blackscore, whitescore);
			if (blackscore > whitescore )
				pass(black);
			else
			{
				putPieces();
			}
		}
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
	whitescore = 0;
	blackscore = 0;
	komi(whitescore, blackscore, white);
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
//find a black pieces whenever you put a white pieces
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

//this is an extra mode for ai and it will make eyes if you don't put near black pieces
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
	}

}



//return token of the first white's group that it finds
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
		return 0;
	}
	else
	{
		iw = i;
		jw = j;
		return gettoken(i, j);
	}
		
}
//it's a recersive func for offensive mode of ai
int offrecersive(int i,int j,int array[][10]) { // mire mohre ro capture mikone  
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
//try to capture white player when ai is not defending
void offensive(int array[][10], int &ib, int &jb, int startpointi, int startpointj)
{
	int i, j,y=0,x=0;
	int token;
	token = findwhitegp(iw, jw,startpointi,startpointj);
	y= offrecersive(iw, jw, array);
	if (!y) {
		x = jw + 1;
		offensive(array, iw, jw,iw,x);
	}

}
//find a place in a group of black Pieces with more liberty
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
			if (array[ib - 1][jb + 1] == 0)
			{
				counterrast++;
			}
			if (array[ib + 1][jb + 1] == 0)
			{
				counterrast++;
			}
			if (array[ib][jb + 2] == 0)
			{
				counterrast++;
			}
		}
	}
	int sw2 = 0;
	if (overflow(ib, 0, 9) && overflow(jb - 1, 0, 9) && sw2 == 0) {
		if (array[ib][jb - 1] == 0)
		{
			ichap = ib;
			jchap = jb - 1;
			if (array[ib - 1][jb - 1] == 0)
			{
				counterchap++;
			}
			if (array[ib + 1][jb - 1] == 0)
			{

				counterchap++;
			}
			if (array[ib][jb - 2] == 0)
			{
				counterchap++;
			}

		}
	}
	int sw3 = 0;
	if (overflow(ib + 1, 0, 9) && overflow(jb, 0, 9) && sw3 == 0) {
		if (array[ib + 1][jb] == 0)
		{
			ipayin = ib + 1;
			jpayin = jb;
			if (array[ib + 1][jb + 1] == 0)
			{
				counterpayin++;
			}
			if (array[ib + 1][jb - 1] == 0)
			{
				counterpayin++;
			}
			if (array[ib + 2][jb] == 0)
			{
			counterpayin++;
			
			}
			
		}
	}
	if (overflow(ib - 1, 0, 9) && overflow(jb + 1, 0, 9)) {
		if (array[ib - 1][jb] == 0)
		{
			ibala = ib - 1;
			jbala = jb;
			if (array[ib - 1][jb + 1] == 0)
			{
				counterbala++;
			}
			else if (array[ib - 1][jb - 1] == 0)
			{
				counterbala++;
			}
			else if (array[ib - 2][jb] == 0)
			{
				counterbala++;
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
		else { // masaln countere bala va payin dotashoon 1beshan
			//ib = rand() % 10;
			//jb = rand() % 10;
			offensive(array, ib, jb, 0, 0);
			return 1;			
			//eyemaker(array, ib, jb, 1, 1 );
			
		}
	}
}
//this is main ai func and it decide between two logics
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
//will be called @ neighbourprio
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
		strcpy(name, "blackPiece.png");
	}
	else {
		strcpy(name, "whitePiece.png");
	}
	//set circle center poin in position
	x -= r;
	y -= r;
	r *= 2;
	image = create_bitmap(name, x, y, r, r);

	return image;
}
//it's a recersive func for capturing
int check(int i, int j, player_turn playerturn, int hosti, int hostj, int debug[][20]) {
	int result[4] = { 0 };//save directions are free or no and compare them 
	if (!(i + 1 == hosti && j == hostj))//check that this side is parent side (this prevent infinity loop checks)
	{
		if (i + 1 >= 10 || j >= 10)//check over flow
		{
			result[0] = int(playerturn)*(-1);
		}
		else if (debug[i + 1][j] != 0)//check if this place is checked before or not
		{
			result[0] = int(playerturn)*(-1);

		}
		else if (array[i + 1][j] == int(playerturn))//if it's groupmate or not
		{
			debug[i + 1][j] = 1;
			setGroupBlock(i + 1, j, blocktype::ONE, true); //mix
			result[0] = check(i + 1, j, playerturn, i, j, debug);

		}
		else if (array[i + 1][j] == int(playerturn)*(-1))//if it's enemy
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
//find enemies of putted piece
bool checkenemy(int i, int j, player_turn playerturn) {
	bool capture = false;
	player_turn pt = playerturn == white ? black : white;
	int debug[20][20] = { 0 };
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
				capture = true;
				//printf("captured.\n");
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
				capture = true;
				//printf("captured.\n");
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
				capture = true;
				//printf("captured.\n");
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
				capture = true;
				//printf("captured.\n");
			}
			else
				setGroupBlock(i - 1, j, blocktype::ALL, false);
		}
	}
	return capture;
}
//check whether its suicide or not
bool suicide(int i, int j, player_turn playerturn)
{
	int debug[20][20] = { 0 };
	if (check(i, j, playerturn, i, j, debug))
	{
		setGroupBlock(i, j, blocktype::ALL, false);
		return true;
	}
	else
	{
		setGroupBlock(i, j, blocktype::ALL, false);
		return false;
	}
}

void setCircle(int &i, int &j)
{
	circle.r = width / 11 > height / 11 ? height / 11 : width / 11;//this will sure that our r is not greater than our square
	circle.r *= .5f;
	circle.center_x = j * (width / 11);
	circle.center_y = i * (height / 11);
}
//eck mouse is in circle range
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
//
void putPieces()
{
	{
		
		int i, j;
		setAndis(i, j);//caculate which coordinate has been clicked
		iw = i - 1;
		jw = j - 1;
		if (_gamestate == gamestate::singleplayer)
		{
			if (playerturn == player_turn::black)
			{
				if (ai_try >= 500)
					pass(playerturn);
				
				checkpriority(array, i, j, ib, jb);
				if (suicide(ib, jb, black))
				{
					ib = rand() % 10;
					jb = rand() % 10;
					ai_try++;
				}
				else
					ai_try = 0;
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

						bool test = false;//if a group is captured it will redraw and delte them
						array[i][j] = int(playerturn);
						if (checkenemy(i, j, playerturn))
						{
							test = true;
						}
						if (!suicide(i, j, playerturn))
						{
							switch (playerturn)
							{
							case black:
								blacknuts--;
								
								break;
							case white:
								whitenuts--;
								break;
							default:
								break;
							}
							nuts_images[i][j] = create_circle(circle.center_x, circle.center_y, circle.r, playerturn);
							arrayset(array, i, j);
							playAudio(Audios::putAu,ALLEGRO_PLAYMODE_ONCE);
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
							array[i][j] = 0;
							//printf("suicide.\n");
						}
				
					}
					else
					{
						//	ai(playerturn, ib, jb);
					}
				}
			}
		}
		switch (playerturn)
		{
		case black:
			if (blacknuts <= 0)
			{
				pass(playerturn);
				return;
			}
			break;
		case white:
			if (whitenuts <= 0)
			{
				pass(playerturn);
				return;
			}
			break;
		default:
			break;
		}
		if (_gamestate == gamestate::singleplayer)
		{
			if (playerturn == black)
			{
				putPieces();
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
void print_text(const char text[], int x, int y, int R , int G , int B , const char font_name[] ) {
	ALLEGRO_FONT *font = al_load_ttf_font(font_name, width/21, 0);
	al_draw_text(font, al_map_rgb(R, G, B), x, y, ALLEGRO_ALIGN_CENTER, text);
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
//will resize screen and redraw 
void resize_screen()
{

	destroy_bitmap(bg_image);//destroy previos bg
	char name[30];
	switch (_gamestate)
	{
	case menu:
		strcpy(name, "Mainmenu.png");
		break;
	case singleplayer:
		strcpy(name, "go_bg2.png");
		break;
	case twoplayer:
		strcpy(name, "go_bg2.png");
		break;
	case about:
		strcpy(name, "About.png");
		break;
	case startbranch:
		strcpy(name, "startbranch.png");
		break;
	case GameOver:
		strcpy(name, "GameOver.png");
		//gameover();
		break;
	default:
		break;
	}
	al_acknowledge_resize(al_get_current_display());	//let gpu know that screen is resized 
	height = al_get_display_height(al_get_current_display());//height of window
	width = al_get_display_width(al_get_current_display());//width of window
	bg_image = create_bitmap(name, 0, 0, width, height);//create a new bg image
	if(_gamestate==gamestate::singleplayer||_gamestate==gamestate::twoplayer)
		Redraw();
	al_flip_display();
	configureBtn();
}
//show a circle when you hover with keyboard
void showHover()
{
	destroy_bitmap(hovershow);
	int x, y,r;
	x = (makan.posx+1)*width / 11 - .5f;
	y = (makan.posy+1)*height / 11 - .5f;
	x -= circle.r;
	y -= circle.r;
	r = circle.r * 2;
	hovershow = create_bitmap("hover.png", x, y, r, r);
	
	resize_screen();
	turnShow();
}

void savedata()
{
	FILE *infile = NULL;
	infile = fopen("data", "w+b");

	//int x = int(playerturn);
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
	int temp = getPacks(black);
	fwrite(&temp, sizeof(int), 1, infile);
	temp = getPacks(white);
	fwrite(&temp, sizeof(int), 1, infile);
	fflush(infile);
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
	int temp;
	fread(&temp, sizeof(int), 1, infile);
	setPacks(temp, black);
	fread(&temp, sizeof(int), 1, infile);
	setPacks(temp, white);
	//Redraw();
	fclose(infile);
}
//control events
void event_manager(ALLEGRO_EVENT ev)
{
		//check mouse events
	if (ev.type == ALLEGRO_EVENT_MOUSE_AXES ||
		ev.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
		redraw = true;
		
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
			buttonSence();
			if (_gamestate == gamestate::singleplayer || _gamestate == gamestate::twoplayer)
			{
				switch (playerturn)
				{
				case black:
					if (blacknuts <= 0)
					{
						pass(playerturn);
						return;
					}
					break;
				case white:
					if (whitenuts <= 0)
					{
						pass(playerturn);
						return;
					}
					break;
				default:
					break;
				}
				putPieces();//call put function
			}
		}
		
	}
	else if ((_gamestate==gamestate::twoplayer||_gamestate==gamestate::singleplayer)&&ev.type == ALLEGRO_EVENT_TIMER) {
		if (checkEnd(playerturn))
		{
			playAudio(Audios::Gameover, ALLEGRO_PLAYMODE_ONCE);
			al_rest(3);
			end = true;
		}
		//printf("time is %.2f\n", ev.timer.count / 10.0f);
		if (ev.timer.count < 200)
		{
			if (ev.timer.count % 10 == 0)
			{
				UpdateConsole();
			}
		}
		else if(ev.timer.count<250)
		{
			if (ev.timer.count % 5 == 0)
				UpdateConsole();
		}
		else if (ev.timer.count < 280)
		{
			if (ev.timer.count % 2 == 0)
				UpdateConsole();
		}
		else if (ev.timer.count == 300)
		{
			playAudio(Audios::packEnd, ALLEGRO_PLAYMODE::ALLEGRO_PLAYMODE_ONCE);
		}
		else
		{
			if (ev.timer.count % 1 == 0)
				UpdateConsole();
		}
		
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
		switch (_gamestate)
		{
		case menu:
			break;
		case singleplayer:
		case twoplayer:
			if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE)
			{


				pass(playerturn);

			}
			else if (ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE)
			{

				if (message_box("warning", "are you want to save?", "") == 1)
				{
					savedata();
					changeScreen(gamestate::menu);
				}
				else
				{
					changeScreen(gamestate::menu);
					//printf("else\n");
				}
				stopTimer();

			}
			else if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
				savedata();
			}
		else if (ev.keyboard.keycode == ALLEGRO_KEY_UP)
			{
				if (overflow(makan.posy, 1, 9))
				{
					makan.posy--;
					showHover();
				}
			}
		else if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
				if (overflow(makan.posy, 0, 8))
				{
					makan.posy++;
					showHover();
				}
			}
		else if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
				if (overflow(makan.posx, 0, 8))
				{
					makan.posx++;
					showHover();
				}
			}
		else if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) {
				if (overflow(makan.posx, 1, 9))
				{
					makan.posx--;
					showHover();
				}
			}
		else if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
				switch (playerturn)
				{
				case black:
					if (blacknuts <= 0)
					{
						pass(playerturn);
						return;
					}
					break;
				case white:
					if (whitenuts <= 0)
					{
						pass(playerturn);
						return;
					}
					break;
				default:
					break;
				}
				mouse.posx = ((makan.posx + 1)*width) / 11 - .5f;
				mouse.posy = ((makan.posy + 1)*height) / 11 - .5f;
				putPieces();
			}
			break;
		case about:
			if (ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE)
				changeScreen(gamestate::menu);
			break;
		case startbranch:
			if (ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE)
				changeScreen(gamestate::menu);
			break;
		case GameOver:
			break;
		default:
			break;
		}
		//close process
		if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
		{
			al_destroy_display(al_get_current_display());
			end = true;
		} 
		else if (ev.keyboard.keycode == ALLEGRO_KEY_M)
		{
			muteAudio(Audios::bgAu);
		}
		
	}
	if (redraw) {
		redraw = false;
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
		
		strcpy(bgname, "Mainmenu.png");
		bg_image = create_bitmap(bgname, 0, 0, width, height);
		break;
	case singleplayer:
		strcpy(bgname, "go_bg2.png");
		bg_image = create_bitmap(bgname, 0, 0, width, height);
		startTimer(playerturn);
		turnShow();
		break;
	case twoplayer:
		strcpy(bgname, "go_bg2.png");
		bg_image = create_bitmap(bgname, 0, 0, width, height);
		startTimer(playerturn);
		turnShow();
		break;
	case about:
		strcpy(bgname, "About.png");
		bg_image = create_bitmap(bgname, 0, 0, width, height);
		break;
	case startbranch:
		strcpy(bgname, "startbranch.png");
		bg_image = create_bitmap(bgname, 0, 0, width, height);
		break;
	case GameOver:
		strcpy(bgname, "GameOver.png");
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
			//printf("Start.\n");
		}
		else if (isButtonPressed(_button[buttons::aboutbtn]))
		{
			changeScreen(gamestate::about);
			//printf("About.\n");
		}
		else if (isButtonPressed(_button[buttons::exitbtn]))
		{
			int temp;
			temp=message_box("", "Are you Sure ?", "Warning");
			if (temp ==1)
				end = true;
			//printf("Exit.\n");
		}
		
		break;

	case startbranch:
		if (isButtonPressed(_button[buttons::singleplayerbtn]))
		{
			changeScreen(gamestate::singleplayer);
			//printf("single.\n");
		}
		else if (isButtonPressed(_button[buttons::twoplayerbtn]))
		{
			changeScreen(gamestate::twoplayer);
			//printf("twopla\n");
		}
		else if (isButtonPressed(_button[buttons::backbtn]))
		{
			changeScreen(gamestate::menu);
			//printf("About.\n");
		}
		else if (isButtonPressed(_button[buttons::loadbtn]))
		{
			loaddata();
			printf("game loaded.\n");
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
	makan.posx = 5;
	makan.posy = 5;
	_gamestate = gamestate::menu;
	//init seed
	srand(time(NULL));
	get_max_screen_size();
	inits();
	configureBtn();
	char name[] = "Mainmenu.png";
	//char audioname[] = "bgf.wav";
	loadAuido("gameoverCH.wav", Audios::Gameover);
	loadAuido("tick2.ogg", Audios::putAu);
	loadAuido("clocktick.wav", Audios::clockAu);
	loadAuido("packEnd.wav", Audios::packEnd);
	if (loadAuido("bgf.wav",Audios::bgAu))
	//printf("Audio init.\n");
	if (playAudio(Audios::bgAu,ALLEGRO_PLAYMODE_LOOP))
	//printf("audio played.\n");

	bg_image = create_bitmap(name, 0, 0, width, height);
	initTimer();
	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	//registering event sources
	al_register_event_source(event_queue, al_get_display_event_source(al_get_current_display()));//display source
	al_register_event_source(event_queue, al_get_mouse_event_source());//mouse source
	al_register_event_source(event_queue, al_get_keyboard_event_source());//keyboard source	
	al_register_event_source(event_queue, al_get_timer_event_source(getTimer()));
	
	ALLEGRO_EVENT ev;
	//main loop
	while (!end)
	{
		
		al_wait_for_event(event_queue, &ev);
		//printf("%d\n", getTime());
		event_manager(ev);
		

	}
	
	printf("white score is: %d", whitescore);
	printf("black score is: %d", blackscore);

	return 0;
}