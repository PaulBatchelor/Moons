#include <math.h>
#include <stdlib.h>
#include <stdio.h>

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

#ifdef _cplusplus
extern "C" {
#endif

#include "soundpipe.h"
#include "sporth.h"

#ifdef _cplusplus
}
#endif

#include "draw.h"
#include "base.h"

/* width and height */
long g_width = 640;
long g_height = 480;

float g_theta = 0;

static void hex2float(int r, int g, int b) 
{
    glColor3f(r / 255.0, g / 255.0, b / 255.0);
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
    glClearColor( 13.0/255.0, 0, 20.0/255.0, 1 );
    /* enable color material */
    glEnable( GL_COLOR_MATERIAL );
    /* enable depth test */
    glEnable( GL_DEPTH_TEST );
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
    gluPerspective( 45.0, (GLfloat) w / (GLfloat) h, 1.0, 300.0 );
    /* set the matrix mode to modelview */
    glMatrixMode( GL_MODELVIEW );
    /* load the identity matrix */
    glLoadIdentity( );
    /* position the view point */
    gluLookAt( 0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );
}


void mouseFunc( int button, int state, int x, int y )
{
    if( button == GLUT_LEFT_BUTTON ) {
        /* when left mouse button is down */
        if( state == GLUT_DOWN ) {
            exit(1);
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

void idleFunc( )
{
    /* render the scene */
    glutPostRedisplay( );
}

static int orbit_draw(moon_circle *moon) 
{
    int32_t i, npoints = 256;
    float incr = 2 * M_PI / (npoints - 1);
    float theta = moon->theta;
    
    glLineWidth( 2.0 );
    
    switch(moon->note)  {
        case 0:
            hex2float(74, 221, 237);
            break;
        case 1:
            hex2float(242, 189, 205);
            break;
        case 2:
            hex2float(152, 255, 152);
            break;
        case 3:
            hex2float(255, 250, 152);
            break;
        default:
            hex2float(74, 221, 237);
            break;
    }

    glBegin(GL_LINE_STRIP);
        for(i = 0; i < npoints; i++) {
            glVertex2f(moon->radius * cos(i * incr), moon->radius * sin(i * incr));
        }
    glEnd();
    
    glBegin(GL_TRIANGLE_FAN);
        for(i = 0; i < npoints; i++) {
            glVertex2f(0.07 * cos(i * incr) + moon->radius * cos(theta), 
                    0.07 * sin(i * incr) + moon->radius * sin(theta));
        }
    glEnd();
}

int moon_draw(moon_base *mb)
{
    /* clear the color and depth buffers */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   
    orbit_draw(&mb->moon[0]); 
    orbit_draw(&mb->moon[1]); 
    orbit_draw(&mb->moon[2]); 
    orbit_draw(&mb->moon[3]); 

    glFlush( );
    glutSwapBuffers( );
    return 0;
}
