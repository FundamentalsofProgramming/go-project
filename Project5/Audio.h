#pragma once
#include<allegro5\allegro_audio.h>
enum Audios
{
	bgAu = 0, putAu=1, btnAu=2,clockAu=3,packEnd=4,Gameover=5
};
//load sample for instance
bool loadAuido(char addres[], Audios Au);
//an overload of above func
bool loadAuido(const char addres[], Audios Au);
//play audio instance
bool playAudio(Audios Au, ALLEGRO_PLAYMODE type);
//stop playing audio instance
void stopAudio(Audios Au);
//will mute audio
void muteAudio(Audios Au);