#include "040pixel.h"
#include <math.h>


/*
Creates point-slope equation with points (i[0], i[1]), (j[0], j[1])
returns value on input x[0]
 */
double pointSlope(double x0, const double i[2], const double j[2]){
    return i[1] + (j[1]-i[1]) / (j[0] - i[0]) * (x0 - i[0]);
}

void pixSetInterpolate(double x0, double x1, const double a[2], const double bMa[2], 
         const double cMa[2], const double A_I[2][2], const double alpha[3], const double betaMalpha[3], 
         const double gammaMalpha[3], const double rgb[3]){

            double xMa[2] = {x0 - a[0], x1 - a[1]};

            double pq[2]; //vector to hold value of p and q

            mat221Multiply(A_I, xMa, pq);

            double rgb_int[3]; //vector for interpolated and modulated rgb

            double Alpha[2][2]; //matrix to store color vectors
            mat22Columns(betaMalpha, gammaMalpha, Alpha);

            // interpolation
            mat221Multiply(Alpha, pq, rgb_int);
            vecAdd(3, rgb_int, alpha, rgb_int);

            //modulation
            vecModulate(3, rgb_int, rgb, rgb_int);
            
            pixSetRGB(x0, x1, rgb_int[0], rgb_int[1], rgb_int[2]);


        }

void triRenderWithAleft(
        const double a[2], const double b[2], const double c[2], 
        const double rgb[3], const double alpha[3], const double beta[3], 
        const double gamma[3]){
    
    double bMa[2], cMa[2];
    double A[2][2], A_I[2][2];
    double betaMalpha[3], gammaMalpha[3];

    vecSubtract(2, b, a, bMa);
    vecSubtract(2, c, a, cMa);

    mat22Columns(bMa, cMa, A);
    mat22Invert(A, A_I);

    vecSubtract(3, beta, alpha, betaMalpha);
    vecSubtract(3, gamma, alpha, gammaMalpha);
    

    if (a[0] <=c[0] && c[0] <= b[0]){ // c is between a,b

        if (a[0] != c[0]){ //render left half
            for (int x0 = (int) ceil(a[0]); x0 <= floor(c[0]); x0++){
                int bottom = (int) ceil(pointSlope(x0, a, b));
                int top = (int) floor(pointSlope(x0, a, c));

                for (int x1 = bottom; x1 <= top; x1++)
                    pixSetInterpolate(
                        x0, x1, a, bMa, cMa, A_I, alpha, 
                        betaMalpha, gammaMalpha, rgb);                     
            }
        }

        if (b[0] != c[0]){ //render right half
            for (int x0 = (int) floor(c[0]) + 1; x0 <= floor(b[0]); x0++){
                int bottom = (int) ceil(pointSlope(x0, a, b));
                int top = (int) floor(pointSlope(x0, c, b));

                for (int x1 = bottom; x1 <= top; x1++)
                    pixSetInterpolate(
                        x0, x1, a, bMa, cMa, A_I, alpha, 
                        betaMalpha, gammaMalpha, rgb);            
            }
        }


    }
    else // b is between a,c

        if (a[0] != b[0]){ //render left half
            for (int x0 = (int) ceil(a[0]); x0 <= floor(b[0]); x0++){
                int bottom = (int) ceil(pointSlope(x0, a, b));
                int top = (int) floor(pointSlope(x0, a, c));

                for (int x1 = bottom; x1 <= top; x1++)
                    pixSetInterpolate(
                        x0, x1, a, bMa, cMa, A_I, alpha, 
                        betaMalpha, gammaMalpha, rgb);                     
            }
        }

        if (b[0] != c[0]){ //render right half
            for (int x0 = (int) floor(b[0]) + 1; x0 <= floor(c[0]); x0++){
                int bottom = (int) ceil(pointSlope(x0, c, b));
                int top = (int) floor(pointSlope(x0, a, c));

                for (int x1 = bottom; x1 <= top; x1++)
                    pixSetInterpolate(
                        x0, x1, a, bMa, cMa, A_I, alpha, 
                        betaMalpha, gammaMalpha, rgb);     
            }
        }
}

void triRender(
        const double a[2], const double b[2], const double c[2], 
        const double rgb[3], const double alpha[3], const double beta[3], 
        const double gamma[3]){

    /* Make sure permutation of vertices is with a as leftmost */
    if (a[0] <= b[0] && a[0] <= c[0])
        triRenderWithAleft(a, b, c, rgb, alpha, beta, gamma);
    else if (b[0] <= a[0] && b[0] <= c[0])
        triRenderWithAleft(b, c, a, rgb, alpha, beta, gamma);
    else
        triRenderWithAleft(c, a, b, rgb, alpha, beta, gamma);
	
}

