/*
Thomas Zeng & Shaun Baron-Furuyama
*/


typedef struct shaShading shaShading;

struct shaShading {
    int unifDim;
    int attrDim;
    int texNum;
    int varyDim;
    void (*shadeVertex)( int, const double *, int, const double *, int, double *);
    void (*shadeFragment)(int, const double *, int, const texTexture **, int, const double *, double *);
};

