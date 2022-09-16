#include "040pixel.h"
#include <math.h>


/*
Creates point-slope equation with points (i[0], i[1]), (j[0], j[1])
returns value on input x[0]
 */
double pointSlope(double x0, const double i[2], const double j[2]){
    return i[1] + (j[1]-i[1]) / (j[0] - i[0]) * (x0 - i[0]);
}

void triRenderWithAleft(
        const double a[2], const double b[2], const double c[2], 
        const double rgb[3]){

    if (a[0] <=c[0] && c[0] <= b[0]){ // c is between a,b

        if (a[0] != c[0]){ //render left half
            for (int x0 = (int) ceil(a[0]); x0 <= floor(c[0]); x0++){
                int bottom = (int) ceil(pointSlope(x0, a, b));
                int top = (int) floor(pointSlope(x0, a, c));

                for (int x1 = bottom; x1 <= top; x1++)
                    pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
                
            }
        }

        if (b[0] != c[0]){ //render right half
            for (int x0 = (int) floor(c[0]) + 1; x0 <= floor(b[0]); x0++){
                int bottom = (int) ceil(pointSlope(x0, a, b));
                int top = (int) floor(pointSlope(x0, c, b));

                for (int x1 = bottom; x1 <= top; x1++)
                    pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
            }
        }


    }
    else // b is between a,c

        if (a[0] != b[0]){ //render left half
            for (int x0 = (int) ceil(a[0]); x0 <= floor(b[0]); x0++){
                int bottom = (int) ceil(pointSlope(x0, a, b));
                int top = (int) floor(pointSlope(x0, a, c));

                for (int x1 = bottom; x1 <= top; x1++)
                    pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
                
            }
        }

        if (b[0] != c[0]){ //render right half
            for (int x0 = (int) floor(b[0]) + 1; x0 <= floor(c[0]); x0++){
                int bottom = (int) ceil(pointSlope(x0, c, b));
                int top = (int) floor(pointSlope(x0, a, c));

                for (int x1 = bottom; x1 <= top; x1++)
                    pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
            }
        }
}

void triRender(
        const double a[2], const double b[2], const double c[2], 
        const double rgb[3]){

    /* Make sure permutation of vertices is with a as leftmost */
    if (a[0] <= b[0] && a[0] <= c[0])
        triRenderWithAleft(a, b, c, rgb);
    else if (b[0] <= a[0] && b[0] <= c[0])
        triRenderWithAleft(b, c, a, rgb);
    else
        triRenderWithAleft(c, a, b, rgb);
	
}

