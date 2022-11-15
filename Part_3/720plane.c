


/* A plane has no geometry uniforms. */
#define plaUNIFDIM 0

/* An implementation of getIntersection for bodies that are planes. */
void plaGetIntersection(
        int unifDim, const double unif[], const isoIsometry *isom, 
        const double p[3], const double d[3], double bound, 
        rayIntersection* inter) {


    double p_local[3], d_local[3];
    isoUntransformPoint(isom, p, p_local);
    isoUnrotateDirection(isom, d, d_local);

//     isoUnrotateDirection(isom, p_untrans, p_local);

    inter->t = rayNONE;

    if(d_local[2] != 0){
        double t = -p_local[2] / d_local[2];

        if(t>= rayEPSILON && t <= bound)
           inter->t = t;
        else
           inter->t = rayNONE;

    }else
        inter->t = rayNONE;

        

}

/* An implementation of getTexCoordsAndNormal for bodies that are planes. */
void plaGetTexCoordsAndNormal(
        int unifDim, const double unif[], const isoIsometry *isom, 
        const double p[3], const double d[3], const rayIntersection *inter, 
        double texCoords[2], double normal[3]) {
    
    double normal_local[3] = {0.0, 0.0, 1.0};
    isoRotateDirection(isom, normal_local, normal);

    double p_local[3], d_local[3];
    isoUntransformPoint(isom, p, p_local);
    isoUnrotateDirection(isom, d, d_local);

    texCoords[0] = p_local[0] + inter->t * d_local[0];
    texCoords[1] = p_local[1] + inter->t * d_local[1];


}


