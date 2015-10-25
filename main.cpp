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

int moon_init(moon_base *mb)
{
    mb->speed = 10;
    moon_cluster_create(&mb->satellites, MAX_MOONS);
    moon_cluster_create(&mb->ripples, MAX_RIPPLES);
    rstack_init(&mb->rstack);
    mb->undo = 0;
    return 0; 
}

int moon_cluster_create(moon_cluster *mc, unsigned int max_moons)
{
    mc->moon = (moon_circle *)malloc(sizeof(moon_circle) * max_moons);
    mc->nmoons = 0;
    mc->max_moons = max_moons;
    return 0;
} 

int moon_cluster_destroy(moon_cluster *mc)
{
    free(mc->moon);
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

static void set_scale(moon_base *mb, int scale_num) 
{
    SPFLOAT *scale = mb->scale->tbl;
    fprintf(stderr, "Switching to scale %d\n", scale_num);
    switch(scale_num) {
        case 1:
            scale[0] = 62;
            scale[1] = 67;
            scale[2] = 69;
            scale[3] = 74;
            scale[4] = 76;
            scale[5] = 78;
            scale[6] = 85;
            break;
        case 2:
            scale[0] = 60;
            scale[1] = 65;
            scale[2] = 67;
            scale[3] = 72;
            scale[4] = 74;
            scale[5] = 76;
            scale[6] = 81;
            break;
        case 3:
            scale[0] = 62;
            scale[1] = 65;
            scale[2] = 69;
            scale[3] = 72;
            scale[4] = 76;
            scale[5] = 79;
            scale[6] = 81;
            break;
        case 4:
            scale[0] = 58;
            scale[1] = 65;
            scale[2] = 72;
            scale[3] = 74;
            scale[4] = 81;
            scale[5] = 82;
            scale[6] = 89;
            break;
        case 5:
            scale[0] = 63;
            scale[1] = 70;
            scale[2] = 74;
            scale[3] = 75;
            scale[4] = 79;
            scale[5] = 82;
            scale[6] = 84;
            break;
        default:
            break;
            
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
    moon_cluster_destroy(&mb->satellites);
    moon_cluster_destroy(&mb->ripples);
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
            if(g_data.satellites.nmoons > 0){
                g_data.satellites.moon[g_data.satellites.nmoons - 1].decay_mode = 1;
                g_data.pd.p[0] = 1;
                g_data.undo = 1;
            }
            break;
        case '1':
            set_scale(&g_data, 1);
            break;
        case '2':
            set_scale(&g_data, 2);
            break;
        case '3':
            set_scale(&g_data, 3);
            break;
        case '4':
            set_scale(&g_data, 4);
            break;
        case '5':
            set_scale(&g_data, 5);
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
    glEnable( GL_BLEND );
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
            moon_add(&g_data, &g_data.satellites, rad, theta);
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

