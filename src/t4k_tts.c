/*
Compile using folowing command
gcc -Wall -o speak speak.c -lespeak -I/usr/include/espeak/ 
     -D_GNU_SOURCE=1 -D_REENTRANT -I/usr/include/SDL  -lSDL
*/



#include "t4k_globals.h"
#include "t4k_common.h"

#include <speak_lib.h>
#include "SDL_thread.h"




//TTS Thread function
int tts_thread_func(void *arg)
{
	espeak_POSITION_TYPE position_type = POS_CHARACTER;
	tts_argument recived = *((tts_argument*)(arg));
	fprintf(stderr,"\nSpeaking : %s - %d",recived.text,recived.interrupt);
	
	
	if (recived.interrupt == INTERRUPT)
		T4K_Tts_cancel();
	else
		T4K_Tts_wait();
	
	int Size = strlen(recived.text)+1;
	espeak_Synth(recived.text, Size, 0, position_type, 0,	espeakCHARS_AUTO,0, NULL);	
	espeak_Synchronize();
	return 1;
}

void T4K_Tts_cancel()
{
	espeak_Cancel();
}

void T4K_Tts_wait()
{
	while (espeak_IsPlaying() && tts_thread)
		{};
	SDL_Delay(30); 
}

//This function should be called at begining 
void T4K_Tts_init()
{
	espeak_Initialize(AUDIO_OUTPUT_PLAYBACK, 500, NULL, 0 );
}

//Used to set person in TTS. in the case of espeak we will set language by this.
void T4K_Tts_set_voice(char voice_name[]){
	espeak_SetVoiceByName(voice_name);
}


//Stop the speech if it is working
void T4K_Tts_stop(){
	extern SDL_Thread *tts_thread;
	if (tts_thread)
    {
		SDL_KillThread(tts_thread);
		tts_thread = NULL;
        espeak_Cancel();
    }
}


//TTS Parameters
void T4K_Tts_set_volume(int volume){
espeak_SetParameter(espeakVOLUME,2*volume,0);
}

void T4K_Tts_set_rate(int rate){
espeak_SetParameter(espeakRATE,(3.7*rate)+80,0);
}

void T4K_Tts_set_pitch(int pitch){
espeak_SetParameter(espeakPITCH,pitch,0);
}

void T4K_Tts_say(int rate,int pitch,int interrupt, const char* text, ...){
	extern SDL_Thread *tts_thread;
	tts_argument data_to_pass;
	
	T4K_Tts_set_rate(rate);
    T4K_Tts_set_pitch(pitch);

	//Getting the formated text
	char to_say[1000];
	va_list list;
	va_start(list,text);
	vsprintf(to_say,text,list);
	va_end(list);
	
	
	data_to_pass.interrupt = interrupt;
	strcpy(data_to_pass.text,to_say);
	
	//Calling threded function to say.	
	tts_thread = SDL_CreateThread(tts_thread_func, &data_to_pass);
}	
