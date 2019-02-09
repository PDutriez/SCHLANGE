// Libaudio Version 2.0 by dany
// Non blocking audio player

#include <SDL/SDL.h>
#include "libaudio.h"


// prototype for our audio callback
static void my_audio_callback(void *userdata, Uint8 *stream, int len);
// variable declarations
static Uint8 *audio_pos;           // global pointer to the audio buffer to be played
static Uint32 audio_len=0;         // remaining length of the sample we have to play. Initial value must be 0 !!

static Uint32 wav_length;          // length of our sample
static Uint8 *wav_buffer;          // buffer containing our audio file
static SDL_AudioSpec wav_spec;     // the specs of our piece of music

static int Status_flag=NO_INIT; 
static int inside_callback=0;

/*
** Sound Driver by dany
*/

//**************** For PI Board **************************************

//gcc simple1.c libaudio.c  -L./SDL/Library/ -lSDL -o simple1  (w/o libaudio precompiled)







//**************** For PC **************************************
// Next function is a TEST BENCH for Quick tests 
// As stand alone put your test code here and rename main1 to main
// gcc libaudio.c -I/usr/local/include -L/usr/local/lib -lSDL -o libaudio
// Run with ./libaudio

// As library rename main as main1 and link this file with your App (testaudio.c)
// gcc testaudio.c libaudio.c -I/usr/local/include -L/usr/local/lib -lSDL -lpthread -o testaudio
// Run with ./testaudio




int main1(int argc, char* argv[]){

	init_sound();
	
	set_file_to_play("Avicii - Wake Me Up .wav");
	
	play_sound();
	
	printf("Start Playing ..\n");

	while (player_status()!=FINISHED)
	{
		SDL_Delay(6000); 
		pause_sound();
		printf("Pause ..\n");
		SDL_Delay(1000); 
		play_sound();
		printf("Playing ..\n");
		
	}
	stop_sound();
	
	return(0);
}

int init_sound(void)
{
	
	// Initialize SDL.
	
	Status_flag=READY;
	
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
			return 1;
	else
	
			return 0;
}

// Define audio file to play
int	set_file_to_play(const char *music_path)
{
	
	
	Status_flag=STOPPED;
	
	/* Load the WAV */
	// the specs, length and buffer of our wav are filled
	if( SDL_LoadWAV(music_path, &wav_spec, &wav_buffer, &wav_length) == NULL ){
		
	  printf("Sound File %s Not found \n",music_path);
	  	
	  return 1;
	}
		
	// set the callback function
	wav_spec.callback = my_audio_callback;
	wav_spec.userdata = NULL;
	// set our global static variables
	audio_pos = wav_buffer; // copy sound buffer
	audio_len = wav_length; // copy file length
	
	/* Open the audio device */
	if ( SDL_OpenAudio(&wav_spec, NULL) < 0 ){
	  fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
	  exit(-1);
	}
	   
	return 0;
}

int play_sound(void)
{	
	
	if(Status_flag==FINISHED) // if audio file is has finished this command is not allowed
		return 1;
	
	Status_flag=PLAYING;
	
	/* Start playing */
	SDL_PauseAudio(0);

	    return 0;
}


int pause_sound(void)
{	
	
	if(Status_flag==FINISHED) // if audio file is has finished this command is not allowed 
		return 1;			  
		
	Status_flag=PAUSED;
	
	/* Stop playing (Paused)*/
	SDL_PauseAudio(1);
	
	return 0;
}

int stop_sound(void)
{
	   
// shut everything down

	if(Status_flag!=READY && Status_flag!=NO_INIT)   // Protect user from closing twice or when system is not initialized
	{
		while(inside_callback==1); // Ensure that we are not inside the callback before Locking the audio thread
		SDL_LockAudio();		   // Now its safe to lock the audio thread :) By dany
		SDL_CloseAudio();		   // So I Got the ax :))))) (salinger (4))
		SDL_FreeWAV(wav_buffer);   // Free audio buffer 	
		Status_flag=READY;		   // signal the FSM we are now back in the ready state	
	}
	
	return 0;
}

int end_play (void)
{
	player_status();	// Skip Finish state 	
	stop_sound();		// and Stop playing
	return(0);
}

int player_status(void)
{
	return(Status_flag);
}


//============================================================
// audio callback function
// here you have to copy the data of your audio buffer into the
// requesting audio buffer (stream)
// you should only copy as much as the requested length (len)
// =================================================================
void my_audio_callback(void *userdata, Uint8 *stream, int len) {
	
	inside_callback=1;		// Signal the outside world we are inside the callback (no safe to lock the audio thread now) 
	
	if (audio_len ==0)      // if nothing to play just return
	{
		
		Status_flag=FINISHED;  // End Of File
		inside_callback=0;     // Signal the outside world the callback has finished (now is safe to lock the audio thread) 
		
		return;
		
	}
	
	
	len = ( len > audio_len ? audio_len : len );
	//SDL_memcpy (stream, audio_pos, len); 					// simply copy from one buffer into the other
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another
	
	audio_pos += len;      // update file audio pointer and length
	audio_len -= len;
	
	inside_callback=0;    // Signal the outside world the callback has finished (now is safe to lock the audio thread) 
}


void debug_status()
{
	
	if(Status_flag==NO_INIT)
		printf(GREEN_TEXT "DEBUG NO_INIT\n" WHITE_TEXT );
	if(Status_flag==READY)
		printf(GREEN_TEXT "DEBUG READY\n" WHITE_TEXT );
	if(Status_flag==STOPPED)
		printf(GREEN_TEXT "DEBUG STOPPED\n" WHITE_TEXT );
	if(Status_flag==PAUSED)
		printf(GREEN_TEXT "DEBUG PAUSED\n" WHITE_TEXT );		
	if(Status_flag==PLAYING)
		printf(GREEN_TEXT "DEBUG PLAYING\n" WHITE_TEXT );
	if(Status_flag==FINISHED)
		printf(GREEN_TEXT "DEBUG FINISHED\n" WHITE_TEXT );	
	
	printf("audio_len: %d  \n",audio_len );		
		
		
		
		
		
		
		
		
		
		
		
}
