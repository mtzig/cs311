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

    if (key == GLFW_KEY_R) //red
        red = 1.0, green = 0.0, blue = 0.0;
    else if (key == GLFW_KEY_G) //green
        red = 0.0, green = 1.0, blue = 0.0;
    else if (key == GLFW_KEY_B) //blue
        red = 0.0, green = 0.0, blue = 1.0;
    else if (key == GLFW_KEY_C) //cyan
        red = 0.0, green = 1.0, blue = 1.0;
    else if (key == GLFW_KEY_M) //magenta
        red = 1.0, green = 0.0, blue = 1.0;
    else if (key == GLFW_KEY_Y) //yellow
        red = 1.0, green = 1.0, blue = 0.0;
    else if (key == GLFW_KEY_K) //black
        red = 0.0, green = 0.0, blue = 0.0;
    else if (key == GLFW_KEY_W) //white
        red = 1.0, green = 1.0, blue = 1.0;
    else if (key == GLFW_KEY_Q) //clear the screen
        pixClearRGB(0.0, 0.0, 0.0);


}


void handleMouseUp(double x, double y, int button, int shiftIsDown, 
        int controlIsDown, int altOptionIsDown, int superCommandIsDown) {
            
        mouseIsDown = 0;
}

void handleMouseDown(double x, double y, int button, int shiftIsDown, 
        int controlIsDown, int altOptionIsDown, int superCommandIsDown) {

        mouseIsDown = 1;
}

/* paints pixel if the mouse is held down */
void handleMouseMove(double x, double y) {

    if (mouseIsDown == 1)
        pixSetRGB(x,y, red, green, blue);
}





int main(void) {

    if (pixInitialize(512, 512, "Pixel Graphics") != 0)
        return 1;

    pixSetKeyDownHandler(handleKeyDown);
    pixSetMouseUpHandler(handleMouseUp);
    pixSetMouseDownHandler(handleMouseDown);
    pixSetMouseMoveHandler(handleMouseMove);

    /* Clear the window to black. */
    pixClearRGB(0.0, 0.0, 0.0);


    pixRun();
    pixFinalize();
    return 0;
}


