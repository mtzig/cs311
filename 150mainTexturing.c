/* 
Thomas Zeng & Shaun Baron-Furuyama

On macOS, compile with...
    clang 150mainTexturing.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit

*/

#include <stdio.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include "040pixel.h"

#include "080vector.c"
#include "100matrix.c"
#include "150texture.c"
#include "140triangle.c"



double a[2] = {288.0, -312.0};
double b[2] = {288.0, 288.0};
double c[2] = {-452.0, -452.0};
double rgb[3] = {1.0, 1.0, 1.0};
double angle = 0.0;

double alpha[2] = {0.0, 0.0};
double beta[2] = {1.0, 0.0}; 
double gamma[2] = {0.0, 1.0};

int interpolateMode = 0;
texTexture *tex;


void handleTimeStep(double oldTime, double newTime) {
    double transl[2] = {512.0, 512.0};
    double aa[2], bb[2], cc[2], rrggbb[3], rot[2][2];
    angle += (newTime - oldTime) / 10.0;
    mat22Rotation(angle, rot);
    mat221Multiply(rot, a, aa);
    mat221Multiply(rot, b, bb);
    mat221Multiply(rot, c, cc);
    vecAdd(2, transl, aa, aa);
    vecAdd(2, transl, bb, bb);
    vecAdd(2, transl, cc, cc);
    pixClearRGB(0.0, 0.0, 0.0);
    triRender(aa, bb, cc, rgb, tex, alpha, beta, gamma);
}


void handleKeyDown(int key, int shiftIsDown, int controlIsDown,
        int altOptionIsDown, int superCommandIsDown) {

    if (key == GLFW_KEY_ENTER){
        interpolateMode = 1 - interpolateMode;
        texSetFiltering(tex, interpolateMode);
    }

        
}

int main(void) {

    if (pixInitialize(1024, 1024, "Interpolator") != 0)
        return 1;

    tex = (texTexture *)malloc(sizeof(texTexture));
    texInitializeFile(tex, "kanagawa.jpeg");
    texSetFiltering(tex, 0);
    texSetTopBottom(tex, 0);
    texSetLeftRight(tex, 0);

    pixSetTimeStepHandler(handleTimeStep);
    pixSetKeyDownHandler(handleKeyDown);


    pixRun();
    texFinalize(tex);
    free(tex);
    pixFinalize();
    return 0;
}


