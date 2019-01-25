#pragma once
bool checktime(int playercolor);
bool startTimer(int playercolor);
bool initTimer();
int getTime();
bool stopTimer();
bool checkEnd(int playercolor);
ALLEGRO_TIMER *getTimer();