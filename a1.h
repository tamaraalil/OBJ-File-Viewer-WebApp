#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Program Name: a1.h
 * Author: Tamara Alilovic
 */

typedef struct faces {
    int v;
    int vt;
    int vn;
} Faces;

typedef struct object {
    char *objName;
    char *grpName;
    float **v;
    float **vn;
    float **vt;
    int numVReallocs;
    int numVNReallocs;
    int numVTReallocs;
    int numFReallocs;
    int numFaces;
    int numVertices;
    int numVNormals;
    int numVTextures;
    Faces **f;
} Object;

typedef struct rgb {
    int height;
    int width;
    int depth;
    int **r;
    int **g;
    int **b;
} RGB;
