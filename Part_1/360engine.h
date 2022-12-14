


/*** Vectors in general dimensions ***/

/* Copies the dim-dimensional vector v to the dim-dimensional vector copy. The 
output can safely alias the input. */
void vecCopy(int dim, const double v[], double copy[]);

/* Adds the dim-dimensional vectors v and w. The output can safely alias the 
input. */
void vecAdd(int dim, const double v[], const double w[], double vPlusW[]);

/* Subtracts the dim-dimensional vectors v and w. The output can safely alias 
the input. */
void vecSubtract(
        int dim, const double v[], const double w[], double vMinusW[]);

/* Scales the dim-dimensional vector w by the number c. The output can safely 
alias the input.*/
void vecScale(int dim, double c, const double w[], double cTimesW[]);

/* Returns the dot product of the vectors v and w. */
double vecDot(int dim, const double v[], const double w[]);

/* Returns the length of the vector v. */
double vecLength(int dim, const double v[]);

/* Returns the length of the vector v. If the length is non-zero, then also 
places a normalized (length-1) version of v into unit. The output can safely 
alias the input. */
double vecUnit(int dim, const double v[], double unit[]);

/* Given two vectors v and w of the same dimension, produces a third vector of 
the same dimension, obtained by multiplying v and w component-wise. The output 
can safely alias the input. */
void vecModulate(int dim, const double v[], const double w[], double vw[]);



/*** Vectors in specific dimensions ***/

/* Copies three numbers into a three-dimensional vector. */
void vec3Set(double a0, double a1, double a2, double a[3]);

/* Copies four numbers into a four-dimensional vector. */
void vec4Set(double a0, double a1, double a2, double a3, double a[4]);

/* Copies eight numbers into a eight-dimensional vector. */
void vec8Set(
        double a0, double a1, double a2, double a3, double a4, double a5, 
		double a6, double a7, double a[8]);

/* Computes the cross product of v and w, and places it into vCrossW. The 
output CANNOT safely alias the input. */
void vec3Cross(const double v[3], const double w[3], double vCrossW[3]);

/* Computes the vector v from its spherical coordinates. rho >= 0.0 is the 
radius. 0 <= phi <= pi is the co-latitude. -pi <= theta <= pi is the longitude 
or azimuth. */
void vec3Spherical(double rho, double phi, double theta, double v[3]);



/*** 2 x 2 matrices ***/

/* Pretty-prints the given matrix, with one line of text per row of matrix. */
void mat22Print(const double m[2][2]);

/* Returns the determinant of the matrix m. If the determinant is 0.0, then the 
matrix is not invertible, and mInv is untouched. If the determinant is not 0.0, 
then the matrix is invertible, and its inverse is placed into mInv. The output 
CANNOT safely alias the input. */
double mat22Invert(const double m[2][2], double mInv[2][2]);

/* Multiplies a 2x2 matrix m by a 2-column v, storing the result in mTimesV. 
The output CANNOT safely alias the input. */
void mat221Multiply(const double m[2][2], const double v[2], double mTimesV[2]);

/* Fills the matrix m from its two columns. The output CANNOT safely alias the 
input. */
void mat22Columns(const double col0[2], const double col1[2], double m[2][2]);

/* The theta parameter is an angle in radians. Sets the matrix m to the 
rotation matrix corresponding to counterclockwise rotation of the plane through 
the angle theta. */
void mat22Rotation(double theta, double m[2][2]);



/*** 3 x 3 matrices ***/

/* Multiplies the 3x3 matrix m by the 3x3 matrix n. The output CANNOT safely 
alias the input. */
void mat333Multiply(
        const double m[3][3], const double n[3][3], double mTimesN[3][3]);

/* Multiplies the 3x3 matrix m by the 3x1 matrix v. The output CANNOT safely 
alias the input. */
void mat331Multiply(
        const double m[3][3], const double v[3], double mTimesV[3]);

/* Multiplies the transpose of the 3x3 matrix m by the 3x1 matrix v. To 
clarify, in math notation it computes M^T v. The output CANNOT safely alias the 
input. */
void mat331TransposeMultiply(
        const double m[3][3], const double v[3], double mTTimesV[3]);

/* Builds a 3x3 matrix representing 2D rotation and translation in homogeneous 
coordinates. More precisely, the transformation first rotates through the angle 
theta (in radians, counterclockwise), and then translates by the vector t. */
void mat33Isometry(double theta, const double t[2], double isom[3][3]);

/* Given a length-1 3D vector axis and an angle theta (in radians), builds the 
rotation matrix for the rotation about that axis through that angle. */
void mat33AngleAxisRotation(
        double theta, const double axis[3], double rot[3][3]);

/* Given two length-1 3D vectors u, v that are perpendicular to each other. 
Given two length-1 3D vectors a, b that are perpendicular to each other. Builds 
the rotation matrix that rotates u to a and v to b. */
void mat33BasisRotation(
        const double u[3], const double v[3], const double a[3], 
        const double b[3], double rot[3][3]);



/*** 4 x 4 matrices ***/

/* Computes the transpose M^T of the given matrix M. The output CANNOT safely 
alias the input. */
void mat44Transpose(const double m[4][4], double mT[4][4]);

/* Multiplies m by n, placing the answer in mTimesN. The output CANNOT safely 
alias the input. */
void mat444Multiply(
        const double m[4][4], const double n[4][4], double mTimesN[4][4]);

/* Multiplies m by v, placing the answer in mTimesV. The output CANNOT safely 
alias the input. */
void mat441Multiply(
        const double m[4][4], const double v[4], double mTimesV[4]);

/* Given a rotation and a translation, forms the 4x4 homogeneous matrix 
representing the rotation followed in time by the translation. */
void mat44Isometry(
        const double rot[3][3], const double trans[3], double isom[4][4]);

/* Sets its argument to the 4x4 zero matrix. */
void mat44Zero(double m[4][4]);

/* Builds a 4x4 matrix for a viewport with lower left (0, 0) and upper right 
(width, height). This matrix maps a projected viewing volume 
[-1, 1] x [-1, 1] x [-1, 1] to screen [0, w] x [0, h] x [0, 1] (each interval 
in that order). */
void mat44Viewport(double width, double height, double view[4][4]);

/* Inverse to the matrix produced by mat44Viewport. */
void mat44InverseViewport(double width, double height, double view[4][4]);



/*** Textures ***/

#define texLINEAR 0
#define texNEAREST 1
#define texREPEAT 2
#define texCLIP 3

typedef struct texTexture texTexture;
/* Feel free to read from this struct's members, but don't write to them. */
struct texTexture {
    int width, height;  /* do not have to be powers of 2 */
    int texelDim;       /* e.g. 3 for RGB textures */
    int filtering;      /* texLINEAR or texNEAREST */
    int topBottom;      /* texREPEAT or texCLIP */
    int leftRight;      /* texREPEAT or texCLIP */
    double *data;       /* width * height * texelDim doubles, row-major order */
};

/* Sets all texels within the texture. Assumes that the texture has already 
been initialized. Assumes that texel has the same texel dimension as the 
texture. */
void texClearTexels(texTexture *tex, const double texel[]);

/* Initializes a texTexture struct to a given width and height and a solid 
color. The width and height do not have to be powers of 2. Returns 0 if no 
error occurred. The user must remember to call texFinalize when finished with 
the texture. */
int texInitializeSolid(
        texTexture *tex, int width, int height, int texelDim, 
        const double texel[]);

/* Initializes a texTexture struct by loading an image from a file. Many image 
types are supported (using the public-domain STB Image library). The width and 
height do not have to be powers of 2. Returns 0 if no error occurred. The user 
must remember to call texFinalize when finished with the texture. */
/* WARNING: Currently there is a weird behavior, in which some image files show 
up with their rows and columns switched, so that their width and height are 
flipped. If that's happening with your image, then use a different image. */
int texInitializeFile(texTexture *tex, const char *path);

/* Sets the texture filtering, to either texNEAREST or texLINEAR. */
void texSetFiltering(texTexture *tex, int filtering);

/* Sets the texture wrapping for the top and bottom edges, to either texCLIP 
or texREPEAT. */
void texSetTopBottom(texTexture *tex, int topBottom);

/* Sets the texture wrapping for the left and right edges, to either texCLIP 
or texREPEAT. */
void texSetLeftRight(texTexture *tex, int leftRight);

/* Gets a single texel within the texture. Assumes that texel has the same texel 
dimension as the texture. Texel (s, t) = (0, 0) is in the lower left corner, 
texel (width - 1, 0) is in the lower right corner, etc. */
void texGetTexel(const texTexture *tex, int s, int t, double texel[]);

/* Sets a single texel within the texture. For details, see texGetTexel. */
void texSetTexel(texTexture *tex, int x, int y, const double texel[]);

/* Deallocates the resources backing the texture. This function must be called 
when the user is finished using the texture. */
void texFinalize(texTexture *tex);

/* Samples from the texture, taking into account wrapping and filtering. The s 
and t parameters are texture coordinates. The texture itself is assumed to have 
texture coordinates [0, 1] x [0, 1], with (0, 0) in the lower left corner, (1, 
0) in the lower right corner, etc. Assumes that the texture has already been 
initialized. Assumes that sample has been allocated with (at least) texelDim 
doubles. Places the sampled texel into sample. */
void texSample(const texTexture *tex, double s, double t, double sample[]);



/*** Shader programs ***/

typedef struct shaShading shaShading;
struct shaShading {
	int unifDim, attrDim, texNum, varyDim;
	void (*shadeVertex)(
	    int unifDim, const double unif[], int attrDim, const double attr[], 
	    int varyDim, double vary[]);
	void (*shadeFragment)(
	    int unifDim, const double unif[], int texNum, const texTexture *tex[], 
	    int varyDim, const double vary[], double rgbd[4]);
};



/*** Depth buffers ***/

/* Feel free to read the struct's members, but don't write them, except through 
the accessors below such as depthSetDepth, etc. */
typedef struct depthBuffer depthBuffer;
struct depthBuffer {
	int width, height;
	double *depths;			/* width * height doubles */
};

/* Initializes a depth buffer. When you are finished with the buffer, you must 
call depthFinalize to deallocate its backing resources. */
int depthInitialize(depthBuffer *buf, int width, int height);

/* Deallocates the resources backing the buffer. This function must be called 
when you are finished using a buffer. */
void depthFinalize(depthBuffer *buf);

/* Sets every depth-value to the given depth. Typically you use this function 
at the start of each frame, passing a large positive value for depth. */
void depthClearDepths(depthBuffer *buf, double depth);

/* Sets the depth-value at pixel (i, j) to the given depth. */
void depthSetDepth(depthBuffer *buf, int i, int j, double depth);

/* Returns the depth-value at pixel (i, j). */
double depthGetDepth(const depthBuffer *buf, int i, int j);



/*** Meshes ***/

/* Feel free to read the struct's members, but don't write them, except through 
the accessors below such as meshSetTriangle, meshSetVertex. */
typedef struct meshMesh meshMesh;
struct meshMesh {
	int triNum, vertNum, attrDim;
	int *tri;						/* triNum * 3 ints */
	double *vert;					/* vertNum * attrDim doubles */
};

/* Initializes a mesh with enough memory to hold its triangles and vertices. 
Does not actually fill in those triangles or vertices with useful data. When 
you are finished with the mesh, you must call meshFinalize to deallocate its 
backing resources. */
int meshInitialize(meshMesh *mesh, int triNum, int vertNum, int attrDim);

/* Sets the trith triangle to have vertex indices i, j, k. */
void meshSetTriangle(meshMesh *mesh, int tri, int i, int j, int k);

/* Returns a pointer to the trith triangle. For example:
	int *triangle13 = meshGetTrianglePointer(&mesh, 13);
	printf("%d, %d, %d\n", triangle13[0], triangle13[1], triangle13[2]); */
int *meshGetTrianglePointer(const meshMesh *mesh, int tri);

/* Sets the vertth vertex to have attributes attr. */
void meshSetVertex(meshMesh *mesh, int vert, const double attr[]);

/* Returns a pointer to the vertth vertex. For example:
	double *vertex13 = meshGetVertexPointer(&mesh, 13);
	printf("x = %f, y = %f\n", vertex13[0], vertex13[1]); */
double *meshGetVertexPointer(const meshMesh *mesh, int vert);

/* Deallocates the resources backing the mesh. This function must be called 
when you are finished using a mesh. */
void meshFinalize(meshMesh *mesh);

/* Initializes a mesh from a mesh file. The file format is simple and custom 
(not any industry standard). It is documented at meshSaveFile. This function 
does not do as much error checking as one might like. Use it only on trusted, 
non-corrupted files, such as ones that you have recently created using 
meshSaveFile. Returns 0 on success, non-zero on failure. Don't forget to invoke 
meshFinalize when you are done using the mesh. */
int meshInitializeFile(meshMesh *mesh, const char *path);

/* Saves a mesh to a file in a simple custom format (not any industry 
standard). Returns 0 on success, non-zero on failure. The first line is a 
comment of the form 'Carleton College CS 311 mesh version YYYY/MM/DD'.

I now describe version 2019/01/15. The second line says 'triNum [triNum]', 
where the latter is an integer value. The third and fourth lines do the same 
for vertNum and attrDim. The fifth line says '[triNum] Triangles:'. Then there 
are triNum lines, each holding three integers between 0 and vertNum - 1 
(separated by a space). Then there is a line that says '[vertNum] Vertices:'. 
Then there are vertNum lines, each holding attrDim floating-point numbers 
(terminated by a space). */
int meshSaveFile(const meshMesh *mesh, const char *path);

/* Renders the mesh. But if the mesh and the shading have differing values for 
attrDim, then prints an error message and does not render anything. */
void meshRender(
        const meshMesh *mesh, depthBuffer *buf, const double viewport[4][4], 
        const shaShading *sha, const double unif[], const texTexture *tex[]);



/*** 2D mesh builders ***/

/* Initializes a mesh to two triangles forming a rectangle of the given sides. 
The four attributes are X, Y, S, T. Do not call meshInitialize separately; it 
is called inside this function. Don't forget to call meshFinalize when done. */
int mesh2DInitializeRectangle(
        meshMesh *mesh, double left, double right, double bottom, double top);

/* Initializes a mesh to sideNum triangles forming an ellipse of the given 
center (x, y) and radii rx, ry. The four attributes are X, Y, S, T. Do not call 
meshInitialize separately; it is called inside this function. Don't forget to 
call meshFinalize when done. */
int mesh2DInitializeEllipse(
        meshMesh *mesh, double x, double y, double rx, double ry, int sideNum);



/*** 3D mesh builders ***/

/* Builds a mesh for a parallelepiped (box) of the given size. The attributes 
are XYZ position, ST texture, and NOP unit normal vector. The normals are 
discontinuous at the edges (flat shading, not smooth). To facilitate this, some 
vertices have equal XYZ but different NOP, for 24 vertices in all. Don't forget 
to meshFinalize when finished. */
int mesh3DInitializeBox(
        meshMesh *mesh, double left, double right, double bottom, double top, 
        double base, double lid);

/* Rotate a curve about the Z-axis. Can be used to make a sphere, spheroid, 
capsule, circular cone, circular cylinder, box, etc. The z-values should be in 
ascending order --- or at least the first z should be less than the last. The 
first and last r-values should be 0.0, and no others. Probably the t-values 
should be in ascending or descending order. The sideNum parameter controls the 
fineness of the mesh. The attributes are XYZ position, ST texture, and NOP unit 
normal vector. The normals are smooth. Don't forget to meshFinalize when 
finished. */
int mesh3DInitializeRevolution(
        meshMesh *mesh, int zNum, const double z[], const double r[], 
        const double t[], int sideNum);

/* Builds a mesh for a sphere, centered at the origin, of radius r. The sideNum 
and layerNum parameters control the fineness of the mesh. The attributes are 
XYZ position, ST texture, and NOP unit normal vector. The normals are smooth. 
Don't forget to meshFinalize when finished. */
int mesh3DInitializeSphere(
        meshMesh *mesh, double r, int layerNum, int sideNum);

/* Builds a mesh for a circular cylinder with spherical caps, centered at the 
origin, of radius r and length l > 2 * r. The sideNum and layerNum parameters 
control the fineness of the mesh. The attributes are XYZ position, ST texture, 
and NOP unit normal vector. The normals are smooth. Don't forget to meshFinalize 
when finished. */
int mesh3DInitializeCapsule(
        meshMesh *mesh, double r, double l, int layerNum, int sideNum);

/* Builds a mesh for a circular cylinder, centered at the origin, of radius r 
and length l. The sideNum parameter controls the fineness of the mesh. The 
attributes are XYZ position, ST texture, and NOP unit normal vector. The normals 
are smooth except where the side meets the ends. Don't forget to meshFinalize 
when finished. */
int mesh3DInitializeCylinder(meshMesh *mesh, double r, double l, int sideNum);

/* Builds a non-closed 'landscape' mesh based on a grid of Z-values. There are 
size * size Z-values, which arrive in the data parameter. The mesh is made of 
(size - 1) * (size - 1) squares, each made of two triangles. The spacing 
parameter controls the spacing of the X- and Y-coordinates of the vertices. The 
attributes are XYZ position, ST texture, and NOP unit normal vector. Don't 
forget to call meshFinalize when finished with the mesh. To understand the exact 
layout of the data, try this example code:
double zs[3][3] = {
    {10.0, 9.0, 7.0}, 
    {6.0, 5.0, 3.0}, 
    {4.0, 3.0, -1.0}};
int error = mesh3DInitializeLandscape(&mesh, 3, 20.0, (double *)zs); */
int mesh3DInitializeLandscape(
        meshMesh *mesh, int size, double spacing, const double *data);

/* Given a landscape, such as that built by meshInitializeLandscape. Builds a 
new landscape mesh by extracting triangles based on how horizontal they are. If 
noMoreThan is true, then triangles are kept that deviate from horizontal by no more than angle. If noMoreThan is false, then triangles are kept that deviate 
from horizontal by more than angle. Don't forget to call meshFinalize when 
finished. Warning: May contain extraneous vertices not used by any triangle. */
int mesh3DInitializeDissectedLandscape(
        meshMesh *mesh, const meshMesh *land, double angle, int noMoreThan);



/*** Isometries ***/

/* Feel free to read from, but not write to, this struct's members. */
typedef struct isoIsometry isoIsometry;
struct isoIsometry {
	double translation[3];
	double rotation[3][3];
};

/* Sets the rotation. */
void isoSetRotation(isoIsometry *iso, const double rot[3][3]);

/* Sets the translation. */
void isoSetTranslation(isoIsometry *iso, const double transl[3]);

/* Applies the rotation and translation to a point. The output CANNOT safely 
alias the input. */
void isoTransformPoint(
        const isoIsometry *iso, const double p[3], double isoP[3]);

/* Applies the inverse of the isometry to a point. If you transform a point and 
then untransform the result, then you recover the original point. Similarly, if 
you untransform a point and then transform the result, then you recover the 
original point. The output CANNOT safely alias the input. */
void isoUntransformPoint(
        const isoIsometry *iso, const double isoP[3], double p[3]);

/* Applies the rotation to a direction vector (typically unit). The output 
CANNOT safely alias the input. */
void isoRotateDirection(
        const isoIsometry *iso, const double d[3], double rotD[3]);

/* Applies the inverse rotation to a direction vector (typically unit). The 
output CANNOT safely alias the input. */
void isoUnrotateDirection(
        const isoIsometry *iso, const double rotD[3], double d[3]);

/* Fills homog with the homogeneous version of the isometry. */
void isoGetHomogeneous(const isoIsometry *iso, double homog[4][4]);

/* Fills homog with the homogeneous version of the inverse isometry. That is, 
the product of this matrix and the one from isoGetHomogeneous is the identity 
matrix. */
void isoGetInverseHomogeneous(const isoIsometry *iso, double homogInv[4][4]);



/*** Cameras ***/

/* Feel free to read from this struct's members, but don't write to them. */
typedef struct camCamera camCamera;
struct camCamera {
	double projection[6];
	int projectionType;
	isoIsometry isometry;
};

#define camORTHOGRAPHIC 0
#define camPERSPECTIVE 1
#define camPROJL 0
#define camPROJR 1
#define camPROJB 2
#define camPROJT 3
#define camPROJF 4
#define camPROJN 5

/* Sets the projection type, to either camORTHOGRAPHIC or camPERSPECTIVE. */
void camSetProjectionType(camCamera *cam, int projType);

/* Sets all six projection parameters. */
void camSetProjection(camCamera *cam, const double proj[6]);

/* Sets one of the six projection parameters. */
void camSetOneProjection(camCamera *cam, int i, double value);

/* Builds a 4x4 matrix representing orthographic projection with a boxy viewing 
volume [left, right] x [bottom, top] x [far, near]. That is, on the near plane 
the box is the rectangle R = [left, right] x [bottom, top], and on the far 
plane the box is the same rectangle R. Keep in mind that 0 > near > far. Maps 
the viewing volume to [-1, 1] x [-1, 1] x [-1, 1], with far going to 1 and near 
going to -1. */
void camGetOrthographic(const camCamera *cam, double proj[4][4]);

/* Inverse to the matrix produced by camGetOrthographic. */
void camGetInverseOrthographic(const camCamera *cam, double proj[4][4]);

/* Builds a 4x4 matrix representing perspective projection. The viewing frustum 
is contained between the near and far planes, with 0 > near > far. On the near 
plane, the frustum is the rectangle R = [left, right] x [bottom, top]. On the 
far plane, the frustum is the rectangle (far / near) * R. Maps the viewing 
volume to [-1, 1] x [-1, 1] x [-1, 1], with far going to 1 and near going to 
-1. */
void camGetPerspective(const camCamera *cam, double proj[4][4]);

/* Inverse to the matrix produced by camGetPerspective. */
void camGetInversePerspective(const camCamera *cam, double proj[4][4]);

/* Sets the six projection parameters, based on the width and height of the 
viewport and three other parameters. The camera looks down the center of the 
viewing volume. For perspective projection, fovy is the full (not half) 
vertical angle of the field of vision, in radians. focal > 0 is the distance 
from the camera to the 'focal' plane (where 'focus' is used in the sense of 
attention, not optics). ratio expresses the far and near clipping planes 
relative to focal: far = -focal * ratio and near = -focal / ratio. Reasonable 
values are fovy = M_PI / 6.0, focal = 10.0, and ratio = 10.0, so that 
far = -100.0 and near = -1.0. For orthographic projection, the projection 
parameters are set to produce the orthographic projection that, at the focal 
plane, is most similar to the perspective projection just described. You must 
re-invoke this function after each time you resize the viewport. */
void camSetFrustum(
        camCamera *cam, double fovy, double focal, double ratio, double width, 
        double height);

/* Returns the homogeneous 4x4 product of the camera's projection and the 
camera's inverse isometry (regardless of whether the camera is in orthographic 
or perspective mode). */
void camGetProjectionInverseIsometry(const camCamera *cam, double homog[4][4]);

/* Sets the camera's isometry, in a manner suitable for third-person viewing. 
The camera is aimed at the world coordinates target. The camera itself is 
displaced from that target by a distance rho, in the direction specified by the 
spherical coordinates phi and theta (as in vec3Spherical). Under normal use, 
where 0 < phi < pi, the camera's up-direction is world-up, or as close to it as 
possible. */
void camLookAt(
        camCamera *cam, const double target[3], double rho, double phi, 
		double theta);

/* Sets the camera's isometry, in a manner suitable for first-person viewing. 
The camera is positioned at the world coordinates position. From that position, 
the camera's sight direction is described by the spherical coordinates phi and 
theta (as in vec3Spherical). Under normal use, where 0 < phi < pi, the camera's 
up-direction is world-up, or as close to it as possible. */
void camLookFrom(
        camCamera *cam, const double position[3], double phi, double theta);



/*** Landscapes ***/

/* A landscape is simply a square array of doubles, with each one giving an 
elevation. This file contains functions for generating landscapes. Before 
calling any of the randomized functions, you must seed the random number 
generator, for example by doing

    #include <time.h>
    time_t t;
	srand((unsigned)time(&t));

To turn the landscape into a mesh, use the appropriate 3D mesh initializer 
functions. */

/* Makes a flat landscape with the given elevation. */
void landFlat(int size, double *data, double elevation);

/* Returns a random integer in [a, b]. Before using this function, call 
srand(). Warning: This is a poor-quality generator. It is not suitable for 
serious cryptographic or statistical applications. */
int landInt(int a, int b);

/* Returns a random double in [a, b]. Before using this function, call srand(). 
Warning: This is a poor-quality generator. It is not suitable for serious 
cryptographic or statistical applications. */
double landDouble(double a, double b);

/* Given a line y = m x + b across the landscape (with the x-axis pointing east 
and the y-axis pointing north), raises points north of the line and lowers 
points south of it (or vice-versa). */
void landFaultEastWest(
        int size, double *data, double m, double b, double raisingNorth);

/* Given a line x = m y + b across the landscape (with the x-axis pointing east 
and the y-axis pointing north), raises points east of the line and lower points 
west of it (or vice-versa). */
void landFaultNorthSouth(
        int size, double *data, double m, double b, double raisingEast);

/* Randomly chooses a vertical fault and slips the landscape up and down on the 
two sides of that fault. Before using this function, call srand(). */
void landFaultRandomly(int size, double *data, double magnitude);

/* Blurs each non-border elevation with the eight elevations around it. */
void landBlur(int size, double *data);

/* Forms a Gaussian hill or valley at (x, y), with width controlled by stddev 
and height/depth controlled by raising. */
void landBump(
        int size, double *data, int x, int y, double stddev, double raising);

/* Computes the min, mean, and max of the elevations. */
void landStatistics(
        int size, double *data, double *min, double *mean, double *max);


