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

static void hex2float(int r, int g, int b) 
{
    glColor3f(r / 255.0, g / 255.0, b / 255.0);
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
            hex2float(255, 255, 39);
            break;
        case 1:
            hex2float(255, 0, 77);
            break;
        case 2:
            hex2float(0, 231, 86);
            break;
        case 3:
            hex2float(41, 173, 255);
            break;
        case 4:
            hex2float(255, 163, 0);
            break;
        case 5:
            hex2float(255, 119, 168);
            break;
        case 6:
            hex2float(0, 135, 81);
            break;
        case 7:
            hex2float(131, 118, 156);
            break;
        default:
            hex2float(255, 255, 39);
            break;
    }

    glBegin(GL_LINE_STRIP);
        for(i = 0; i < npoints; i++) {
            glVertex3f(moon->radius * cos(i * incr), moon->radius * sin(i * incr), z);
        }
    glEnd();
    
    glBegin(GL_TRIANGLE_FAN);
        for(i = 0; i < npoints; i++) {
            glVertex3f(0.07 * cos(i * incr) + moon->radius * cos(theta), 
                    0.07 * sin(i * incr) + moon->radius * sin(theta), z);
        }
    glEnd();
    
    glBegin(GL_LINE_STRIP);
            glVertex3f(dash_X1, dash_Y1, z);
            glVertex3f(dash_X2, dash_Y2, z);
    glEnd();
}

int moon_draw(moon_base *mb)
{
    int i;
    /* clear the color and depth buffers */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
    for(i = 0; i < mb->nmoons; i++) { 
        orbit_draw(&mb->moon[i]); 
    }

    glFlush( );
    glutSwapBuffers( );
    return 0;
}
