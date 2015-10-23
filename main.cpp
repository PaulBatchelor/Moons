#include <math.h>
#include <stdlib.h>
#include <iostream>

#ifdef __MACOSX_CORE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <string.h>
#endif

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

long g_width = 640;
long g_height = 480;

int moon_add(moon_base *mb, float radius, float theta, int note)
{
    if(mb->nmoons >= mb->max_moons) {
        fprintf(stderr, "Warning: max number of moons created!");
        return 0;
    }

    int id;
    mb->nmoons++;

    while(theta >= 2 * M_PI) theta -= 2 * M_PI;
    while(theta < 0) theta += 2 * M_PI;

    id = mb->nmoons - 1; 
    
    mb->moon[id].theta = theta;
    mb->moon[id].itheta = theta;
    mb->moon[id].radius = radius;
    mb->moon[id].time = 0;

    theta = fabs(theta) / (2.0 * M_PI);
    theta = floor(mb->scale->size * theta);
    mb->moon[id].note= (int)theta;
    fprintf(stderr, "the note is %d!, theta is %g\n", mb->moon[id].note, theta);

    return 0;
}

int moon_init(moon_base *mb)
{
    mb->speed = 10;
    mb->max_moons = MAX_MOONS;
    mb->nmoons = 0;
    mb->undo = 0;    
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
        output[i + 1] = md->sp->out[1];
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
        //audio.openStream( &oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames, &callme, &g_data, &options );
        audio.openStream( &oParams, NULL, MY_FORMAT, MY_SRATE, &bufferFrames, &callme, &g_data, &options );
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
        case 'u': 
            if(g_data.nmoons > 0) g_data.nmoons--; 
            g_data.pd.p[0] = 1;
            g_data.undo = 1;
            break;
        default:
            break;
    }
    glutPostRedisplay( );
}

void reshapeFunc( GLsizei w, GLsizei h )
{
    /* save the new window size */
    g_width = w; g_height = h;
    /* map the view port to the client area */
    glViewport( 0, 0, w, h );
    /* set the matrix mode to project */
    glMatrixMode( GL_PROJECTION );
    /* load the identity matrix */
    glLoadIdentity( );
    /* create the viewing frustum */
    gluPerspective( 45.0, (GLfloat) w / (GLfloat) h, 1.0, 45.0 );
    /* set the matrix mode to modelview */
    glMatrixMode( GL_MODELVIEW );
    /* load the identity matrix */
    glLoadIdentity( );
    /* position the view point */
    gluLookAt( 0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );
}

void initGfx()
{
    /* double buffer, use rgb color, enable depth buffer */
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    /* initialize the window size */
    glutInitWindowSize( g_width, g_height );
    /* set the window postion */
    glutInitWindowPosition( 100, 100 );
    /* create the window */
    glutCreateWindow( "simple" );
   
    /* set the idle function - called when idle */
    glutIdleFunc( idleFunc );
    /* set the display function - called when redrawing */
    glutDisplayFunc( displayFunc );
    /* set the reshape function - called when client area changes */
    glutReshapeFunc( reshapeFunc );
    /* set the keyboard function - called on keyboard events */
    glutKeyboardFunc( keyboardFunc );
    /* set the mouse function - called on mouse stuff */
    glutMouseFunc( mouseFunc );
    
    /* set clear color */
    glClearColor( 0, 0, 0, 1 );
    /* enable color material */
    glEnable( GL_COLOR_MATERIAL );
    /* enable depth test */
    glEnable( GL_DEPTH_TEST );
}


void mouseFunc( int button, int state, int x, int y )
{
    GLfloat depth = 0;
    GLdouble fX, fY, fZ;
    GLdouble  model[16], proj[16];
    GLint view[4];
    float rad, theta;
    if( button == GLUT_LEFT_BUTTON ) {
        /* when left mouse button is down */
        if( state == GLUT_DOWN ) {
            glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
            glMatrixMode (GL_MODELVIEW);
            glGetDoublev(GL_MODELVIEW_MATRIX, model);
            glGetDoublev(GL_PROJECTION_MATRIX, proj);
            glGetIntegerv(GL_VIEWPORT, view);
            printf("depth: %g", depth);
            gluUnProject(x, y, 0.8182, model, proj, view, &fX, &fY, &fZ);
            rad = sqrt((x*x) + (y*y));
            printf("x: %d, y: %d r: %g w: %d h: %d\n", x, y, rad, g_width, g_height);
            fY *= -1; 
            rad = sqrt((fX*fX) + (fY*fY));
            theta = atan(fY / fX);
            if(fX < 0 && fY < 0) theta -= M_PI;
            else if(fX < 0 && fY > 0) theta += M_PI;

            fprintf(stderr, "fX: %g fY: %g r: %g theta: %g\n", fX, fY, rad, theta);
            moon_add(&g_data, rad, theta, 2);
            glMatrixMode (GL_MODELVIEW);

        }
        else {

        }
    }
    else if ( button == GLUT_RIGHT_BUTTON )
    {
        /* when right mouse button down */
        if( state == GLUT_DOWN ) {
        }
        else {
        }
    }
    else {
    }
    
    glutPostRedisplay( );
}
