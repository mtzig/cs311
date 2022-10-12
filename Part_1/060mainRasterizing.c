/* 
Thomas Zeng
9/14/22

Draws a few triangles using 060triangle.c

Compile with:
    clang 060mainRasterizing.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit


 */


#include <stdio.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include "040pixel.h"
#include "060triangle.c"



int main(void) {
 
    if (pixInitialize(512, 512, "Rasterizer") != 0)
        return 1;

    /* Clear the window to black. */
    pixClearRGB(0.0, 0.0, 0.0);

    /* 
       C
    A --- B
    */
    triRender(50,50, 80, 60, 70, 80,1,0,0);

    /*
    C
    A --- B
    */
    triRender(100,100, 110, 120, 100, 130, 0, 1, 0);

    /*
          C
    A --- B
    */
    triRender(200,200, 210, 220, 210, 230, 0,0,1);

    /* 
    A --- C
       B
    */
    triRender(300,300, 320, 250, 350, 270,1,1,0);

    /*
    A --- C
    B
    */
    triRender(100,300, 100, 290, 150, 295, 0, 1, 1);

    /*
    A --- C
          B
    */
    triRender(100,400, 190, 350, 190, 400, 1,0,1);
    /*
    A --- C
            B
    */
    triRender(400,400,500,350, 450,400,1,1,1);

    /* triange with different permutation of vertices
        should not see any difference in output
    */
    triRender(500,350, 450,400, 400,400,1,1,1);
    triRender(450,400, 400,400, 500,350,1,1,1);

    /*
            C
    A --- B
    */
    triRender(200,100,250,100, 300,260,1,1,1);


    pixRun();

    pixFinalize();
    return 0;
}


