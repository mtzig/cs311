#include "040pixel.h"
#include <math.h>


/*
Creates point-slope equation on (i0, i1), (j0, j1)
Calculates value on x0
 */
double pointSlope(double x0, double i0, double i1, double j0, double j1){
    return i1 + (j1-i1) / (j0 - i0) * (x0 - i0);
}

void triRenderWithAleft(
        double a0, double a1, double b0, double b1, double c0, double c1, 
        double r, double g, double b) {

    if (a0 <=c0 && c0 <= b0){ // c is between a,b

        if (a0 != c0){ //render left half
            for (int x0 = (int) ceil(a0); x0 <= floor(c0); x0++){
                int bottom = (int) ceil(pointSlope(x0, a0, a1, b0, b1));
                int top = (int) floor(pointSlope(x0, a0, a1, c0, c1));

                for (int x1 = bottom; x1 <= top; x1++)
                    pixSetRGB(x0, x1, r, g, b);
                
            }
        }

        if (b0 != c0){ //render right half
            for (int x0 = (int) floor(c0) + 1; x0 <= floor(b0); x0++){
                int bottom = (int) ceil(pointSlope(x0, a0, a1, b0, b1));
                int top = (int) floor(pointSlope(x0, c0, c1, b0, b1));

                for (int x1 = bottom; x1 <= top; x1++)
                    pixSetRGB(x0, x1, r, g, b);
            }
        }


    }
    else // b is between a,c

        if (a0 != b0){ //render left half
            for (int x0 = (int) ceil(a0); x0 <= floor(b0); x0++){
                int bottom = (int) ceil(pointSlope(x0, a0, a1, b0, b1));
                int top = (int) floor(pointSlope(x0, a0, a1, c0, c1));

                for (int x1 = bottom; x1 <= top; x1++)
                    pixSetRGB(x0, x1, r, g, b);
                
            }
        }

        if (b0 != c0){ //render right half
            for (int x0 = (int) floor(b0) + 1; x0 <= floor(c0); x0++){
                int bottom = (int) ceil(pointSlope(x0, c0, c1, b0, b1));
                int top = (int) floor(pointSlope(x0, a0, a1, c0, c1));

                for (int x1 = bottom; x1 <= top; x1++)
                    pixSetRGB(x0, x1, r, g, b);
            }
        }
}

void triRender(
        double a0, double a1, double b0, double b1, double c0, double c1, 
        double r, double g, double b) {

    /* Make sure permutation of vertices is with a as leftmost */
    if (a0 <= b0 && a0 <= c0)
        triRenderWithAleft(a0, a1, b0, b1, c0, c1, r, g, b);
    else if (b0 <= a0 && b0 <= c0)
        triRenderWithAleft(b0, b1, c0, c1, a0, a1, r, g, b);
    else
        triRenderWithAleft(c0, c1, a0, a1, b0, b1, r, g, b);
	
}

