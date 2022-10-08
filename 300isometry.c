


/* Describes an isometry as a rotation followed by a translation. Can be used 
to describe the position and orientation of a rigid body. If the position is 
the translation, and the columns of the rotation are the local coordinate axes 
in global coordinates, then the isometry takes local coordinates to global. */

/* Feel free to read from, but not write to, this struct's members. */
typedef struct isoIsometry isoIsometry;
struct isoIsometry {
	double translation[3];
	double rotation[3][3];
};

/* Sets the rotation. */
void isoSetRotation(isoIsometry *iso, const double rot[3][3]) {
	vecCopy(9, (double *)rot, (double *)(iso->rotation));
}

/* Sets the translation. */
void isoSetTranslation(isoIsometry *iso, const double transl[3]) {
	vecCopy(3, transl, iso->translation);
}

/* Applies the rotation and translation to a point. The output CANNOT safely 
alias the input. */
void isoTransformPoint(
        const isoIsometry *iso, const double p[3], double isoP[3]) {
	mat331Multiply(iso->rotation, p, isoP);
	vecAdd(3, isoP, iso->translation, isoP);
}

/* Applies the inverse of the isometry to a point. If you transform a point and 
then untransform the result, then you recover the original point. Similarly, if 
you untransform a point and then transform the result, then you recover the 
original point. The output CANNOT safely alias the input. */
void isoUntransformPoint(
        const isoIsometry *iso, const double isoP[3], double p[3]) {
	
	double untrans[3];
	vecSubtract(3, isoP, iso->translation, untrans);
	mat331TransposeMultiply(iso->rotation, untrans, p);

	
}

/* Applies the rotation to a direction vector (typically unit). The output 
CANNOT safely alias the input. */
void isoRotateDirection(
        const isoIsometry *iso, const double d[3], double rotD[3]) {
	mat331Multiply(iso->rotation, d, rotD);
}

/* Applies the inverse rotation to a direction vector (typically unit). The 
output CANNOT safely alias the input. */
void isoUnrotateDirection(
        const isoIsometry *iso, const double rotD[3], double d[3]) {
	mat331TransposeMultiply(iso->rotation, rotD, d);
}

/* Fills homog with the homogeneous version of the isometry. */
void isoGetHomogeneous(const isoIsometry *iso, double homog[4][4]) {
	mat44Isometry(iso->rotation, iso->translation, homog);
}

/* Fills homog with the homogeneous version of the inverse isometry. That is, 
the product of this matrix and the one from isoGetHomogeneous is the identity 
matrix. */
void isoGetInverseHomogeneous(const isoIsometry *iso, double homogInv[4][4]) {

	double rotateInverse[3][3], translateInverse[3];
	// inverse i.e. transpose of rotation
	mat33Transpose(iso->rotation, rotateInverse);

	// "inverse" of translation
	mat331Multiply(rotateInverse, iso->translation, translateInverse);
	for(int i=0 ; i<2; i++)
		translateInverse[i] = -translateInverse[i];

	mat44Isometry(rotateInverse, translateInverse, homogInv);

}


