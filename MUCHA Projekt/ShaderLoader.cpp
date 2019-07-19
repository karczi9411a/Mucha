#define GLUT_DISABLE_ATEXIT_HACK

#include "GLee.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>

#include "ShaderLoader.h"

char *loadTextFile(const char *filename) {
	FILE *f = fopen(filename, "rb");
	if(!f)	return(NULL);

	unsigned long fSize;
	fseek(f, 0, SEEK_END);
	fSize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *buff = (char*)malloc(fSize + 1);
	if(!buff)	return(NULL);

	fread(buff, 1, fSize, f);
	buff[fSize] = 0;

	return(buff);
}

bool checkErrors(GLuint object) {
	int res, logLen;
	char buff[1024];

	glGetShaderiv(object, GL_COMPILE_STATUS, &res);
	if(res == GL_FALSE) {
		puts("GL_COMPILE_STATUS ERROR!");
		glGetShaderInfoLog(object, 1024, &logLen, buff);
		puts(buff);
	}

	glGetProgramiv(object, GL_LINK_STATUS, &res);
	if(res == GL_FALSE) {
		puts("GL_LINK_STATUS ERROR!");
		glGetProgramInfoLog(object, 1024, &logLen, buff);
		puts(buff);
	}

	return(true);
}

shaderStruct setupShaders(const char *vertexShader, const char *fragmentShader) {
	char *strVert, *strFrag;

	shaderStruct s;

	s.vertexShader = glCreateShader(GL_VERTEX_SHADER);
	s.fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	strVert = loadTextFile(vertexShader);
	if(!strVert) {
		puts("** Blad ladowania pliku *.vert");
		exit(0);
	}

	strFrag = loadTextFile(fragmentShader);
	if(!strFrag) {
		puts("** Blad ladowania pliku *.frag");
		exit(0);
	}

	glShaderSource(s.vertexShader, 1, (const GLchar**)(&strVert), NULL);
	glShaderSource(s.fragmentShader, 1, (const GLchar**)(&strFrag), NULL);

	free(strVert);
	free(strFrag);

	glCompileShader(s.vertexShader);
	glCompileShader(s.fragmentShader);

	s.shaderProgram = glCreateProgram();
	glAttachShader(s.shaderProgram, s.vertexShader);
	glAttachShader(s.shaderProgram, s.fragmentShader);

	glLinkProgram(s.shaderProgram);
	glUseProgram(s.shaderProgram);

	checkErrors(s.vertexShader);
	checkErrors(s.fragmentShader);

	return s;
}
