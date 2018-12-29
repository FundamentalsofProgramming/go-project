#include"Audio.h"
#include<stdio.h>
#include<allegro5\allegro_audio.h>
#include<allegro5\allegro_acodec.h>
#include<allegro5\allegro.h>

ALLEGRO_SAMPLE *sample = NULL;
ALLEGRO_SAMPLE_INSTANCE *instance = NULL;
bool isplaying = false;


bool loadAuido(char addres[])
{
	al_init_acodec_addon();
	printf("lo.\n");
	sample = al_load_sample(addres);
	al_reserve_samples(1);
	if (sample)
	{
		return true;
		
	}
	return false;
}

bool playAudio(ALLEGRO_PLAYMODE type)
{
	if (!sample)
		return false;
	instance = al_create_sample_instance(sample);
	if (instance )
	{
		//al_play_sample_instance(instance);
		al_play_sample(sample, 1, 0, 1, type, NULL);
	}
	/*while (al_get_sample_instance_playing(instance))
	{
		printf("IS playing.\n");
	}*/
	
}
bool stopAudio()
{
	//al_stop_sample()
	return 0;
}