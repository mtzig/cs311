


/* 
Thomas Zeng & Shaun Baron-Furuyama

On macOS, compile with...
    clang 200mainAbstracted.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLFW/glfw3.h>

#include "040pixel.h"

#include "080vector.c"
#include "100matrix.c"
#include "150texture.c"
#include "200shading.c"

/* It is important that shadeFragment correctly parse the data that we give it. 
To help avoid errors in communication, we set up some constants. Notice that the 
documentation for the new triRender requires ATTRX to be 0 and ATTRY to be 1. */
#define ATTRX 0
#define ATTRY 1
#define ATTRS 2
#define ATTRT 3


#define TEXR 0
#define TEXG 1
#define TEXB 2

#define VARYX 0
#define VARYY 1
#define VARYS 2
#define VARYT 3

#define UNIFROT 0
#define UNIFTRANSLX 1
#define UNIFTRANSLY 2
/* attr has already been interpolated from the vertex attributes. tex is an 
array of texNum elements, each of which is of type texTexture *. rgb is the 
output parameter. The other parameters should be self-explanatory. This function 
is not allowed to read from any variables other than its arguments and local 
variables. It is not allowed to write to any variables except for its rgb 
parameter and local variables. In particular, it cannot use any global 
variables. */
void shadeFragment(
        int unifDim, const double unif[], int texNum, const texTexture *tex[], 
        int attrDim, const double vary[], double rgb[3]) {

        texSample(tex[0], vary[VARYS], vary[VARYT], rgb);
		// vecModulate(3, attr+ATTRR, rgb, rgb); //interpolated rgb is at the pluss ATTR position
		// vecModulate(unifDim, unif, rgb, rgb );

}

/* Outputs vary, based on the other parameters, which are unaltered. Like 
shadeFragment, this function should not access any variables other than its 
parameters and any local variables that it declares. */
void shadeVertex(
        int unifDim, const double unif[], int attrDim, const double attr[], 
        int varyDim, double vary[]) {
	double rot[2][2];
	mat22Rotation(unif[UNIFROT], rot);
	mat221Multiply(rot, attr, vary);
	
	vary[VARYX] += unif[UNIFTRANSLX];
	vary[VARYY] += unif[UNIFTRANSLY];
	vary[VARYS] = attr[ATTRS];
	vary[VARYT] = attr[ATTRT];


}

/* We have to include triangle.c after defining shadeFragment, because triRender 
refers to shadeFragment. (Later in the course we handle this issue better.) */
#include "170triangle.c"

#include "200mesh.c"
#include "190mesh2D.c"

/* This struct is initialized in main() below. */
shaShading sha;
/* Here we make an array of one texTexture pointer, in such a way that the 
const qualifier can be enforced throughout the surrounding code. C is confusing 
for stuff like this. Don't worry about mastering C at this level. It doesn't 
come up much in our course. */
texTexture texture;
const texTexture *textures[1] = {&texture};
const texTexture **tex = textures;

meshMesh mesh;

double unif[3] = {0.0, 0.0, 0.0};

void render(void) {
	pixClearRGB(0.0, 0.0, 0.0);

	meshRender(&mesh, &sha, unif, tex);    

}

void handleTimeStep(double oldTime, double newTime) {
	unif[UNIFROT] += (newTime - oldTime) / 10;

	

	unif[UNIFTRANSLX] += 0.1;
	unif[UNIFTRANSLY] += 0.1;
	

	render();

}

void handleKeyUp(int key, int shiftIsDown, int controlIsDown, 
		int altOptionIsDown, int superCommandIsDown) {
	if (key == GLFW_KEY_ENTER) {
		if (texture.filtering == texLINEAR)
			texSetFiltering(&texture, texNEAREST);
		else
			texSetFiltering(&texture, texLINEAR);
	}
}



int main(void) {
	if (pixInitialize(1024, 1024, "Abstracted") != 0)
		return 1;
	if (texInitializeFile(&texture, "kanagawa.jpeg") != 0) {
	    pixFinalize();
		return 2;
	}

	if (mesh2DInitializeEllipse(&mesh, 200,0, 200.0, 200.0, 200.0) != 0)
		return 3;

    texSetFiltering(&texture, texNEAREST);
    texSetLeftRight(&texture, texREPEAT);
    texSetTopBottom(&texture, texREPEAT);
    sha.unifDim = 3;
    sha.attrDim = 2 + 2;
	sha.varyDim = 2 + 2;
    sha.texNum = 1;

    pixSetKeyUpHandler(handleKeyUp);
	pixSetTimeStepHandler(handleTimeStep);

    pixRun();
	meshFinalize(&mesh);
    texFinalize(&texture);
    pixFinalize();
    return 0;
}


