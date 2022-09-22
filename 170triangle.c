/*
Thomas Zeng & Shaun Baron-Furuyama
*/


#include <math.h>


/*
Creates point-slope equation with points (i[0], i[1]), (j[0], j[1])
returns value on input x[0]
 */
double pointSlope(double x0, const double i[], const double j[]){
    return i[ATTRY] + (j[ATTRY]-i[ATTRY]) / (j[ATTRX] - i[ATTRX]) * (x0 - i[ATTRX]);
}


/* 
interpolates attributes, saves in attr
*/
void interpolate(const int attrDim, const double a[], const double A_I[2][2], const double bMa[], const double cMa[], const int x0, const int x1, double attr[]){

  
            //calculate p and q
            double xMa[2] = {x0 - a[ATTRX], x1 - a[ATTRY]};
            double pq[2]; //vector to hold value of p and q
            mat221Multiply(A_I, xMa, pq);

            // interpolation
            double p_ba[attrDim], q_ga[attrDim];
            vecScale(attrDim, pq[0], bMa, p_ba);
            vecScale(attrDim, pq[1], cMa, q_ga);

            vecAdd(attrDim, p_ba, q_ga, attr);
            vecAdd(attrDim, attr, a, attr);


}

void triRenderWithAleft(
        const shaShading *sha, const double unif[], const texTexture *tex[], 
        const double a[], const double b[], const double c[]){
    
    int attrDim = sha->attrDim;

    double bMa[attrDim], cMa[attrDim];
    double A[2][2], A_I[2][2];


    vecSubtract(attrDim, b, a, bMa);
    vecSubtract(attrDim, c, a, cMa);

    mat22Columns(bMa, cMa, A);
    mat22Invert(A, A_I);


    double attr[attrDim]; //will hold attribute vector
    double rgb[3]; //will hold final rgb color of pixel

    if (a[ATTRX] <=c[ATTRX] && c[ATTRX] <= b[ATTRX]){ // c is between a,b

        if (a[ATTRX] != c[ATTRX]){ //render left half
            for (int x0 = (int) ceil(a[ATTRX]); x0 <= floor(c[ATTRX]); x0++){
                int bottom = (int) ceil(pointSlope(x0, a, b));
                int top = (int) floor(pointSlope(x0, a, c));

                for (int x1 = bottom; x1 <= top; x1++){
                    interpolate(attrDim, a, A_I, bMa, cMa, x0, x1, attr);
                    shadeFragment(sha->unifDim, unif, sha->texNum, tex, attrDim, attr, rgb);
                    pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
                }
                          
            }
        }

        if (b[ATTRX] != c[ATTRX]){ //render right half
            for (int x0 = (int) floor(c[ATTRX]) + 1; x0 <= floor(b[ATTRX]); x0++){
                int bottom = (int) ceil(pointSlope(x0, a, b));
                int top = (int) floor(pointSlope(x0, c, b));

                for (int x1 = bottom; x1 <= top; x1++){
                    interpolate(attrDim, a, A_I, bMa, cMa, x0, x1, attr);
                    shadeFragment(sha->unifDim, unif, sha->texNum, tex, attrDim, attr, rgb);
                    pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);    
                }     
            }
        }


    }
    else // b is between a,c

        if (a[ATTRX] != b[ATTRX]){ //render left half
            for (int x0 = (int) ceil(a[ATTRX]); x0 <= floor(b[ATTRX]); x0++){
                int bottom = (int) ceil(pointSlope(x0, a, b));
                int top = (int) floor(pointSlope(x0, a, c));

                for (int x1 = bottom; x1 <= top; x1++){
                    interpolate(attrDim, a, A_I, bMa, cMa, x0, x1, attr);
                    shadeFragment(sha->unifDim, unif, sha->texNum, tex, attrDim, attr, rgb);
                    pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]); 
                }                 
            }
        }

        if (b[ATTRX] != c[ATTRX]){ //render right half
            for (int x0 = (int) floor(b[ATTRX]) + 1; x0 <= floor(c[ATTRX]); x0++){
                int bottom = (int) ceil(pointSlope(x0, c, b));
                int top = (int) floor(pointSlope(x0, a, c));

                for (int x1 = bottom; x1 <= top; x1++){
                    interpolate(attrDim, a, A_I, bMa, cMa, x0, x1, attr);
                    shadeFragment(sha->unifDim, unif, sha->texNum, tex, attrDim, attr, rgb);
                    pixSetRGB(x0, x1, rgb[0], rgb[1], rgb[2]);
                }
            }
        }
}

void triRender(
        const shaShading *sha, const double unif[], const texTexture *tex[], 
        const double a[], const double b[], const double c[]){

    /* Make sure permutation of vertices is with a as leftmost */
    if (a[ATTRX] <= b[ATTRX] && a[ATTRX] <= c[ATTRX])
        triRenderWithAleft(sha, unif, tex, a, b, c);
    else if (b[ATTRX] <= a[ATTRX] && b[ATTRX] <= c[ATTRX])
        triRenderWithAleft(sha, unif, tex, b, c, a);
    else
        triRenderWithAleft(sha, unif, tex, c, a, b);
	
}

