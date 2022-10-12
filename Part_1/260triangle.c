/*
Thomas Zeng & Shaun Baron-Furuyama
*/

#include <math.h>

#define VARYX 0
#define VARYY 1

/*
Creates point-slope equation with points (i[0], i[1]), (j[0], j[1])
returns value on input x[0]
 */
double pointSlope(double x0, const double i[], const double j[])
{
    return i[VARYY] + (j[VARYY] - i[VARYY]) / (j[VARYX] - i[VARYX]) * (x0 - i[VARYX]);
}

/*
interpolates varyings, saves in vary
*/
void interpolate(const int varyDim, const double a[], const double A_I[2][2], const double bMa[], const double cMa[], const int x0, const int x1, double vary[])
{

    // calculate p and q
    double xMa[2] = {x0 - a[VARYX], x1 - a[VARYY]};
    double pq[2]; // vector to hold value of p and q
    mat221Multiply(A_I, xMa, pq);

    // interpolation
    double p_ba[varyDim], q_ga[varyDim];
    vecScale(varyDim, pq[0], bMa, p_ba);
    vecScale(varyDim, pq[1], cMa, q_ga);

    vecAdd(varyDim, p_ba, q_ga, vary);
    vecAdd(varyDim, vary, a, vary);
}

void triRenderWithAleft(
    const shaShading *sha, depthBuffer *buf, const double unif[],
    const texTexture *tex[], const double a[], const double b[],
    const double c[])
{

    int varyDim = sha->varyDim;

    double bMa[varyDim], cMa[varyDim], det;
    double A[2][2], A_I[2][2];

    vecSubtract(varyDim, b, a, bMa);
    vecSubtract(varyDim, c, a, cMa);

    mat22Columns(bMa, cMa, A);

    det = mat22Invert(A, A_I);

    // det has to be positive for traingle to be facing out
    if (det <= 0)
        return;

    double vary[varyDim]; // will hold varying vector
    double rgbd[4];       // will hold final rgb color of pixel

    if (a[VARYX] <= c[VARYX] && c[VARYX] <= b[VARYX])
    { // c is between a,b

        if (a[VARYX] != c[VARYX])
        { // render left half
            for (int x0 = (int)ceil(a[VARYX]); x0 <= floor(c[VARYX]); x0++)
            {
                int bottom = (int)ceil(pointSlope(x0, a, b));
                int top = (int)floor(pointSlope(x0, a, c));

                for (int x1 = bottom; x1 <= top; x1++)
                {
                    interpolate(varyDim, a, A_I, bMa, cMa, x0, x1, vary);
                    sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, varyDim, vary, rgbd);
                    if (rgbd[3] < depthGetDepth(buf, x0, x1)) //only render if depth is smaller
                    {
                        depthSetDepth(buf, x0, x1, rgbd[3]); //must reset depth
                        pixSetRGB(x0, x1, rgbd[0], rgbd[1], rgbd[2]);
                    }
                }
            }
        }

        if (b[VARYX] != c[VARYX])
        { // render right half
            for (int x0 = (int)floor(c[VARYX]) + 1; x0 <= floor(b[VARYX]); x0++)
            {
                int bottom = (int)ceil(pointSlope(x0, a, b));
                int top = (int)floor(pointSlope(x0, c, b));

                for (int x1 = bottom; x1 <= top; x1++)
                {
                    interpolate(varyDim, a, A_I, bMa, cMa, x0, x1, vary);
                    sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, varyDim, vary, rgbd);
                    if (rgbd[3] < depthGetDepth(buf, x0, x1))
                    {
                        depthSetDepth(buf, x0, x1, rgbd[3]);
                        pixSetRGB(x0, x1, rgbd[0], rgbd[1], rgbd[2]);
                    }
                }
            }
        }
    }
    else // b is between a,c

        if (a[VARYX] != b[VARYX])
        { // render left half
            for (int x0 = (int)ceil(a[VARYX]); x0 <= floor(b[VARYX]); x0++)
            {
                int bottom = (int)ceil(pointSlope(x0, a, b));
                int top = (int)floor(pointSlope(x0, a, c));

                for (int x1 = bottom; x1 <= top; x1++)
                {
                    interpolate(varyDim, a, A_I, bMa, cMa, x0, x1, vary);
                    sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, varyDim, vary, rgbd);
                    if (rgbd[3] < depthGetDepth(buf, x0, x1))
                    {
                        depthSetDepth(buf, x0, x1, rgbd[3]);
                        pixSetRGB(x0, x1, rgbd[0], rgbd[1], rgbd[2]);
                    }
                }
            }
        }

    if (b[VARYX] != c[VARYX])
    { // render right half
        for (int x0 = (int)floor(b[VARYX]) + 1; x0 <= floor(c[VARYX]); x0++)
        {
            int bottom = (int)ceil(pointSlope(x0, c, b));
            int top = (int)floor(pointSlope(x0, a, c));

            for (int x1 = bottom; x1 <= top; x1++)
            {
                interpolate(varyDim, a, A_I, bMa, cMa, x0, x1, vary);
                sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, varyDim, vary, rgbd);
                if (rgbd[3] < depthGetDepth(buf, x0, x1))
                {
                    depthSetDepth(buf, x0, x1, rgbd[3]);
                    pixSetRGB(x0, x1, rgbd[0], rgbd[1], rgbd[2]);
                }
            }
        }
    }
}

void triRender(
    const shaShading *sha, depthBuffer *buf, const double unif[],
    const texTexture *tex[], const double a[], const double b[],
    const double c[])
{

    /* Make sure permutation of vertices is with a as leftmost */
    if (a[VARYX] <= b[VARYX] && a[VARYX] <= c[VARYX])
        triRenderWithAleft(sha, buf, unif, tex, a, b, c);
    else if (b[VARYX] <= a[VARYX] && b[VARYX] <= c[VARYX])
        triRenderWithAleft(sha, buf, unif, tex, b, c, a);
    else
        triRenderWithAleft(sha, buf, unif, tex, c, a, b);
}
