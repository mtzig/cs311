


/* 
Thomas Zeng
9/14/22

Compile with:
    clang 060mainRasterizing.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit

 */


#include <stdio.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include "040pixel.h"
#include "060triangle.c"



int main(void) {
 
    if (pixInitialize(512, 512, "Pixel Graphics") != 0)
        return 1;

    /* Clear the window to black. */
    pixClearRGB(0.0, 0.0, 0.0);

    triRender(50,50, 60, 30, 70, 40,1,0,0);

    pixRun();

    pixFinalize();
    return 0;
}


