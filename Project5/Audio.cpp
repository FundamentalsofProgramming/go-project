#include"Audio.h"
#include<stdio.h>
#include<allegro5\allegro_audio.h>
#include<allegro5\allegro_acodec.h>
#include<allegro5\allegro.h>

ALLEGRO_SAMPLE *sample = NULL;
ALLEGRO_SAMPLE_INSTANCE *instance = NULL;
bool isplaying = false;

//bool isPlayingAudio()
//{
//	
//}
//LOAD Audio sample
bool loadAuido(char addres[])
{
	al_init_acodec_addon();
//	printf("lo.\n");
	sample = al_load_sample(addres);
	//reserve samples
	al_reserve_samples(1);
	//if sample is not empty
	if (sample)
	{
		return true;
		
	}
	return false;
}
//start playing audio
bool playAudio(ALLEGRO_PLAYMODE type)
{
	if (!sample)
		return false;
	//create an instance of sample
	instance = al_create_sample_instance(sample);
	if (instance )
	{
		//configure properties
		al_attach_sample_instance_to_mixer(instance, al_get_default_mixer());//attach an instance to mixer
		al_set_sample_instance_gain(instance, 1);
		al_set_sample_instance_pan(instance,0);
		al_set_sample_instance_speed(instance,1);//audio play speed
		al_set_sample_instance_playmode(instance, type);
		al_set_sample_instance_playing(instance, true);//play audio
	}
}
//pause audio
void stopAudio()
{
	al_set_sample_instance_playing(instance,!(al_get_sample_instance_playing(instance)));
}
//mute audio
void muteAudio()
{
	int a = al_get_sample_instance_gain(instance);
	if (a)
		al_set_sample_instance_gain(instance, 0);
	else
		al_set_sample_instance_gain(instance, 1);
}