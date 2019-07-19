

typedef struct {
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint shaderProgram;
} shaderStruct;

char *loadTextFile(const char *filename);
bool checkErrors(GLuint object);
shaderStruct setupShaders(const char *vertexShader, const char *fragmentShader);
