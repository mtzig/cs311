


/* 
Thomas Zeng
9/12/22

Compile with:

    clang 050mainPainting.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit

*/

#include <stdio.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include "040pixel.h"

double red = 1.0, green = 1.0, blue = 1.0;
int mouseIsDown = 0;

void handleKeyDown(int key, int shiftIsDown, int controlIsDown,
        int altOptionIsDown, int superCommandIsDown) {
    printf("handleKeyUp: %d, shift %d, control %d, altOpt %d, supComm %d\n",
        key, shiftIsDown, controlIsDown, altOptionIsDown, superCommandIsDown);
    // R, G, B, C, M, Y, K, and W
    if (key == GLFW_KEY_R)
        red = 1.0, green = 0.0, blue = 0.0;
    else if (key == GLFW_KEY_G)
        red = 0.0, green = 1.0, blue = 0.0;
    else if (key == GLFW_KEY_B)
        red = 0.0, green = 0.0, blue = 1.0;
    else if (key == GLFW_KEY_C)
        red = 0.0, green = 1.0, blue = 1.0;
    else if (key == GLFW_KEY_M)
        red = 1.0, green = 0.0, blue = 1.0;
    else if (key == GLFW_KEY_Y)
        red = 1.0, green = 1.0, blue = 0.0;
    else if (key == GLFW_KEY_K)
        red = 0.0, green = 0.0, blue = 0.0;
    else if (key == GLFW_KEY_W)
        red = 1.0, green = 1.0, blue = 1.0;
    else if (key == GLFW_KEY_Q)
        pixClearRGB(0.0, 0.0, 0.0);


}

/* Similarly, the following callbacks handle some mouse interactions. */
void handleMouseUp(double x, double y, int button, int shiftIsDown, 
        int controlIsDown, int altOptionIsDown, int superCommandIsDown) {
        mouseIsDown = 0;
}

void handleMouseDown(double x, double y, int button, int shiftIsDown, 
        int controlIsDown, int altOptionIsDown, int superCommandIsDown) {
        mouseIsDown = 1;
}
void handleMouseMove(double x, double y) {
    if (mouseIsDown == 1)
        pixSetRGB(x,y, red, green, blue);
}




/* You can also set callbacks for key down, key repeat, and mouse down. See 
040pixel.h for details. */

int main(void) {
    /* Make a 512 x 512 window with the title 'Pixel Graphics'. This function 
    returns 0 if no error occurred. */
    if (pixInitialize(512, 512, "Pixel Graphics") != 0)
        return 1;
    /* Register the callbacks (defined above) with the user interface, so that 
    they are called as needed during pixRun (invoked below). */
    pixSetKeyDownHandler(handleKeyDown);
    pixSetMouseUpHandler(handleMouseUp);
    pixSetMouseDownHandler(handleMouseDown);
    pixSetMouseMoveHandler(handleMouseMove);

    /* Clear the window to black. */
    pixClearRGB(0.0, 0.0, 0.0);

    /* Run the event loop. The callbacks that were registered above are 
    invoked as needed. */
    pixRun();
    /* In this course, whenever you call an 'initialize' function to set up some 
    resources, you must later call a matching 'finalize' function to clean up 
    those resources. */
    pixFinalize();
    return 0;
}


