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

float g_theta = 0;

static void hex2float(int r, int g, int b, float a) 
{
    glColor4f(r / 255.0, g / 255.0, b / 255.0, a);
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
    float z = 0.0;
    float dash_X1 = (moon->radius - 0.05) * cos(moon->itheta);
    float dash_X2 = (moon->radius + 0.05) * cos(moon->itheta);
    float dash_Y1 = (moon->radius - 0.05) * sin(moon->itheta);
    float dash_Y2 = (moon->radius + 0.05) * sin(moon->itheta);

    glLineWidth( 1.0 );
    
    switch(moon->note)  {
        case 0:
            hex2float(255, 255, 39, moon->alpha);
            break;
        case 1:
            hex2float(255, 0, 77, moon->alpha);
            break;
        case 2:
            hex2float(0, 231, 86, moon->alpha);
            break;
        case 3:
            hex2float(41, 173, 255, moon->alpha);
            break;
        case 4:
            hex2float(255, 163, 0, moon->alpha);
            break;
        case 5:
            hex2float(255, 119, 168, moon->alpha);
            break;
        case 6:
            hex2float(0, 135, 81, moon->alpha);
            break;
        case 7:
            hex2float(131, 118, 156, moon->alpha);
            break;
        default:
            hex2float(255, 255, 39, moon->alpha);
            break;
    }

    /* draw orbit path */
    glBegin(GL_LINE_STRIP);
        for(i = 0; i < npoints; i++) {
            glVertex3f(moon->radius * cos(i * incr), moon->radius * sin(i * incr), z);
        }
    glEnd();
   
    /* draw moon */
    glBegin(GL_TRIANGLE_FAN);
        for(i = 0; i < npoints; i++) {
            glVertex3f(moon->size * cos(i * incr) + moon->radius * cos(theta), 
                    moon->size * sin(i * incr) + moon->radius * sin(theta), z);
        }
    glEnd();
   
    /* draw starting point notch */ 
    glBegin(GL_LINE_STRIP);
            glVertex3f(dash_X1, dash_Y1, z);
            glVertex3f(dash_X2, dash_Y2, z);
    glEnd();

    if(moon->decay_mode == 0 && moon->alpha < 1) {
        moon->alpha += moon->decay; 
    }else if(moon->decay_mode == 1 && moon->alpha > 0) {
        moon->alpha -= moon->decay;
    }else if(moon->decay_mode == 1 && moon->alpha < 0) {
        *moon->nmoons = *moon->nmoons - 1;
	moon->decay_mode = 2;
	printf("there are now %d moons\n", *moon->nmoons);
    }
}

int moon_add(moon_base *mb, float radius, float theta)
{
    if(mb->satellites.nmoons >= mb->satellites.max_moons) {
        fprintf(stderr, "Warning: max number of moons created!");
        return 0;
    }

    int id;
    mb->satellites.nmoons++;

    id = mb->satellites.nmoons - 1; 

    while(theta >= 2 * M_PI) theta -= 2 * M_PI;
    while(theta < 0) theta += 2 * M_PI;
    
    mb->satellites.moon[id].theta = theta;
    mb->satellites.moon[id].itheta = theta;
    mb->satellites.moon[id].radius = radius;
    mb->satellites.moon[id].time = 0;
    mb->satellites.moon[id].alpha = 0;
    mb->satellites.moon[id].decay = 0.05;
    mb->satellites.moon[id].decay_mode = 0;
    mb->satellites.moon[id].nmoons = &mb->satellites.nmoons;
    mb->satellites.moon[id].size = 0.07;

    theta = fabs(theta) / (2.0 * M_PI);
    theta = floor(mb->scale->size * theta);
    mb->satellites.moon[id].note= (int)theta;
    fprintf(stderr, "the note is %d!, theta is %g\n", 
            mb->satellites.moon[id].note, theta);

    return 0;
}

int moon_draw(moon_base *mb)
{
    int i;
    /* clear the color and depth buffers */
    glBlendFunc(GL_ONE, GL_ZERO);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for(i = 0; i < mb->satellites.nmoons; i++) { 
        orbit_draw(&mb->satellites.moon[i]); 
    }

    glFlush( );
    glutSwapBuffers( );
    return 0;
}
