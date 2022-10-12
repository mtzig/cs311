/*
Thomas Zeng & Shaun Baron-Furuyama
*/


/* On macOS, compile with...
    clang 260mainDepth.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLFW/glfw3.h>

#include "040pixel.h"

#include "250vector.c"
#include "250matrix.c"
#include "260depth.c"
#include "150texture.c"
#include "260shading.c"
#include "260triangle.c"
#include "260mesh.c"
#include "190mesh2D.c"
#include "250mesh3D.c"

#define ATTRX 0
#define ATTRY 1
#define ATTRZ 2
#define ATTRS 3
#define ATTRT 4
#define ATTRN 5
#define ATTRO 6
#define ATTRP 7
#define VARYX 0
#define VARYY 1
#define VARYZ 2
#define VARYS 3
#define VARYT 4
#define UNIFR 0
#define UNIFG 1
#define UNIFB 2
#define UNIFMODELING 3
#define TEXR 0
#define TEXG 1
#define TEXB 2

void shadeVertex(
        int unifDim, const double unif[], int attrDim, const double attr[], 
        int varyDim, double vary[]) {
	double attrHomog[4] = {attr[0], attr[1], attr[2], 1.0};
	mat441Multiply((double(*)[4])(&unif[UNIFMODELING]), attrHomog, vary);
	//varyz should be automatic
	vary[VARYS] = attr[ATTRS];
	vary[VARYT] = attr[ATTRT];
}

void shadeFragment(
        int unifDim, const double unif[], int texNum, const texTexture *tex[], 
        int varyDim, const double vary[], double rgbd[4]) {
	double sample[tex[0]->texelDim];
	texSample(tex[0], vary[VARYS], vary[VARYT], sample);
	//vecModulate(3, sample, &unif[UNIFR], rgb);
	rgbd[0] = sample[TEXR] * unif[UNIFR];
	rgbd[1] = sample[TEXG] * unif[UNIFG];
	rgbd[2] = sample[TEXB] * unif[UNIFB];
	rgbd[3] = -vary[VARYZ];

}

shaShading sha;
texTexture texture;
const texTexture *textures[1] = {&texture};
const texTexture **tex = textures;
meshMesh mesh, mesh1;
depthBuffer buf;

double unif[3 + 16] = {
    1.0, 1.0, 1.0, 
	1.0, 0.0, 0.0, 0.0, 
	0.0, 1.0, 0.0, 0.0, 
	0.0, 0.0, 1.0, 0.0, 
	0.0, 0.0, 0.0, 1.0};
double rotationAngle = 0.0;
double translationVector[2] = {256.0, 256.0};

void render(void) {
	pixClearRGB(0.0, 0.0, 0.0);
	depthClearDepths(&buf, 1000.0);
	meshRender(&mesh, &buf, &sha, unif, tex);
	meshRender(&mesh1, &buf, &sha, unif, tex);

}

void handleKeyUp(
        int key, int shiftIsDown, int controlIsDown, int altOptionIsDown, 
        int superCommandIsDown) {
	if (key == GLFW_KEY_ENTER) {
		if (texture.filtering == texLINEAR)
			texSetFiltering(&texture, texNEAREST);
		else
			texSetFiltering(&texture, texLINEAR);
		render();
	}
}

void handleTimeStep(double oldTime, double newTime) {
	if (floor(newTime) - floor(oldTime) >= 1.0)
		printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
	unif[UNIFR] = sin(newTime);
	unif[UNIFG] = cos(oldTime);
	rotationAngle += (newTime - oldTime);
	double isom[4][4];
	double rotation[3][3];
	double axis[3] = {1.0 / sqrt(3.0), 1.0 / sqrt(3.0), 1.0 / sqrt(3.0)};
	mat33AngleAxisRotation(rotationAngle, axis, rotation);
	mat44Isometry(rotation, translationVector, isom);
	vecCopy(16, (double *)isom, &unif[UNIFMODELING]);
	render();
}

int main(void) {
	if (pixInitialize(512, 512, "Three Dimensions") != 0)
		return 1;
	if (texInitializeFile(&texture, "kanagawa.jpeg") != 0) {
	    pixFinalize();
		return 2;
	}
	if (mesh3DInitializeBox(&mesh, -128.0, 128.0, -64.0, 64.0, -32.0, 32.0) != 0) {
	    texFinalize(&texture);
	    pixFinalize();
		return 3;
	}
	if (depthInitialize(&buf, 512, 512) != 0){
		meshFinalize(&mesh);
		texFinalize(&texture);
	    pixFinalize();
		return 4;
	}
	// if (mesh3DInitializeCapsule(&mesh1, 100.0, 50.0, 50, 50) != 0){
	if (mesh3DInitializeSphere(&mesh1, 50.0, 50, 50) != 0) {
		depthFinalize(&buf);
		meshFinalize(&mesh);
		texFinalize(&texture);
	    pixFinalize();
		return 5;
	}
    texSetFiltering(&texture, texNEAREST);
    texSetLeftRight(&texture, texREPEAT);
    texSetTopBottom(&texture, texREPEAT);
    sha.unifDim = 3 + 16;
    sha.attrDim = 3 + 2 + 3;
    sha.varyDim = 3 + 2;
    sha.shadeVertex = shadeVertex;
    sha.shadeFragment = shadeFragment;
    sha.texNum = 1;
    render();
    pixSetKeyUpHandler(handleKeyUp);
    pixSetTimeStepHandler(handleTimeStep);
    pixRun();
	depthFinalize(&buf);
    meshFinalize(&mesh);
    meshFinalize(&mesh1);
    texFinalize(&texture);
    pixFinalize();
    return 0;
}


