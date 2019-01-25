#include<allegro5\timer.h>
#include<allegro5\altime.h>
#include<stdio.h>
int blackpacks = 3, whitepacks = 3;
ALLEGRO_TIMER *timer=NULL;
bool endTime = false;
//return *timer for external acces
ALLEGRO_TIMER *getTimer()
{
	return timer;
}



bool startTimer(int playercolor)
{
	//al_set_timer_count(timer, 0); //set count time 0
	al_stop_timer(timer);
	switch (playercolor)
	{
	case 1://white
		if (whitepacks != 0)
			al_start_timer(timer);
		else
			endTime = true;
		break;
	case -1: //black
		if (blackpacks != 0)
			al_start_timer(timer);
		else
			endTime = true;
		break;
	default:
		break;
	}
	return true;
}
//will stop timer and force it to set zero
bool stopTimer()
{
	while (al_get_timer_count(timer)!=0)//make sure that it's zero
	{
		al_set_timer_count(timer, 0);
		//printf("Timer count after stop is:%d\n", al_get_timer_count(timer));
	}
	
	al_stop_timer(timer);
	//printf("timer is stopped after stop:%d\n", al_get_timer_started(timer));
	return true;
}

//check timer value and packs
bool checktime(int playercolor) 
{
	if (al_get_timer_count(timer) >= 300) //checl if its more than 80(optional)
	{
		printf("Timed pass.\n");
		switch (playercolor)
		{
		case 1:
			whitepacks--;
			printf("white:%d\n", whitepacks);
			stopTimer();
			if (endTime)// check if there is packet or not
				return true;
			else
			{
				startTimer(playercolor);
			}
			break;
		case -1:
			blackpacks--;
			printf("black:%d\n", blackpacks);
			stopTimer();
			if (endTime)
				return true;
			else
				startTimer(playercolor);
			break;
		default:
			break;
		}
		return false;
	}
	//return false;
}
bool checkEnd(int playercolor)
{
	checktime(playercolor);
	return endTime;
}
int getTime()
{
	if (timer)
	{
		return al_get_timer_count(timer);

	}
}
bool initTimer()
{
	timer = al_create_timer(0.1);
	if (timer)
	{
		return true;
	}
	return false;
}
