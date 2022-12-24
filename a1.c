#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "a1.h"
#define PY_SSIZE_T_CLEAN

/*
 * Program Name: a1.c
 * Author: Tamara Alilovic
 */

int *ppmdata;
float *vertices;
float *vnormals;
float *vtextures;
int *indices;

/* Returns the next line in the inputted file */
void getLine (char *line, char c, FILE *fp) {
    int length = 1;
    int flag = 0;
    if (c == '\n') {
        /* If the line is empty, return an empty line */
        line[0] = '\n';
        line[1] = '\0';
        flag = 1;
    }
    while (c != '\n') {
        /* Save contents of line until newline */
        line[length - 1] = c;
        length++;
        c = getc(fp);
    }
    if (line == NULL) {
        /* Prevents seg fault from printing EOF */
        line = " ";
    }
    if (flag != 1) {
        line[length-1] = '\0';
    }
}

/* Tokenizes line by whitespace */
char **tokenizeLine(char **token, char *line) {
    char *temp = strtok(line, " \n\t");
    int i = 0;
    while(temp != NULL) {
        token[i++] = temp;
        temp = strtok(NULL, " \n\t");
    }
    token[i] = NULL;
    return token;
}

/* Parse v lines and save them into the struct */
Object *parseVertices(Object *obj, char **tokens) {
    static int i = 1;
    static int reall = 2;
    if (i == (1024*(reall/2)) ) {
        /* If inital space malloced is full, double the array size */
        obj->v = realloc(obj->v, sizeof(float *) * 1024*reall);
        int j = i;
        for (j = i; j < 1024*reall; j++) {
            obj->v[j] = malloc(sizeof(float) * 3);
        }
        obj->numVReallocs = reall;
        reall = reall * 2;
    }
    /* Save elements to struct */
    obj->v[i][0] = atof(tokens[1]);
    obj->v[i][1] = atof(tokens[2]);
    obj->v[i][2] = atof(tokens[3]);
    obj->numVertices++;
    i++;
    return obj;
}

/* Parse v lines and save them into the struct */
Object *parseVNormals(Object *obj, char **tokens) {
    static int i = 1;
    static int reall = 2;
    if (i == (1024*(reall/2))) {
        /* If inital space malloced is full, double the array size */
        obj->vn = realloc(obj->vn, sizeof(float *) * 1024*reall);
        int j = i;
        for (j = i; j < 1024*reall; j++) {
            obj->vn[j] = malloc(sizeof(float) * 3);
        }
        obj->numVNReallocs = reall;
        reall = reall * 2;
    }
    /* Save elements to struct */
    obj->vn[i][0] = atof(tokens[1]);
    obj->vn[i][1] = atof(tokens[2]);
    obj->vn[i][2] = atof(tokens[3]);
    obj->numVNormals++;
    i++;
    return obj;
}

/* Parse vt lines and save them to the struct */
Object *parseVTextures(Object *obj, char **tokens) {
    static int i = 1;
    static int reall = 2;
    if (i == (1024*(reall/2))) {
        /* If inital space malloced is full, double the array size */
        obj->vt = realloc(obj->vt, sizeof(float *) * 1024*reall);
        int j = i;
        for (j = i; j < 1024*reall; j++) {
            obj->vt[j] = malloc(sizeof(float) * 2);
        }
        obj->numVTReallocs = reall;
        reall = reall * 2;
    }
    /* Save elements to struct */
    obj->vt[i][0] = atof(tokens[1]);
    obj->vt[i][1] = atof(tokens[2]);
    obj->numVTextures++;
    i++;
    return obj;
}

/* Tokenize elements in f line by "/" */
char **tokenizeFLine(char **token, char *line) {
    /* Initialize array with null characters */
    int j = 0;
    int x = 0;
    for (j = 0; j < 5; j++) {
        for (x = 0; x < 10; x++) {
            token[j][x] = '\0';
        }
    }
    int i = 0;
    int e = 0;
    j = 0;
    for(j = 0; j < (int)strlen(line); j++) {
        if (line[j] != '/') {
            token[e][i] = line[j];
            i++;
        } else if(line[j] == '/' && line[j+1]=='/'){
            /* If there is no vt line, input a 0 and continue */
            token[e][i+1] = '0';
            token[e][i+1] = '\0';
            i = 0;
            e++;
        } else {
            /* When a / is reached, null-terminate the element and move to the next one */
            token[e][i] =  '\0';
            i = 0;
            e++;
        }
    }
    return token;
}

/* Helper function to parseFace - saves elements to struct */
void parseFacesHelper(Object *obj, char *nums, int i) {
    char **numbers = malloc(sizeof(char*) * 5);
    int k = 0;
    for (k = 0; k < 5; k++) {
        numbers[k] = malloc(sizeof(char) * 10);
    }
    numbers = tokenizeFLine(numbers, nums); /* Separate input by / */
    static int reall = 2;
    if (obj->numFaces == (1024*(reall/2)) && obj->numFaces!=0) {
        /* If initial space malloced is full, double array size */
        obj->f = realloc(obj->f, sizeof(Faces *) * (1024*reall));
        int j = obj->numFaces;
        for (j = obj->numFaces; j < 1024*reall; j++) {
            obj->f[j] = malloc(sizeof(Faces) * 4);
        }
        obj->numFReallocs = reall;
        reall = reall * 2;
    }
    /* Save the v number to the struct, and vt and vn if applicable */
    obj->f[obj->numFaces][i].v = atoi(numbers[0]);
    if (strcmp(numbers[1], "0") != 0) {
        obj->f[obj->numFaces][i].vt = atoi(numbers[1]);
    }
    if (strcmp(numbers[2], "0") != 0) {
        obj->f[obj->numFaces][i].vn = atoi(numbers[2]);
    }
    i++;
    /* Free numbers array */
    k = 0;
    for (k = 0; k < 5; k++) {
        free(numbers[k]);
    }
    free(numbers);
}

/* Parse f lines and save them to the struct */
Object *parseFaces(Object *obj, char **tokens) {
    static int i = 1;
    static int j = 1;
    if (tokens[j] != NULL) {
        /* While there are elements in the f line, parse them */
        int indexCount = j - 1;
        parseFacesHelper(obj, tokens[j], indexCount);
        j++;
    } 
    if (tokens[j] != NULL) {
        i++;
        parseFaces(obj, tokens); /* Move to next f line */
    } else {
        j = 1;
    }
    return obj;
}

/* Process data by detecting which line is being read in (v, vt, vn, f) and parse accordingly */
void processData(FILE *fp, Object *obj) {
    char c = getc(fp);
    static int numLines = 1;
    char **tokens = malloc(sizeof(char**) * 256);
    char *line = malloc(sizeof(char *) * 400);
    
    if (numLines == 1 && c == EOF) { /* Empty file error handling */
        printf("Error - Empty file.\n");
        exit(0);
    }
    if (c != EOF) { /* Continue to end of file */
        numLines++;
        getLine(line, c, fp);
        if (line[0] == '\n') {
            /* If line is empty, move on to the next line */
            processData(fp, obj);
        }
        tokens = tokenizeLine(tokens, line);
        if (tokens[0] != NULL) {
            if (strcmp(tokens[0], "o") == 0 || strcmp(tokens[0], "O") == 0) { /* Save object file name */
                obj->objName = tokens[1];
            }
            if (strcmp(tokens[0], "g") == 0 || strcmp(tokens[0], "G") == 0) { /* Save group name */
                obj->grpName = tokens[1];
            }
            if (strcmp(tokens[0], "v") == 0 || strcmp(tokens[0], "V") == 0) { /* Parse vertice lines */
                obj = parseVertices(obj, tokens);
            }
            if (strcmp(tokens[0], "vn") == 0 || strcmp(tokens[0], "VN") == 0) { /* Parse the vertice normal lines */
                obj = parseVNormals(obj, tokens);
            }
            if (strcmp(tokens[0], "vt") == 0 || strcmp(tokens[0], "VT") == 0) { /* Parse the vertice texture lines */
                obj = parseVTextures(obj, tokens);
            }
            if (strcmp(tokens[0], "f") == 0 || strcmp(tokens[0], "F") == 0) { /* Parse the faces lines */
                obj->numFaces++;
                obj = parseFaces(obj, tokens);
            }
            processData(fp, obj);
        }
    }
    free(line);
    free(tokens);
}

/* Free malloced space in struct after results are printed */
void freeStruct(Object *obj) {
    /* Free f array */
    int i = 0;
    for (i = 0; i < (1024 * (obj->numFReallocs)); i++) {
        free(obj->f[i]);
    }
    free(obj->f);
    /* Free v array */
    int j = 0;
    for (j = 0; j < (1024 * (obj->numVReallocs)); j++) {
        free(obj->v[j]);
    }
    free(obj->v);
    /* Free vn array */
    i = 0;
    if (obj->numVNReallocs != 0) {
        /* Account for realloced space if it was needed */
        for (i = 0; i < (1024 * (obj->numVNReallocs)); i++) {
            free(obj->vn[i]);
        }
    } else {
        for (i = 0; i < 1024; i++) {
            free(obj->vn[i]);
        }
    }
    free(obj->vn);
    /* Free vt array */
    i = 0;
    if (obj->numVTReallocs != 0) {
        /* Account for realloced space if it was needed */
        for (i = 0; i < (1024 * (obj->numVTReallocs)); i++) {
            free(obj->vt[i]);
        }
    } else {
        for (i = 0; i < 1024; i++) {
            free(obj->vt[i]);
        }
    }
    free(obj->vt);
    free(obj);
}

char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}


int main (int argc, char *argv[]) {
    /* Error check for the file name in the command line */
    if (argc < 2) {
        printf("Error - Input obj file name.\n");
        exit(0);
    } else if (argc > 2) {
        printf("Error - Too many arguments.\n");
        exit(0);
    } else {
        /* Append .obj and .ppm to filename from command line */
        FILE *fpobj;
        char *objfile = malloc(sizeof(char) * 256);
        char *ppmfile = malloc(sizeof(char) * 256);
        objfile[0] = '\0';
        ppmfile[0] = '\0';
        strcat(objfile, argv[1]);
        strcat(ppmfile, argv[1]);
        strcat(objfile, ".obj\0");
        strcat(ppmfile, ".ppm\0");
        fpobj = fopen(objfile, "r");
        /* fpppm = fopen(ppmfile, "r");*/
        if (fpobj == NULL) { /* Error check for broken file */
            /*fprintf(stderr, "Error opening file.\n");*/
            exit(0);
        }

        PyObject *pName;
        PyObject *pModule;
        PyObject *p3Func;
        PyObject *p6Func;
        PyObject *pArgs;
        PyObject *pValue;
        Py_Initialize();
        pName = PyUnicode_DecodeFSDefault("readppm");
        pModule = PyImport_Import(pName);
        Py_DECREF(pName);
        RGB *rgb = malloc(sizeof(RGB));

        if (pModule != NULL) {
            p3Func = PyObject_GetAttrString(pModule, "p3file");
            p6Func = PyObject_GetAttrString(pModule, "p6file");

            if (p3Func && p6Func && PyCallable_Check(p3Func) && PyCallable_Check(p6Func)) {
                /* Set arguments for python function to PPM file name */
                pArgs = PyTuple_New(1);
                pValue = PyUnicode_FromString(ppmfile);
                PyTuple_SetItem(pArgs, 0, pValue);

                /* Get file type from first line of file: P3 or P6 */
                FILE *fp = fopen(ppmfile, "r");
                char *ppmtype = malloc(sizeof(char) * 3);
                if (fp == NULL) {
                    /* Error check openning PPM file */
                    fprintf(stderr, "Failed to open PPM file from command line\n");
                    return 1;
                } 
                ppmtype[0] = getc(fp);
                ppmtype[1] = getc(fp);
                ppmtype[2] = '\0';
                if (strcmp(ppmtype, "P3") == 0) {
                    /* Call p3file function if file begins with "P3" */
                    pValue = PyObject_CallObject(p3Func, pArgs);
                }
                if (strcmp(ppmtype, "P6") == 0) {
                    /* Call p6file function if file begins with "P6" */
                    pValue = PyObject_CallObject(p6Func, pArgs);
                }
                Py_DECREF(pArgs);
                if (pValue != NULL) {
                    /* Retrieve results from Python program & save data to struct */
                    int m = 0;

                    /* Read width, height, and depth numbers from python return value */
                    /*RGB *rgb = malloc(sizeof(RGB));*/
                    PyObject *headernums = PyList_GetItem(pValue, 0);
                    rgb->height = (int)PyLong_AsLong(headernums);
                    headernums = PyList_GetItem(pValue, 1);
                    rgb->width = (int)PyLong_AsLong(headernums);
                    headernums = PyList_GetItem(pValue, 2);
                    rgb->depth = (int)PyLong_AsLong(headernums);

                    /* Malloc space for struct to hold RGB data from PPM file */
                    rgb->r = malloc(sizeof(int *) * (rgb->height));
                    int f = 0;
                    for (f = 0; f < rgb->height; f++) {
                        rgb->r[f] = malloc(sizeof(int) * (rgb->width));
                    }
                    rgb->g = malloc(sizeof(int *) * (rgb->height));
                    f = 0;
                    for (f = 0; f < rgb->height; f++) {
                        rgb->g[f] = malloc(sizeof(int) * (rgb->width));
                    }
                    rgb->b = malloc(sizeof(int *) * (rgb->height));
                    f = 0;
                    for (f = 0; f < rgb->height; f++) {
                        rgb->b[f] = malloc(sizeof(int) * (rgb->width));
                    }

                    /* Save data to struct */
                    int index = 3;
                    f = 0;
                    for (f = 0; f < rgb->height; f++) {
                        for (m = 0; m < rgb->width; m++) {
                            PyObject *num = PyList_GetItem(pValue, index);
                            rgb->r[f][m] = (int)PyLong_AsLong(num);
                            index++;
                            num = PyList_GetItem(pValue, index);
                            rgb->g[f][m] = (int)PyLong_AsLong(num);
                            index++;
                            num = PyList_GetItem(pValue, index);
                            rgb->b[f][m] = (int)PyLong_AsLong(num);
                            index++;
                        }
                    }

                    /* Print output 
                    printf("%s\n", ppmtype);
                    printf("%d %d\n", rgb->width, rgb->height);
                    printf("%d\n", rgb->depth);
                    m = 0;
                    int p = 0;
                    for (m = 0; m < (rgb->height); m++) { 
                        for (p = 0; p < (rgb->width); p++) {
                            printf(" %d %d %d ", rgb->r[m][p], rgb->g[m][p], rgb->b[m][p]);
                        }
                        printf("\n");
                    } */

                    /* Format PPM data for Javascript file */
                    m = 0;
                    int p = 0;
                    index = 0;
                    ppmdata = malloc(sizeof(int) * (rgb->height * rgb->width) * 4);
                    for(m = 0; m < (rgb->height); m++) {
                        for (p = 0; p < (rgb->width); p++) {
                            ppmdata[index] = rgb->r[m][p];
                            /*printf(" %d, ", ppmdata[index]);*/
                            index++;
                            ppmdata[index] = rgb->g[m][p];
                            /*printf("%d, ", ppmdata[index]);*/
                            index++;
                            ppmdata[index] = rgb->b[m][p];
                            /*printf("%d, ", ppmdata[index]);*/
                            index++;
                            ppmdata[index] = 255;
                            /*printf("%d, ", ppmdata[index]);*/
                            index++;
                            /*printf("\n"); */
                        }
                    }

                    /* Free malloced space 
                    m = 0;
                    for (m = 0; m < rgb->height; m++) {
                        free(rgb->r[m]);
                    }
                    free(rgb->r);
                    m = 0;
                    for (m = 0; m < rgb->height; m++) {
                        free(rgb->g[m]);
                    }
                    free(rgb->g);
                    m = 0;
                    for (m = 0; m < rgb->height; m++) {
                        free(rgb->b[m]);
                    }
                    free(rgb->b);
                    free(rgb);
                    free(ppmtype);*/

                    Py_DECREF(pValue);
                }
                fclose(fp);
            }
        } else {
            PyErr_Print();  /* Error check loading python file */
            fprintf(stderr, "Failed to load python file\n");
            return 1;
        }
        if (Py_FinalizeEx() < 0) {
            return 120;
        }


        /* Allocate memory for struct and arrays in struct */
        Object *obj = malloc(sizeof(Object));
        obj->v = malloc(sizeof(float *) * 1024);
        int i = 0;
        for (i = 0; i < 1024; i++) {
            obj->v[i] = malloc(sizeof(float) * 3);
        }
        i = 0;
        obj->vn = malloc(sizeof(float *) * 1024);
        for (i = 0; i < 1024; i++) {
            obj->vn[i] = malloc(sizeof(float) * 3);
        }
        i = 0;
        obj->vt = malloc(sizeof(float *) * 1024);
        for (i = 0; i < 1024; i++) {
            obj->vt[i] = malloc(sizeof(float) * 2);
        }
        i = 0;
        obj->f = malloc(sizeof(Faces *) * 1024);
        for (i = 0; i < 1024; i++) {
            obj->f[i] = malloc(sizeof(Faces) * 4);
        }
        
        /* Initialize counters in struct */
        obj->numFaces = 0;
        obj->numVReallocs = 1;
        obj->numVNReallocs = 1;
        obj->numVTReallocs = 1;
        obj->numFReallocs = 1;
        obj->numVertices = 0;
        obj->numVNormals = 0;
        obj->numVTextures = 0;

        processData(fpobj, obj);

        /* Format OBJ file data */
        vertices = malloc(sizeof(float) * (obj->numFaces * 3)*3);
        vnormals = malloc(sizeof(float) * (obj->numFaces * 3)*3);
        vtextures = malloc(sizeof(float) * (obj->numFaces * 3)*2);
        indices = malloc(sizeof(int) * (obj->numFaces * 3));

        i = 1;
        int j = 0;
        int vindex = 0;
        int normindex = 0;
        int textindex = 0;
        int indiceindex = 0;
        int verticeCount = 0;
        for (i = 1; i <= (obj->numFaces); i++) {
            j = 0;
            /*printf("vertice check\n");*/
            for (j = 0; j < 3; j++) {
                if (obj->f[i][j].v != 0) {
                    vertices[vindex] = obj->v[obj->f[i][j].v][0];
                    vindex++;
                    vertices[vindex] = obj->v[obj->f[i][j].v][1];
                    vindex++;
                    vertices[vindex] = obj->v[obj->f[i][j].v][2];
                    vindex++;
                }
            }
            j = 0;
            for (j = 0; j < 3; j++) {
                if (obj->f[i][j].vt != 0) {
                    vtextures[textindex] = obj->vt[obj->f[i][j].vt][0];
                    textindex++;
                    vtextures[textindex] = obj->vt[obj->f[i][j].vt][1];
                    textindex++;
                }
            }
            j = 0;
            for (j = 0; j < 3; j++) {
                if (obj->f[i][j].vn != 0) {
                    vnormals[normindex] = obj->vn[obj->f[i][j].vn][0];
                    normindex++;
                    vnormals[normindex] = obj->vn[obj->f[i][j].vn][1];
                    normindex++;
                    vnormals[normindex] = obj->vn[obj->f[i][j].vn][2];
                    normindex++;
                }
            }
            j = 0;
            for (j = 0; j < 3; j++) {
                verticeCount++;
            }
        }
        i = 0;
        for (i = 0; i < (obj->numFaces * 3); i++) {
            indices[indiceindex] = i;
            indiceindex++;
        }

        /*printf("%d\n", indiceindex);
        printf("num vertices %d, num normals %d, num textures %d\nnum faces %d", obj->numVertices, obj->numVNormals, obj->numVTextures, obj->numFaces);*/
        
        /* Fill file with formatted data */
        FILE *js;
        js = fopen("arrays.txt", "w+");
        /*int w = 0;*/
        if (js == NULL) {
            printf("Error writing js file.\n");
            exit(0);
        } else {
            fprintf(js, "function getVertexCount() {\n\treturn %d;\n}\n\n", verticeCount);
            fprintf(js, "function getdistance() {\n\treturn -6.0;\n}\n\n");
            
            /* Load vertice data into JS file */
            fprintf(js, "function loadvertices() {\n\treturn [\n");
            i = 1;
            for (i = 1; i <= obj->numFaces; i++) {
                j = 0;
                for (j = 0; j < 3; j++) {
                    if (i < obj->numFaces) {
                        if (obj->f[i][j].v != 0) {
                            fprintf(js, "\t\t%f, %f, %f,\n", obj->v[obj->f[i][j].v][0], obj->v[obj->f[i][j].v][1], obj->v[obj->f[i][j].v][2]);
                        }
                    } else {
                        if (obj->f[i][j].v != 0 && j == 2) {
                            fprintf(js, "\t\t%f, %f, %f\n", obj->v[obj->f[i][j].v][0], obj->v[obj->f[i][j].v][1], obj->v[obj->f[i][j].v][2]);
                        } else if (obj->f[i][j].v != 0 && j < 2) {
                            fprintf(js, "\t\t%f, %f, %f, \n", obj->v[obj->f[i][j].v][0], obj->v[obj->f[i][j].v][1], obj->v[obj->f[i][j].v][2]);
                        }
                    }
                }
            }
            fprintf(js, "\t\t];\n}\n\n");

            /* Load normal data into JS file */
            fprintf(js, "function loadnormals() {\n\treturn [\n");
            i = 1;
            for (i = 1; i <= obj->numFaces; i++) {
                j = 0;
                for (j = 0; j < 3; j++) {
                    if (i < obj->numFaces) {
                        if (obj->f[i][j].vn != 0 ) {
                            fprintf(js, "\t\t%f, %f, %f,\n", obj->vn[obj->f[i][j].vn][0], obj->vn[obj->f[i][j].vn][1], obj->vn[obj->f[i][j].vn][2]);
                        }
                    } else {
                        if (obj->f[i][j].vn != 0 && j == 2) {
                            fprintf(js, "\t\t%f, %f, %f,\n", obj->vn[obj->f[i][j].vn][0], obj->vn[obj->f[i][j].vn][1], obj->vn[obj->f[i][j].vn][2]);
                        } else if (obj->f[i][j].vn != 0 && j < 2) {
                            fprintf(js, "\t\t%f, %f, %f,\n", obj->vn[obj->f[i][j].vn][0], obj->vn[obj->f[i][j].vn][1], obj->vn[obj->f[i][j].vn][2]);
                        }
                    }
                }
            }
            /*fprintf(js, "\t\t-1.000000, 0.000000, 0.000000\n");*/
            fprintf(js, "\t\t];\n}\n\n");

            /* Load texture data into JS file */
            fprintf(js, "function loadtextcoords() {\n\treturn [\n");
            i = 1;
            for (i = 1; i <= obj->numFaces; i++) {
                j = 0;
                for (j = 0; j < 3; j++) {
                    if (i < obj->numFaces) {
                        if (obj->f[i][j].vt != 0) { 
                            fprintf(js, "\t\t%f, %f,\n", obj->vt[obj->f[i][j].vt][0], obj->vt[obj->f[i][j].vt][1]);
                        }
                    } else {
                        if (obj->f[i][j].vt != 0 && j == 2) {
                            fprintf(js, "\t\t%f, %f\n", obj->vt[obj->f[i][j].vt][0], obj->vt[obj->f[i][j].vt][1]);
                        } else if (obj->f[i][j].vt != 0 && j < 2) {
                            fprintf(js, "\t\t%f, %f,\n", obj->vt[obj->f[i][j].vt][0], obj->vt[obj->f[i][j].vt][1]);
                        }
                    }
                
                }
            }
            fprintf(js, "\t\t];\n}\n\n");

            /* Load index data into JS file */
            fprintf(js, "function loadvertexindices() {\n\treturn [\n");
            i = 0;
            fprintf(js, "\t\t");
            for (i = 0; i < (obj->numFaces * 3); i++) {
                fprintf(js, "\t\t%d, \n", i);
            }
            fprintf(js, "\n\t\t];\n}\n\n");

            fprintf(js, "function loadwidth() {\n\treturn %d;\n}\n\n", rgb->width);
            fprintf(js, "function loadheight() {\n\treturn %d;\n}\n\n", rgb->height);

            /* Load rgb data into JS file */
            fprintf(js, "function loadtexture() {\n\treturn( new Uint8Array([\n");
            i = 1;
            for (i = 0; i < (rgb->height); i++) {
                j = 0;
                for (j = 0; j < (rgb->width); j++) {
                    if (i < rgb->height - 1) {
                        fprintf(js, "\t\t%d, %d, %d, 255,\n", rgb->r[i][j], rgb->g[i][j], rgb->b[i][j]);
                    } else {
                        if (j == rgb->height-1) {
                            fprintf(js, "\t\t%d, %d, %d, 255\n", rgb->r[i][j], rgb->g[i][j], rgb->b[i][j]);
                        } else if (j < rgb->height) {
                            fprintf(js, "\t\t%d, %d, %d, 255,\n", rgb->r[i][j], rgb->g[i][j], rgb->b[i][j]);
                        }
                    }

                }
            }
            fprintf(js, "\t\t]) );\n}\n\n");
        }
        printf("finished\n");

        /* Free malloced memory in struct */
        freeStruct(obj);
        free(objfile);
        free(ppmfile);
        fclose(fpobj);
        fclose(js);
    }

    return 0;
}
