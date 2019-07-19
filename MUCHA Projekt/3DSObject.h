typedef struct {
    GLuint vertices;
    GLuint indices;
    GLuint texCoords;

    GLfloat *verticesArray;
    GLuint *indicesArray;
    GLfloat *texCoordsArray;
} _3DSOBJ;

_3DSOBJ *Load3DSObject(const char *filename);
