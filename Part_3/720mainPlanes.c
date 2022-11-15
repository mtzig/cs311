/*
Thomas Zeng & Kyosuke
*/


/* On macOS, compile with...
    clang 720mainPlanes.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit

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
#include "680light.c"
#include "720plane.c"



#define SCREENWIDTH 512
#define SCREENHEIGHT 512

#define UNIFSPEC 0
#define UNIFSHININESS 3

#define MATERUNIFDIM 4
#define GEOMUNIFDIM 1
#define TEXNUM 1

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define RECDEPTH 3


/*** ARTWORK ******************************************************************/

/* Based on the uniforms, textures, rayIntersection, and texture coordinates, 
outputs a material. */
void getPhongMaterial(
        int unifDim, const double unif[], int texNum, const texTexture *tex[], 
        const rayIntersection *inter, const double texCoords[2], 
        rayMaterial *material){
    
    texSample(tex[0], texCoords[0], texCoords[1], material->cDiffuse);
    material->hasAmbient = 1;
    material->hasDiffuse = 1;
    material->hasMirror = 0;
    material->hasSpecular = 1;
    material->hasTransmission = 0;

    vecCopy(3, &unif[UNIFSPEC], material->cSpecular);

    material->shininess = unif[UNIFSHININESS];

}

void getMirrorMaterial(
        int unifDim, const double unif[], int texNum, const texTexture *tex[], 
        const rayIntersection *inter, const double texCoords[2], 
        rayMaterial *material){
    
    material->hasAmbient = 0;
    material->hasDiffuse = 0;
    material->hasMirror = 0;
    material->hasSpecular = 0;
    material->hasTransmission = 0;

    material->hasMirror = 1;

    double cMirror[3] = {1.0, 1.0, 1.0};
    vecCopy(3, cMirror, material->cMirror);

}

camCamera camera;
double cameraTarget[3] = {0.0, 0.0, 0.0};
double cameraRho = 10.0, cameraPhi = M_PI / 3.0, cameraTheta = M_PI / 3.0;
texTexture texture;
texTexture *textures[1] = {&texture};
texTexture **tex = textures;

/* Four spheres and One plane */
#define BODYNUM 5
bodyBody bodies[BODYNUM];

double cAmbient[3] = {0.3, 0.3, 0.3};


#define LIGHTNUM 2
#define LIGHTDIR 0
#define LIGHTPOS 1

lightLight lights[LIGHTNUM];

void getDirectionalLighting(int unifDim, const double unif[], const isoIsometry *isometry, 
        const double x[3], lightLighting *lighting){
    
    // set distance
    lighting->distance = rayINFINITY;

    // set cLight
    vecCopy(3, unif, lighting->cLight);

    // set uLight
    double unitD[3] = {0.0, 0.0, 1};
    isoRotateDirection(isometry, unitD, lighting->uLight);

}

void getPositionalLighting(int unifDim, const double unif[], const isoIsometry *isometry, 
        const double x[3], lightLighting *lighting){
    
    double dLight[3];
    vecSubtract(3, isometry->translation, x, dLight);

    // set distance
    lighting->distance = vecLength(3, dLight);

    // set cLight
    vecCopy(3, unif, lighting->cLight);

    // set uLight
    vecUnit(3, dLight, lighting->uLight);

}



int initializeArtwork(void) {
    camSetProjectionType(&camera, camPERSPECTIVE);
    camSetFrustum(
        &camera, M_PI / 6.0, cameraRho, 10.0, SCREENWIDTH, SCREENHEIGHT);
    camLookAt(&camera, cameraTarget, cameraRho, cameraPhi, cameraTheta);

    if (lightInitialize(&lights[LIGHTPOS], 3, &getPositionalLighting) != 0){

        return 8;
    }
    if (lightInitialize(&lights[LIGHTDIR], 3, &getDirectionalLighting) != 0){
        lightFinalize(&lights[LIGHTPOS]);
        return 7;
    }
    if (bodyInitialize(&bodies[0], GEOMUNIFDIM, MATERUNIFDIM, TEXNUM, 
                       &sphGetIntersection, &sphGetTexCoordsAndNormal,
                       &getMirrorMaterial)!= 0){
        
        lightFinalize(&lights[LIGHTPOS]);
        lightFinalize(&lights[LIGHTDIR]);              
        return 6;
    }
    if (bodyInitialize(&bodies[1], GEOMUNIFDIM, MATERUNIFDIM, TEXNUM, 
                       &sphGetIntersection, &sphGetTexCoordsAndNormal, 
                       &getPhongMaterial)!= 0){

        lightFinalize(&lights[LIGHTPOS]);
        lightFinalize(&lights[LIGHTDIR]);
        bodyFinalize(&bodies[0]);

        return 5;

    }
    if (bodyInitialize(&bodies[2], GEOMUNIFDIM, MATERUNIFDIM, TEXNUM, 
                       &sphGetIntersection, &sphGetTexCoordsAndNormal, 
                       &getPhongMaterial)!= 0){
        
        lightFinalize(&lights[LIGHTPOS]);
        lightFinalize(&lights[LIGHTDIR]);
        bodyFinalize(&bodies[0]);
        bodyFinalize(&bodies[1]);

        return 4;
    }
    if (bodyInitialize(&bodies[3], GEOMUNIFDIM, MATERUNIFDIM, TEXNUM, 
                       &sphGetIntersection, &sphGetTexCoordsAndNormal, 
                       &getPhongMaterial)!= 0){
        
        lightFinalize(&lights[LIGHTPOS]);
        lightFinalize(&lights[LIGHTDIR]);
        bodyFinalize(&bodies[0]);
        bodyFinalize(&bodies[1]);
        bodyFinalize(&bodies[2]);

        return 3;
    }
    if (bodyInitialize(&bodies[4], GEOMUNIFDIM, MATERUNIFDIM, TEXNUM, 
                       &plaGetIntersection, &plaGetTexCoordsAndNormal, 
                       &getPhongMaterial)!= 0){
        
        lightFinalize(&lights[LIGHTPOS]);
        lightFinalize(&lights[LIGHTDIR]);
        bodyFinalize(&bodies[0]);
        bodyFinalize(&bodies[1]);
        bodyFinalize(&bodies[2]);
        bodyFinalize(&bodies[3]);

        return 2;
    }

    // set uniforms for directional light
    double dLightUnif[3] = {0.4, 0.4, 0.4};
    lightSetUniforms(&lights[LIGHTDIR], 0, dLightUnif, 3);

    // set uniforms for positional light
    double pLightUnif[3] = {0.3, 0.0, 0.0};
    lightSetUniforms(&lights[LIGHTPOS], 0, pLightUnif, 3);

    double pLightTrans[3] = {10.0, 10.0, 20.0};
    isoSetTranslation(&(lights[LIGHTPOS].isometry), pLightTrans);



    double transl[3] = {0.0, 0.0, 0.0};
    isoSetTranslation(&(bodies[0].isometry), transl);
    vec3Set(1.4, 0.0, 0.0, transl);
    isoSetTranslation(&(bodies[1].isometry), transl);
    vec3Set(0.0, 1.4, 0.0, transl);
    isoSetTranslation(&(bodies[2].isometry), transl);
    vec3Set(0.0, 0.0, 1.8, transl);
    isoSetTranslation(&(bodies[3].isometry), transl);

    vec3Set(0.0, 0.0, -1.6, transl);
    isoSetTranslation(&(bodies[4].isometry), transl);

    // plane does not use radii, we set it for ease of initialization
    double radii[BODYNUM] = {1.0, 0.5, 0.5, 0.5, -1.0};

    double materUnifs[MATERUNIFDIM] = {1.0, 1.0, 1.0, 64.0};

    for(int i=0; i<BODYNUM; i++){

        bodies[i].textures = tex;
        bodySetGeometryUniforms(&bodies[i], 0, radii + i, 1);
        bodySetMaterialUniforms(&bodies[i], 0, materUnifs, 4);
        printf("%d",i);
    }

    if (texInitializeFile(&texture, "kanagawa.jpeg") != 0) {

        lightFinalize(&lights[LIGHTPOS]);
        lightFinalize(&lights[LIGHTDIR]);
        bodyFinalize(&bodies[0]);
        bodyFinalize(&bodies[1]);
        bodyFinalize(&bodies[2]);
        bodyFinalize(&bodies[3]);
        bodyFinalize(&bodies[4]);

		return 1;
	}


    return 0;
}

void finalizeArtwork(void) {

    lightFinalize(&lights[LIGHTPOS]);
    lightFinalize(&lights[LIGHTDIR]);
    bodyFinalize(&bodies[0]);
    bodyFinalize(&bodies[1]);
    bodyFinalize(&bodies[2]);
    bodyFinalize(&bodies[3]);
    bodyFinalize(&bodies[4]);

    texFinalize(&texture);
    return;
}



/*** RENDERING ****************************************************************/

/* Casts the ray x(t) = p + t d into the scene. Returns 0 if it hits no body or 
1 if it hits any body. Used to determine whether a fragment is in shadow. */
int getSceneShadow(
        int bodyNum, const bodyBody bodies[], const double p[3], 
        const double d[3]){

    // used to get t
    rayIntersection ray;

    for(int i=0; i<BODYNUM; i++){
        bodyGetIntersection(&bodies[i], p, d, rayINFINITY, &ray);

        // if ray hits body, we are done with loop
        if(ray.t != rayNONE)
            return 1;
    }

    // did not hit body
    return 0;
}

/* Given a ray x(t) = p + t d. Finds the color where that ray hits the scene (or 
the background) and loads the color into the rgb parameter. */
void getSceneColor(int recDepth,
        int bodyNum, const bodyBody bodies[], const double cAmbient[3], 
        int lightNum, const lightLight lights[], const double p[3], 
        const double d[3], double rgb[3]){

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

    // color starts off black
    double black[3] = {0.0, 0.0, 0.0};
    vecCopy(3, black, rgb);

    // set color if ray hit a body
    if(minIdx != -1){

        ray.t = bound; //need to set t to correct value

        double texCoords[2], normal[3], sample[3];


        bodyGetTexCoordsAndNormal(&bodies[minIdx], p, d, &minRay, texCoords, normal); 

        rayMaterial material;
        bodyGetMaterial(&bodies[minIdx], &minRay, texCoords, &material);
        
        if(material.hasAmbient){
                double ambColor[3];
                vecModulate(3, material.cDiffuse, cAmbient, ambColor);
                vecAdd(3, ambColor, rgb, rgb);
        }

        lightLighting lighting;

        // computes world position of intersection
        double x[3];
        vecScale(3, ray.t, d, x);
        vecAdd(3, p, x, x);

        if(material.hasMirror && recDepth != 0){

            double dSomething[3], negD[3], dNew[3], cFromMirror[3], mirrColor[3];


            // Calculate dNew
            vecScale(3, -1, d, negD);
            double dScale = 2 * vecDot(3, negD, normal);

            // did not know what to call, but bascially first term of d_new formula
            vecScale(3, dScale, normal, dSomething);
            vecSubtract(3, dSomething, negD, dNew);

            getSceneColor(recDepth-1, bodyNum, bodies, cAmbient, lightNum, lights, x, dNew, cFromMirror);

            vecModulate(3, material.cMirror, cFromMirror, mirrColor);
            vecAdd(3, mirrColor, rgb, rgb);
        }

        for(int i=0; i<lightNum; i++){

            lightGetLighting(&lights[i], x, &lighting);

            int hasShadow = getSceneShadow(bodyNum, bodies, x, lighting.uLight);

            double iDiff; // iDiff is needed for specular and diffuse
            if((material.hasDiffuse || material.hasSpecular) && hasShadow != 1)
                iDiff = MAX(0, vecDot(3, normal, lighting.uLight));

            if(material.hasDiffuse && hasShadow != 1){
                    double diffColor[3];
                    vecModulate(3, lighting.cLight, material.cDiffuse, diffColor);
                    vecScale(3, iDiff, diffColor, diffColor);
                    vecAdd(3, diffColor, rgb, rgb);
            }

            if(material.hasSpecular && hasShadow != 1){
                double iSpec, uRefl[3], uCam[3], specColor[3];

                // compute uRefl
                vecScale(3, 2 * vecDot(3, normal, lighting.uLight), normal, uRefl);
                vecSubtract(3, uRefl, lighting.uLight, uRefl);

                // compute uCam
                vecScale(3, -1, d, uCam);
                vecUnit(3, uCam, uCam);
                
                // compute iSpec
                iSpec = MAX(0, vecDot(3, uRefl, uCam)) * (double) (iDiff > 0);

                // compute specColor
                vecModulate(3, lighting.cLight, material.cSpecular, specColor);
                vecScale(3, pow(iSpec, material.shininess), specColor, specColor);

                vecAdd(3, specColor, rgb, rgb);
            }
        }
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
            getSceneColor(RECDEPTH, BODYNUM, bodies, cAmbient, LIGHTNUM, lights, p, d, rgb);
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

    // last body i.e. plane is stationary
    for (int k = 0; k < BODYNUM-1; k += 1)
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


