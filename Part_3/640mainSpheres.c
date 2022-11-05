


/* On macOS, compile with...
    clang 640mainSpheres.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit
On Ubuntu, compile with...
    cc 640mainSpheres.c 040pixel.o -lglfw -lGL -lm -ldl
*/
#include <stdio.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include "040pixel.h"

#include "250vector.c"
#include "280matrix.c"
#include "300isometry.c"
#include "300camera.c"
#include "640ray.c"

#define SCREENWIDTH 512
#define SCREENHEIGHT 512



/*** SPHERES ******************************************************************/

/* Given a sphere of radius r, centered at the origin in its local coordinates. 
Given a modeling isometry that places that sphere in the scene. Given a ray 
x(t) = p + t d in world coordinates. Outputs a rayIntersection, whose t member 
is the least t, in the interval [rayEPSILON, bound], when the ray intersects the 
sphere. If there is no such t, then the t member is instead rayNONE. */
void getIntersection(
        double r, const isoIsometry *isom, const double p[3], const double d[3], 
        double bound, rayIntersection* inter) {

    // code of Josh
    double c[3];
    vecCopy(3, isom->translation, c);
    double pMinusC[3], dPMinusC, dD, rSq, disc, t;
    vecSubtract(3, p, c, pMinusC);
    dPMinusC = vecDot(3, d, pMinusC);
    dD = vecDot(3, d, d);
    rSq = r * r;
    disc = dPMinusC * dPMinusC - dD * (vecDot(3, pMinusC, pMinusC) - rSq);
    if (disc <= 0) {
        inter->t = rayNONE;
        return;
    }
    disc = sqrt(disc);
    t = (-dPMinusC - disc) / dD;
    if (rayEPSILON <= t && t <= bound) {
        inter->t = t;
        return;
    }
    t = (-dPMinusC + disc) / dD;
    if (rayEPSILON <= t && t <= bound) {

        inter->t = t;
        return;
    }

    inter->t = rayNONE;
}



/*** ARTWORK ******************************************************************/

camCamera camera;
double cameraTarget[3] = {0.0, 0.0, 0.0};
double cameraRho = 10.0, cameraPhi = M_PI / 3.0, cameraTheta = M_PI / 3.0;

/* Four spheres. */
#define BODYNUM 4
isoIsometry isoms[BODYNUM];
double radii[BODYNUM] = {1.0, 0.5, 0.5, 0.5};
double colors[BODYNUM][3] = {
    {1.0, 1.0, 1.0}, 
    {1.0, 0.0, 0.0}, 
    {0.0, 1.0, 0.0}, 
    {0.0, 0.0, 1.0}};

int initializeArtwork(void) {
    camSetProjectionType(&camera, camPERSPECTIVE);
    camSetFrustum(
        &camera, M_PI / 6.0, cameraRho, 10.0, SCREENWIDTH, SCREENHEIGHT);
    camLookAt(&camera, cameraTarget, cameraRho, cameraPhi, cameraTheta);
    double rot[3][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
    for (int k = 0; k < BODYNUM; k += 1)
        isoSetRotation(&(isoms[k]), rot);
    double transl[3] = {0.0, 0.0, 0.0};
    isoSetTranslation(&(isoms[0]), transl);
    vec3Set(1.0, 0.0, 0.0, transl);
    isoSetTranslation(&(isoms[1]), transl);
    vec3Set(0.0, 1.0, 0.0, transl);
    isoSetTranslation(&(isoms[2]), transl);
    vec3Set(0.0, 0.0, 1.0, transl);
    isoSetTranslation(&(isoms[3]), transl);
    return 0;
}

void finalizeArtwork(void) {
    return;
}



/*** RENDERING ****************************************************************/

/* Given a ray x(t) = p + t d. Finds the color where that ray hits the scene (or 
the background) and loads the color into the rgb parameter. */
void getSceneColor(const double p[3], const double d[3], double rgb[3]) {
    /* YOUR CODE GOES HERE. (MINE IS 16 LINES.) */

    // loops through every body
    double bound = rayINFINITY;
    rayIntersection ray;

    int sphereIdx = -1;
    for(int i=0; i<BODYNUM; i++){
        getIntersection(radii[i], &isoms[i], p, d, bound, &ray);

        // update bounds (if ray hit body)
        if(ray.t != rayNONE){
            bound = ray.t;
            sphereIdx = i;
        }
    }

    if(sphereIdx == -1){
        double background[3] = {0.0, 0.0, 0.0};
        vecCopy(3, background, rgb);
    }
    else
        vecCopy(3, colors[sphereIdx], rgb);

    

}

void render(void) {
    /* Build a 4x4 matrix that (along with homogeneous division) takes screen 
    coordinates (x0, x1, 0, 1) to the corresponding world coordinates. */

    double inverseViewPort[4][4], inverseProj[4][4], camIso[4][4], inverseTransforms[4][4], screenToWorld[4][4];

    mat44InverseViewport(SCREENWIDTH, SCREENHEIGHT, inverseViewPort);

    if(camera.projectionType == camORTHOGRAPHIC)
        camGetInverseOrthographic(&camera, inverseProj);
    else
        camGetInversePerspective(&camera, inverseProj);

    isoGetHomogeneous(&camera.isometry, camIso);

    mat444Multiply(camIso, inverseProj, inverseTransforms);
    mat444Multiply(inverseTransforms, inverseViewPort, screenToWorld);

    /* Declare p and maybe compute d. */
    double p[4], d[3];

    if(camera.projectionType == camORTHOGRAPHIC){
        double unitRay[3] = {0.0, 0.0, -1.0};
        isoRotateDirection(&camera.isometry, unitRay, d);
    }

    /* Each screen point is chosen to be on the near plane. */
    double screen[4] = {0.0, 0.0, 0.0, 1.0};
    for (int i = 0; i < SCREENWIDTH; i += 1) {
        screen[0] = i;
        for (int j = 0; j < SCREENHEIGHT; j += 1) {
            screen[1] = j;
            /* Compute p and maybe also d. */

            mat441Multiply(screenToWorld, screen, p);
            vecScale(4, 1/p[3], p, p); // homog division
            // printf("c %f %f %f d", p[0], p[1], p[2]);

            if(camera.projectionType == camPERSPECTIVE)
                vecSubtract(3, p, camera.isometry.translation, d);

            /* Set the pixel to the color of that ray. */
            double rgb[3];
            getSceneColor(p, d, rgb);
            pixSetRGB(i, j, rgb[0], rgb[1], rgb[2]);
        }
    }
}



/*** USER INTERFACE ***********************************************************/

void handleKey(
        int key, int shiftIsDown, int controlIsDown, int altOptionIsDown, 
        int superCommandIsDown) {
    if (key == GLFW_KEY_I)
        cameraPhi -= 0.1;
    else if (key == GLFW_KEY_K)
        cameraPhi += 0.1;
    else if (key == GLFW_KEY_J)
        cameraTheta -= 0.1;
    else if (key == GLFW_KEY_L)
        cameraTheta += 0.1;
    else if (key == GLFW_KEY_U)
        cameraRho *= 1.1;
    else if (key == GLFW_KEY_O)
        cameraRho *= 0.9;
    else if (key == GLFW_KEY_P) {
        if (camera.projectionType == camORTHOGRAPHIC)
            camSetProjectionType(&camera, camPERSPECTIVE);
        else
            camSetProjectionType(&camera, camORTHOGRAPHIC);
    }
    camSetFrustum(
        &camera, M_PI / 6.0, cameraRho, 10.0, SCREENWIDTH, SCREENHEIGHT);
    camLookAt(&camera, cameraTarget, cameraRho, cameraPhi, cameraTheta);
}

void handleTimeStep(double oldTime, double newTime) {
    if (floor(newTime) - floor(oldTime) >= 1.0)
        printf(
            "info: handleTimeStep: %f frames/s\n", 1.0 / (newTime - oldTime));
    double rotAxis[3] = {1.0 / sqrt(3.0), 1.0 / sqrt(3.0), 1.0 / sqrt(3.0)};
    double rotMatrix[3][3];
    mat33AngleAxisRotation(newTime, rotAxis, rotMatrix);
    for (int k = 0; k < BODYNUM; k += 1)
        isoSetRotation(&(isoms[k]), rotMatrix);
    render();
}

int main(void) {
    if (pixInitialize(SCREENWIDTH, SCREENHEIGHT, "640mainSpheres") != 0)
        return 1;
    if (initializeArtwork() != 0) {
        pixFinalize();
        return 2;
    }
    pixSetKeyDownHandler(handleKey);
    pixSetKeyRepeatHandler(handleKey);
    pixSetTimeStepHandler(handleTimeStep);
    pixRun();
    finalizeArtwork();
    pixFinalize();
    return 0;
}


