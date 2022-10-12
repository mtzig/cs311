/* 
Thomas Zeng
9/16/22

Draws a few triangles using 060triangle.c

Compile with:
    clang 080mainRasterizing.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit


 */


#include <stdio.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include "040pixel.h"
#include "080vector.c"
#include "080triangle.c"



int main(void) {
 
    if (pixInitialize(512, 512, "Rasterizer") != 0)
        return 1;

    /* Clear the window to black. */
    pixClearRGB(0.0, 0.0, 0.0);

    /* 
       C
    A --- B
    */
    double a[2] = {50, 50};
    double b[2] = {80, 60};
    double c[2] = {70, 80};
    double rgb[3] = {1, 0, 0};

    triRender(a, b, c, rgb);

   
    pixRun();

    pixFinalize();
    return 0;
}


