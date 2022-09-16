


/*** 2 x 2 Matrices ***/

/* Pretty-prints the given matrix, with one line of text per row of matrix. */
void mat22Print(const double m[2][2]) {
    int i, j;
    for (i = 0; i < 2; i += 1) {
        for (j = 0; j < 2; j += 1)
            printf("%f    ", m[i][j]);
        printf("\n");
    }
}

/* Returns the determinant of the matrix m. If the determinant is 0.0, then the 
matrix is not invertible, and mInv is untouched. If the determinant is not 0.0, 
then the matrix is invertible, and its inverse is placed into mInv. The output 
CANNOT safely alias the input. */
double mat22Invert(const double m[2][2], double mInv[2][2]) {
    
}

/* Multiplies a 2x2 matrix m by a 2-column v, storing the result in mTimesV. 
The output CANNOT safely alias the input. */
void mat221Multiply(const double m[2][2], const double v[2], 
        double mTimesV[2]) {
    
}

/* Fills the matrix m from its two columns. The output CANNOT safely alias the 
input. */
void mat22Columns(const double col0[2], const double col1[2], double m[2][2]) {
    
}

/* The theta parameter is an angle in radians. Sets the matrix m to the 
rotation matrix corresponding to counterclockwise rotation of the plane through 
the angle theta. */
void mat22Rotation(double theta, double m[2][2]) {
    
}


