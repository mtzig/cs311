/*
Thomas Zeng & Kyosuke
*/


/* A resh is a ray-tracing mesh. It has no geometry uniforms outside the 
attached meshMesh. */
#define reshUNIFDIM 0

/* Given vectors a, b - a, and c - a describing a triangle, with the first three 
entries being XYZ. Given point x, with the first three entries being XYZ, such 
that (up to numerical precision) x - a = p (b - a) + q (c - a). Computes p and 
q. Returns 1 on success or 0 on failure. */
int reshGetPQ(
        const double a[], const double bMinusA[], const double cMinusA[], 
        const double x[], double pq[2]) {
    /* For the 3x2 matrix A with columns b - a, c - a, compute A^T A. */
    double aTA[2][2];
    aTA[0][0] = vecDot(3, bMinusA, bMinusA);
    aTA[0][1] = vecDot(3, bMinusA, cMinusA);
    aTA[1][0] = aTA[0][1];
    aTA[1][1] = vecDot(3, cMinusA, cMinusA);
    /* Compute the 2x2 matrix (A^T A)^-1 if possible. */
    double aTAInv[2][2];
    if (mat22Invert(aTA, aTAInv) == 0.0)
        return 0;
    /* Compute the 2x3 matrix (A^T A)^-1 A^T. */
    double aTAInvAT[2][3];
    for (int i = 0; i < 2; i += 1)
        for (int j = 0; j < 3; j += 1)
            aTAInvAT[i][j] = 
                aTAInv[i][0] * bMinusA[j] + aTAInv[i][1] * cMinusA[j];
    /* Then pq = (A^T A)^-1 A^T (x - a). */
    double xMinusA[3];
    vecSubtract(3, x, a, xMinusA);
    pq[0] = vecDot(3, aTAInvAT[0], xMinusA);
    pq[1] = vecDot(3, aTAInvAT[1], xMinusA);
    return 1;
}

/* An implementation of getIntersection for bodies that are reshes. Assumes that 
the data parameter points to an underlying meshMesh with attribute structure 
XYZSTNOP. */
void reshGetIntersection(
        int unifDim, const double unif[], const void *data, 
        const isoIsometry *isom, const double p[3], const double d[3], 
        double bound, rayIntersection* inter) {
    meshMesh *mesh = (meshMesh *)data;

    double p_local[3], d_local[3];
    isoUntransformPoint(isom, p, p_local);
    isoUnrotateDirection(isom, d, d_local);


    // init a bunch of variables used in calculation
    double bMa[3], cMa[3], n[3], nDd, aMp[3], nDaMp, t, x[3], pq[2];

    // double b = bound; // our bound starts at bound

    inter->t = rayNONE; // intialize t to rayNONE

    for (int tri = 0; tri < mesh->triNum; tri++){

		int *verts = meshGetTrianglePointer(mesh, tri);


        // meshGetVertexPointer(mesh, verts[0]);
        double *a = meshGetVertexPointer(mesh, verts[0]);
        double *b = meshGetVertexPointer(mesh, verts[1]);
        double *c = meshGetVertexPointer(mesh, verts[2]);

        // check if hit plane

        vecSubtract(3, b, a, bMa);
        vecSubtract(3, c, a, cMa);
        vec3Cross(bMa, cMa, n);

        nDd = vecDot(3, n, d_local);

        if(nDd == 0){
            continue;
        }
        else{
            vecSubtract(3, a, p_local, aMp);
            nDaMp = vecDot(3, n, aMp);

            t = nDaMp / nDd;

            if(t > bound || t < rayEPSILON)
                continue;
        }

        // check if hit triangle

        vecScale(3, t, d_local, x);
        vecAdd(3, p_local, x, x);


        if(reshGetPQ(a, bMa, cMa, x, pq)==0)
            continue; // didn't intersect in triangle

        // if truly interesect within triangle, update values
        if(pq[0] >= 0 && pq[1] >= 0 && pq[0]+pq[1] <= 1){
            inter->t = t;
            inter->index = tri;
            bound = t;
        }

	}
}

/* An implementation of getTexCoordsAndNormal for bodies that are reshes. 
Assumes that the data parameter points to an underlying meshMesh with attribute 
structure XYZSTNOP. */
void reshGetTexCoordsAndNormal(
        int unifDim, const double unif[], const void *data, 
        const isoIsometry *isom, const double p[3], const double d[3], 
        const rayIntersection *inter, double texCoords[2], double normal[3]) {
    meshMesh *mesh = (meshMesh *)data;
    // texCoords[0] = 0.5;
    // texCoords[1] = 0.5;
    // vec3Set(0.0, 0.0, 1.0, normal);

    double p_local[3], d_local[3];
    isoUntransformPoint(isom, p, p_local);
    isoUnrotateDirection(isom, d, d_local);
	
    int *verts = meshGetTrianglePointer(mesh, inter->index);
    
    double *a = meshGetVertexPointer(mesh, verts[0]);
    double *b = meshGetVertexPointer(mesh, verts[1]);
    double *c = meshGetVertexPointer(mesh, verts[2]);

    double bMa[8], cMa[8], x_local[3], pq[2], x[8], temp[8];

    vecSubtract(8, b, a, bMa);
    vecSubtract(8, c, a, cMa);


    vecScale(3, inter->t, d_local, x_local);
    vecAdd(3, p_local, x_local, x_local);

    reshGetPQ(a, bMa, cMa, x_local, pq);

    vecScale(8, pq[0], bMa, x);

    vecAdd(8, a, x, x);

    vecScale(8, pq[1], cMa, temp);

    vecAdd(8, x, temp, x);

    vecCopy(3, x+5, normal);

    vecUnit(3, normal, normal);

    isoRotateDirection(isom, normal, normal);

    texCoords[0] = x[3];
    texCoords[1] = x[4];

}


