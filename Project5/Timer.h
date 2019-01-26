#pragma once
//check that time is passed trigger 
bool checktime(int playercolor);
//start timer
bool startTimer(int playercolor);
//initialize timer
bool initTimer();
//get timer counter
int getTime();
//will stop timer and reset it to zero
bool stopTimer();
//will check that is packs end or not
bool checkEnd(int playercolor);
//get player packs number
int getPacks(int playercolor);
//set player packs number used @save and load
void setPacks(int num, int playercolor);
//will give acces to timer of lib
ALLEGRO_TIMER *getTimer();