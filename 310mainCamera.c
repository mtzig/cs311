/*
Thomas Zeng & Shaun Baron-Furuyama
*/

/* On macOS, compile with...
	clang 310mainCamera.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLFW/glfw3.h>

#include "040pixel.h"

#include "250vector.c"
#include "280matrix.c"
#include "260depth.c"
#include "150texture.c"
#include "260shading.c"
#include "270triangle.c"
#include "280mesh.c"
#include "190mesh2D.c"
#include "250mesh3D.c"
#include "300isometry.c"
#include "300camera.c"

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
#define VARYW 3
#define VARYS 4
#define VARYT 5
#define UNIFMODELING 0
#define UNIFMODELINGSPHERE 16
#define UNIFPROJ 32
#define TEXR 0
#define TEXG 1
#define TEXB 2

// returns X,Y,Z,W as first four of varying
void shadeVertex(
	int unifDim, const double unif[], int attrDim, const double attr[],
	int varyDim, double vary[])
{
	double attrHomog[4] = {attr[0], attr[1], attr[2], 1.0};

	double vary_world[4]; // will hold world coordinates of vary

	mat441Multiply((double(*)[4])(&unif[UNIFMODELING]), attrHomog, vary_world);

	// projects varying to ndc
	mat441Multiply((double(*)[4])(&unif[UNIFPROJ]), vary_world, vary);

	vary[VARYS] = attr[ATTRS];
	vary[VARYT] = attr[ATTRT];
}

void shadeVertex1(
	int unifDim, const double unif[], int attrDim, const double attr[],
	int varyDim, double vary[])
{
	double attrHomog[4] = {attr[0], attr[1], attr[2], 1.0};

	double vary_world[4]; // will hold world coordinates of vary

	mat441Multiply((double(*)[4])(&unif[UNIFMODELINGSPHERE]), attrHomog, vary_world);

	// projects varying to ndc
	mat441Multiply((double(*)[4])(&unif[UNIFPROJ]), vary_world, vary);

	vary[VARYS] = attr[ATTRS];
	vary[VARYT] = attr[ATTRT];
}

void shadeFragment(
	int unifDim, const double unif[], int texNum, const texTexture *tex[],
	int varyDim, const double vary[], double rgbd[4])
{
	double sample[tex[0]->texelDim];
	texSample(tex[0], vary[VARYS], vary[VARYT], sample);
	rgbd[0] = sample[TEXR];
	rgbd[1] = sample[TEXG];
	rgbd[2] = sample[TEXB];
	rgbd[3] = vary[VARYZ];
}

shaShading sha;

shaShading sha1;

texTexture texture;
const texTexture *textures[1] = {&texture};
const texTexture **tex = textures;
meshMesh mesh, mesh1;
depthBuffer buf;

camCamera cam;

double viewport[4][4];
double target[3] = {0.0, 0.0, -10.0}; // where objects are clustered
double rho = 10.0;
double phi = 0.0;
double theta = -M_PI / 2;

double unif[16 + 16 + 16] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0};

double rotationAngle = 0; // M_PI /2;
double translationVectorRectangle[3] = {0.0, 0.0, 0.0};
double translationVectorSphere[3] = {0.5, 0.5, -8};
// double projection[6] = {0.0, 5.0, 0.0, 5.0, -2.0, 0.0};

void render(void)
{
	pixClearRGB(0.0, 0.0, 0.0);
	depthClearDepths(&buf, 1000000.0);
	meshRender(&mesh, &buf, viewport, &sha, unif, tex);
	meshRender(&mesh1, &buf, viewport, &sha1, unif, tex);
}

void handleKeyUp(
	int key, int shiftIsDown, int controlIsDown, int altOptionIsDown,
	int superCommandIsDown)
{
	if (key == GLFW_KEY_ENTER)
	{
		if (texture.filtering == texLINEAR)
			texSetFiltering(&texture, texNEAREST);
		else
			texSetFiltering(&texture, texLINEAR);
	}
	else if (key == GLFW_KEY_P)
	{
		if (cam.projectionType == camORTHOGRAPHIC)
		{
			camSetProjectionType(&cam, camPERSPECTIVE);

			camSetFrustum(&cam, M_PI / 6.0, 10.0, 10.0, 512, 512);
			camLookAt(&cam, target, rho, phi, theta);

			double proj[4][4];
			camGetProjectionInverseIsometry(&cam, proj);
			vecCopy(16, (double *)proj, &unif[UNIFPROJ]);
		}
		else
		{
			camSetProjectionType(&cam, camORTHOGRAPHIC);

			camSetFrustum(&cam, M_PI / 6.0, 10.0, 10.0, 512, 512);
			camLookAt(&cam, target, rho, phi, theta);

			double proj[4][4];
			camGetProjectionInverseIsometry(&cam, proj);
			vecCopy(16, (double *)proj, &unif[UNIFPROJ]);
		}
	}
	else if (key == GLFW_KEY_UP)
	{
		phi += 0.1;
		camSetFrustum(&cam, M_PI / 6.0, 10.0, 10.0, 512, 512);
		camLookAt(&cam, target, rho, phi, theta);

		double proj[4][4];
		camGetProjectionInverseIsometry(&cam, proj);
		vecCopy(16, (double *)proj, &unif[UNIFPROJ]);
	}
	else if (key == GLFW_KEY_DOWN)
	{
		phi -= 0.1;
		camSetFrustum(&cam, M_PI / 6.0, 10.0, 10.0, 512, 512);
		camLookAt(&cam, target, rho, phi, theta);
		double proj[4][4];
		camGetProjectionInverseIsometry(&cam, proj);
		vecCopy(16, (double *)proj, &unif[UNIFPROJ]);
	}
	else if (key == GLFW_KEY_LEFT)
	{
		theta -= 0.1;
		camSetFrustum(&cam, M_PI / 6.0, 10.0, 10.0, 512, 512);
		camLookAt(&cam, target, rho, phi, theta);
		double proj[4][4];
		camGetProjectionInverseIsometry(&cam, proj);
		vecCopy(16, (double *)proj, &unif[UNIFPROJ]);
	}
	else if (key == GLFW_KEY_RIGHT)
	{
		theta += 0.1;
		camSetFrustum(&cam, M_PI / 6.0, 10.0, 10.0, 512, 512);
		camLookAt(&cam, target, rho, phi, theta);
		double proj[4][4];
		camGetProjectionInverseIsometry(&cam, proj);
		vecCopy(16, (double *)proj, &unif[UNIFPROJ]);
	}
	else if (key == GLFW_KEY_O)
	{
		rho += 0.1;
		camSetFrustum(&cam, M_PI / 6.0, 10.0, 10.0, 512, 512);
		camLookAt(&cam, target, rho, phi, theta);
		double proj[4][4];
		camGetProjectionInverseIsometry(&cam, proj);
		vecCopy(16, (double *)proj, &unif[UNIFPROJ]);
	}
	else if (key == GLFW_KEY_L)
	{
		rho -= 0.1;
		camSetFrustum(&cam, M_PI / 6.0, 10.0, 10.0, 512, 512);
		camLookAt(&cam, target, rho, phi, theta);
		double proj[4][4];
		camGetProjectionInverseIsometry(&cam, proj);
		vecCopy(16, (double *)proj, &unif[UNIFPROJ]);
	}

	render();
}

int main(void)
{
	if (pixInitialize(512, 512, "Three Dimensions") != 0)
		return 1;
	if (texInitializeFile(&texture, "kanagawa.jpeg") != 0)
	{
		pixFinalize();
		return 2;
	}
	// if (mesh3DInitializeCylinder(&mesh, 0.5, 4.0, 20) != 0)
	if (mesh3DInitializeBox(&mesh, -1, 1, 0.64, 2.0, -12, -10) != 0)
	{
		texFinalize(&texture);
		pixFinalize();
		return 3;
	}
	if (depthInitialize(&buf, 512, 512) != 0)
	{
		meshFinalize(&mesh);
		texFinalize(&texture);
		pixFinalize();
		return 4;
	}
	// if (mesh3DInitializeCapsule(&mesh1, 100.0, 50.0, 50, 50) != 0){
	if (mesh3DInitializeSphere(&mesh1, 0.5, 50, 50) != 0)
	{
		depthFinalize(&buf);
		meshFinalize(&mesh);
		texFinalize(&texture);
		pixFinalize();
		return 5;
	}

	mat44Viewport(512, 512, viewport);

	// camSetProjection(&cam, projection);

	// we will default initalize projection to orthographic
	camSetProjectionType(&cam, camORTHOGRAPHIC);

	camSetFrustum(&cam, M_PI / 6.0, 10.0, 10.0, 512, 512);
	camLookAt(&cam, target, rho, phi, theta);

	double proj[4][4];
	camGetProjectionInverseIsometry(&cam, proj);
	vecCopy(16, (double *)proj, &unif[UNIFPROJ]);

	texSetFiltering(&texture, texNEAREST);
	texSetLeftRight(&texture, texREPEAT);
	texSetTopBottom(&texture, texREPEAT);
	sha.unifDim = 16 + 16 + 16;
	sha.attrDim = 3 + 2 + 3;
	sha.varyDim = 4 + 2;
	sha.shadeVertex = shadeVertex;
	sha.shadeFragment = shadeFragment;
	sha.texNum = 1;

	// shading for sphere
	sha1.unifDim = 16 + 16 + 16;
	sha1.attrDim = 3 + 2 + 3;
	sha1.varyDim = 4 + 2;
	sha1.shadeVertex = shadeVertex1;
	sha1.shadeFragment = shadeFragment;
	sha1.texNum = 1;

	double isom[4][4];
	double rotation[3][3];
	double axis[3] = {1.0, 0.0, 0.0};
	mat33AngleAxisRotation(rotationAngle, axis, rotation);
	mat44Isometry(rotation, translationVectorRectangle, isom);
	vecCopy(16, (double *)isom, &unif[UNIFMODELING]);

	mat44Isometry(rotation, translationVectorSphere, isom);
	vecCopy(16, (double *)isom, &unif[UNIFMODELINGSPHERE]);

	render();
	pixSetKeyUpHandler(handleKeyUp);

	pixRun();
	depthFinalize(&buf);
	meshFinalize(&mesh);
	meshFinalize(&mesh1);
	texFinalize(&texture);
	pixFinalize();
	return 0;
}
