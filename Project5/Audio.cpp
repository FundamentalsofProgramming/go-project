#include"Audio.h"
#include<stdio.h>
#include<allegro5\allegro_audio.h>
#include<allegro5\allegro_acodec.h>
#include<allegro5\allegro.h>
struct audio
{
	ALLEGRO_SAMPLE *sample = NULL;
	ALLEGRO_SAMPLE_INSTANCE *instance = NULL;
	
};
bool ismute = false;
ALLEGRO_SAMPLE *sample = NULL;
ALLEGRO_SAMPLE_INSTANCE *instance = NULL;
bool isplaying = false;
audio audios[6];

//bool isPlayingAudio()
//{
//	
//}
//LOAD Audio sample
bool loadAuido(char addres[],Audios Au)
{
	al_init_acodec_addon();
//	printf("lo.\n");
	audios[Au].sample = al_load_sample(addres);
	sample = audios[Au].sample;
	//reserve samples
	al_reserve_samples(1);
	//if sample is not empty
	if (sample)
	{
		return true;
		
	}
	return false;
}
bool loadAuido(const char addres[],Audios Au)
{
	al_init_acodec_addon();
	//	printf("lo.\n");
	audios[Au].sample = al_load_sample(addres);
	sample = audios[Au].sample;
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
bool playAudio(Audios Au,ALLEGRO_PLAYMODE type)
{
	switch (Au)
	{
	
	case clockAu:
		ismute = false;
		break;
	case Gameover:
		ismute = false;
		break;
	default:
		break;
	}
	if (!audios[Au].sample)
		return false;
	audios[Au].instance= al_create_sample_instance(audios[Au].sample);
	instance = audios[Au].instance;
	//create an instance of sample
	//instance = al_create_sample_instance(audios[Au].sample);
	if (instance )
	{
		if (!ismute)
		{
			//configure properties
			al_attach_sample_instance_to_mixer(instance, al_get_default_mixer());//attach an instance to mixer
			al_set_sample_instance_gain(instance, 1);
			al_set_sample_instance_pan(instance, 0);
			al_set_sample_instance_speed(instance, 1);//audio play speed
			al_set_sample_instance_playmode(instance, type);
			al_set_sample_instance_playing(instance, true);//play audio
		}
	}
}
//pause audio
void stopAudio(Audios Au)
{
	instance = audios[Au].instance;
	al_set_sample_instance_playing(instance,!(al_get_sample_instance_playing(instance)));
}
//mute audio
void muteAudio(Audios Au)
{
	instance = audios[Au].instance;
	int a = al_get_sample_instance_gain(instance);
	switch (Au)
	{
	case clockAu:
		break;
	case Gameover:
		break;
	
	default:
		
		if (a)
		{
			ismute = true;
			al_set_sample_instance_gain(instance, 0);
		}
		else
		{
			ismute = false;
			al_set_sample_instance_gain(instance, 1);
		}
		break;
		break;
	}
	
}