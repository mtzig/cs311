/* 
Thomas Zeng & Shaun Baron-Furuyama


On macOS, compile with...
    clang 140mainTexturing.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit

*/

#include <stdio.h>
#include <math.h>
#include "040pixel.h"

#include "080vector.c"
#include "100matrix.c"
#include "140texture.c"
#include "140triangle.c"


double a[2] = {144.0, -156.0};
double b[2] = {244.0, 244.0};
double c[2] = {-226.0, -226.0};
double rgb[3] = {1.0, 1.0, 1.0};
double angle = 0.0;

double alpha[2] = {0.0, 0.0};
double beta[2] = {1.0, 0.0}; 
double gamma[2] = {0.0, 1.0};

texTexture *tex;


void handleTimeStep(double oldTime, double newTime) {
    double transl[2] = {256.0, 256.0};
    double aa[2], bb[2], cc[2], rrggbb[3], rot[2][2];
    angle += (newTime - oldTime) / 10.0;
    mat22Rotation(angle, rot);
    mat221Multiply(rot, a, aa);
    mat221Multiply(rot, b, bb);
    mat221Multiply(rot, c, cc);
    vecAdd(2, transl, aa, aa);
    vecAdd(2, transl, bb, bb);
    vecAdd(2, transl, cc, cc);
    vecScale(3, (2.0 + sin(newTime)) / 3.0, rgb, rrggbb);
    pixClearRGB(0.0, 0.0, 0.0);
    triRender(aa, bb, cc, rrggbb, tex, alpha, beta, gamma);
}

int main(void) {

    if (pixInitialize(512, 512, "Interpolator") != 0)
        return 1;

    tex = (texTexture *)malloc(sizeof(texTexture));
    texInitializeFile(tex, "kanagawa.jpeg");
    texSetFiltering(tex, 0);
    texSetTopBottom(tex, 0);
    texSetLeftRight(tex, 0);

    pixSetTimeStepHandler(handleTimeStep);

    pixRun();
    texFinalize(tex);
    free(tex);
    pixFinalize();
    return 0;
}


