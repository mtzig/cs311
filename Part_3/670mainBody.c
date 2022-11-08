/*
Thomas Zeng & Kyosuke
*/


/* On macOS, compile with...
    clang 670mainBody.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit

*/
#include <stdio.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include "040pixel.h"

#include "650vector.c"
#include "280matrix.c"
#include "150texture.c"
#include "300isometry.c"
#include "300camera.c"
#include "660ray.c"
#include "670body.c"
#include "670sphere.c"

#define SCREENWIDTH 512
#define SCREENHEIGHT 512





/*** ARTWORK ******************************************************************/

/* Based on the uniforms, textures, rayIntersection, and texture coordinates, 
outputs a material. */
void getMaterial(
        int unifDim, const double unif[], int texNum, const texTexture *tex[], 
        const rayIntersection *inter, const double texCoords[2], 
        rayMaterial *material){
    
    texSample(tex[0], texCoords[0], texCoords[1], material->cDiffuse);
    material->hasAmbient = 1;
    material->hasDiffuse = 0;
    material->hasMirror = 0;
    material->hasSpecular = 0;
    material->hasTransmission = 0;

}

camCamera camera;
double cameraTarget[3] = {0.0, 0.0, 0.0};
double cameraRho = 10.0, cameraPhi = M_PI / 3.0, cameraTheta = M_PI / 3.0;
texTexture texture;
texTexture *textures[1] = {&texture};
texTexture **tex = textures;

/* Four spheres. */
#define BODYNUM 4
bodyBody bodies[4];

double cAmbient[3] = {0.5, 0.5, 0.5};

int initializeArtwork(void) {
    camSetProjectionType(&camera, camPERSPECTIVE);
    camSetFrustum(
        &camera, M_PI / 6.0, cameraRho, 10.0, SCREENWIDTH, SCREENHEIGHT);
    camLookAt(&camera, cameraTarget, cameraRho, cameraPhi, cameraTheta);


    if (bodyInitialize(&bodies[0], 1, 0, 1, &sphGetIntersection, &sphGetTexCoordsAndNormal, &getMaterial)!= 0){

        return 5;
    }
    if (bodyInitialize(&bodies[1], 1, 0, 1, &sphGetIntersection, &sphGetTexCoordsAndNormal, &getMaterial)!= 0){
        bodyFinalize(&bodies[0]);

        return 4;

    }
    if (bodyInitialize(&bodies[2], 1, 0, 1, &sphGetIntersection, &sphGetTexCoordsAndNormal, &getMaterial)!= 0){
        bodyFinalize(&bodies[0]);
        bodyFinalize(&bodies[1]);

        return 3;
    }
    if (bodyInitialize(&bodies[3], 1, 0, 1, &sphGetIntersection, &sphGetTexCoordsAndNormal, &getMaterial)!= 0){
        bodyFinalize(&bodies[0]);
        bodyFinalize(&bodies[1]);
        bodyFinalize(&bodies[2]);

        return 2;
    }

    double transl[3] = {0.0, 0.0, 0.0};
    isoSetTranslation(&(bodies[0].isometry), transl);
    vec3Set(1.0, 0.0, 0.0, transl);
    isoSetTranslation(&(bodies[1].isometry), transl);
    vec3Set(0.0, 1.0, 0.0, transl);
    isoSetTranslation(&(bodies[2].isometry), transl);
    vec3Set(0.0, 0.0, 1.0, transl);
    isoSetTranslation(&(bodies[3].isometry), transl);

    double radii[BODYNUM] = {1.0, 0.5, 0.5, 0.5};
    for(int i=0; i<BODYNUM; i++){
        bodies[i].textures = tex;
        bodySetGeometryUniforms(&bodies[i], 0, radii + i, 1);
    }

    if (texInitializeFile(&texture, "kanagawa.jpeg") != 0) {
        bodyFinalize(&bodies[0]);
        bodyFinalize(&bodies[1]);
        bodyFinalize(&bodies[2]);
        bodyFinalize(&bodies[3]);

		return 1;
	}


    return 0;
}

void finalizeArtwork(void) {

    bodyFinalize(&bodies[0]);
    bodyFinalize(&bodies[1]);
    bodyFinalize(&bodies[2]);
    bodyFinalize(&bodies[3]);
    texFinalize(&texture);
    return;
}



/*** RENDERING ****************************************************************/

/* Given a ray x(t) = p + t d. Finds the color where that ray hits the scene (or 
the background) and loads the color into the rgb parameter. */
void getSceneColor(
        int bodyNum, const bodyBody bodies[], const double p[3], 
        const double d[3], double rgb[3]) {

    // loops through every body
    double bound = rayINFINITY;
    rayIntersection ray, minRay;
    int minIdx = -1;

    for(int i=0; i<BODYNUM; i++){
        bodyGetIntersection(&bodies[i], p, d, bound, &ray);

        // update bounds (if ray hit body)
        if(ray.t != rayNONE){
            minRay = ray; // store ray that is closest
            bound = ray.t;
            minIdx = i;
        }
    }

    if(minIdx == -1){
        double background[3] = {0.0, 0.0, 0.0};
        vecCopy(3, background, rgb);
    }
    else{

        ray.t = bound; //need to set t to correct value

        double texCoords[2], normal[3], sample[3];


        bodyGetTexCoordsAndNormal(&bodies[minIdx], p, d, &minRay, texCoords, normal); 

        rayMaterial material;
        bodyGetMaterial(&bodies[minIdx], &minRay, texCoords, &material);


        if(material.hasAmbient)
            vecModulate(3, material.cDiffuse, cAmbient, rgb);
    }

    

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
            getSceneColor(BODYNUM, bodies, p, d, rgb);
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
        isoSetRotation(&(bodies[k].isometry), rotMatrix);
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


