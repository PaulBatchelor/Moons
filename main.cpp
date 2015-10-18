#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <string.h>

#include "RtAudio.h"
#include "draw.h"

extern "C" {
#include "soundpipe.h"
#include "sporth.h"
}

#include "base.h"
#include "audio.h"

using namespace std;

// our datetype
#define SAMPLE float
// corresponding format for RtAudio
#define MY_FORMAT RTAUDIO_FLOAT32
// sample rate
#ifndef MY_SRATE
#define MY_SRATE 48000
#endif
// number of channels
#define MY_CHANNELS 2
// for convenience
#define MY_PIE 3.14159265358979
#define BUFSIZE 1024
#define FFTSIZE 1024
    
moon_base g_data;
RtAudio audio;

int moon_init(moon_base *mb)
{
    mb->theta = 0;
    mb->init = 1;
    return 0;
}


int callme( void * outputBuffer, void * inputBuffer, unsigned int numFrames,
            double streamTime, RtAudioStreamStatus status, void * data )
{
    moon_base *md = (moon_base *) data;
    SAMPLE * input = (SAMPLE *)inputBuffer;
    SAMPLE * output = (SAMPLE *)outputBuffer;

    for( int i = 0; i < numFrames * MY_CHANNELS; i+=2 )
    {
        moon_sound_compute(md);
        output[i] = md->sp->out[0];
        output[i + 1] = md->sp->out[0];
    }
    
    return 0;
}

void displayFunc( )
{
    moon_draw(&g_data);
}

static void stop_audio() 
{
    if( audio.isStreamOpen() )
    {
        audio.stopStream();
        audio.closeStream();
    }

}

int main( int argc, char ** argv )
{

    unsigned int bufferFrames = BUFSIZE;
    
    if( audio.getDeviceCount() < 1 )
    {
        cout << "no audio devices found!" << endl;
        exit( 1 );
    }
    
    /* Init sound engine*/
    
    g_data.sr = MY_SRATE; 
    moon_init(&g_data);
    moon_sound_init(&g_data); 

    /* initialize GLUT */
    glutInit( &argc, argv );
    /* init gfx */
    initGfx();
    
    audio.showWarnings( true );
    
    RtAudio::StreamParameters iParams, oParams;
    iParams.deviceId = audio.getDefaultInputDevice();
    iParams.nChannels = 1;
    iParams.firstChannel = 0;
    oParams.deviceId = audio.getDefaultOutputDevice();
    oParams.nChannels = MY_CHANNELS;
    oParams.firstChannel = 0;
    
    RtAudio::StreamOptions options;
    
   
    try {
        audio.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames, &callme, &g_data, &options );
    }
    catch( RtError& e )
    {
        cout << e.getMessage() << endl;
        exit( 1 );
    }
    
    try {
        /* start stream */
        audio.startStream();
        
        /* let GLUT handle the current thread from here */
        glutMainLoop();
        
        /* stop the stream. */
        audio.stopStream();
    }
    catch( RtError& e )
    {
        cout << e.getMessage() << endl;
        goto cleanup;
    }

    cleanup: 

    return 0;
}

int moon_clean(moon_base *mb) 
{
    stop_audio();
    moon_sound_destroy(mb);
    return 0;
}

void keyboardFunc( unsigned char key, int x, int y )
{
    switch(key) {
        case 'q': 
            moon_clean(&g_data);
            exit(1);
            break;
        default:
            break;
    }
    glutPostRedisplay( );
}
