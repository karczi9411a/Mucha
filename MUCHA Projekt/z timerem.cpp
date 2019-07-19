#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
//#include <GL/GLee.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctime>
#include <malloc.h>
#include <math.h>

#include "Targa.h"

float czas = 1000/60;

int screenWidth = 1200;
int screenHeight = 600;

GLdouble cameraPos[] = {0.0, 0.0, 25.0};

double rotationy = 0.0;
double rotationx = 0.0;
double rotationz = 0.0;

GLfloat lightAmb[] = {1.0, 1.0, 0.0, 1.0};
GLfloat lightDif[] = {0.7, 0.7, 0.7, 1.0};
GLfloat lightPos[] = {100, 100, 100, 1.0};
GLfloat lightSpec[] = {1, 1, 1, 1};

GLuint tex[4];

typedef struct {
    unsigned short vertices;
    unsigned short normals;
    unsigned short indices;
    unsigned short texCoords;

    float verticesArray[1000000];
    float normalsArray[1000000];
    unsigned short indicesArray[1000000];
    float texCoordsArray[1000000];
} _3DSOBJ;

_3DSOBJ *fly;

_3DSOBJ *load3DSObject(const char *filename){
    FILE *f = fopen(filename, "rb");
    _3DSOBJ *object = (_3DSOBJ*)malloc(sizeof(_3DSOBJ));

    int size;
    unsigned short id = 0;
    unsigned int next = 0;

    if(f != NULL){
        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);

        while(ftell(f) < size){
            id = 0;
            next = 0;

            fread(&id, 2, 1, f);
            fread(&next, 4, 1, f);

            switch(id){
                case 0x4D4D:
                    printf("MAIN3DS\n");
                    break;
                case 0x3D3D:
                    printf("EDIT3DS\n");
                    break;
                case 0x4000:
                    printf("EDIT_OBJECT\n");
                    char c;
                    printf("\tObject name: ");
                    do {
                        fread(&c, 1, 1, f);
                        printf("%c", c);
                    } while(c != '\0');
                    printf("\n");
                    break;
                case 0x4100:
                    printf("OBJ_TRIMESH\n");
                    break;
                case 0x4110:
                    printf("TRI_VERTEXL\n");
                    fread(&object->vertices, sizeof(unsigned short), 1, f);
                    printf("\tVertices: %d\n", object->vertices);
                    for(int i = 0; i < object->vertices * 3; i++){
                        fread(&object->verticesArray[i], sizeof(float), 1, f);
                        if((i + 1) % 3 == 0)
                            printf("\t\tVertex %d: (%.3f, %.3f, %.3f)\n", (i + 1) / 3, object->verticesArray[i - 2], object->verticesArray[i - 1], object->verticesArray[i]);
                    }
                    break;
                case 0x4120:
                    printf("TRI_POLYGONL\n");
                    fread(&object->indices, sizeof(unsigned short), 1, f);
                    printf("\tIndices: %d\n", object->indices);
                    GLushort tmp;
                    int i2;
                    i2 = 0;
                    for(int i = 0; i < object->indices * 4; i++){
                        if((i + 1) % 4 == 0) {
                            fread(&tmp, sizeof(unsigned short), 1, f);
                            printf("\t\tPolygon %d: (%d, %d, %d)\n", i2 / 3, object->indicesArray[i2 - 3], object->indicesArray[i2 - 2], object->indicesArray[i2 - 1]);
                        } else {
                            fread(&object->indicesArray[i2], sizeof(unsigned short), 1, f);
                            i2++;
                        }
                    }
                    break;
                case 0x4140:
                    printf("TRI_MAPPINGCOORS\n");
                    fread(&object->texCoords, sizeof(unsigned short), 1, f);
                    printf("\tTex indices: %d\n", object->texCoords);
                    for(int i = 0; i < object->texCoords * 2; i++){
                        fread(&object->texCoordsArray[i], sizeof(float), 1, f);
                        if((i + 1) % 2 == 0)
                            printf("\t\tTex index %d: (%.3f, %.3f)\n", (i + 1) / 2, object->texCoordsArray[i - 1], object->texCoordsArray[i]);
                    }
                    break;
                default:
                    fseek(f, next - 6, SEEK_CUR);
            }
        }

        for(int i = 0; i < object->indices; i++){
                GLfloat p1x = object->verticesArray[object->indicesArray[i * 3] * 3];
                GLfloat p1y = object->verticesArray[object->indicesArray[i * 3] * 3 + 1];
                GLfloat p1z = object->verticesArray[object->indicesArray[i * 3] * 3 + 2];

                GLfloat p2x = object->verticesArray[object->indicesArray[i * 3 + 1] * 3];
                GLfloat p2y = object->verticesArray[object->indicesArray[i * 3 + 1] * 3 + 1];
                GLfloat p2z = object->verticesArray[object->indicesArray[i * 3 + 1] * 3 + 2];

                GLfloat p3x = object->verticesArray[object->indicesArray[i * 3 + 2] * 3];
                GLfloat p3y = object->verticesArray[object->indicesArray[i * 3 + 2] * 3 + 1];
                GLfloat p3z = object->verticesArray[object->indicesArray[i * 3 + 2] * 3 + 2];

                GLfloat nx1 = (p2y - p1y) * (p3z - p1z) - (p2z - p1z) * (p3y - p1y);
                GLfloat ny1 = (p2z - p1z) * (p3x - p1x) - (p2x - p1x) * (p3z - p1z);
                GLfloat nz1 = (p2x - p1x) * (p3y - p1y) - (p2y - p1y) * (p3x - p1x);

                GLfloat l1 = sqrtf(powf(nx1, 2) + powf(ny1, 2) + powf(ny1, 2));

                nx1 /= l1;
                ny1 /= l1;
                nz1 /= l1;

                GLfloat nx2 = (p1y - p2y) * (p3z - p2z) - (p1z - p2z) * (p3y - p2y);
                GLfloat ny2 = (p1z - p2z) * (p3x - p2x) - (p1x - p2x) * (p3z - p2z);
                GLfloat nz2 = (p1x - p2x) * (p3y - p2y) - (p1y - p2y) * (p3x - p2x);

                GLfloat l2 = sqrtf(pow(nx2, 2) + pow(ny2, 2) + pow(ny2, 2));

                nx2 /= l2;
                ny2 /= l2;
                nz2 /= l2;

                GLfloat nx3 = (p2y - p3y) * (p1z - p3z) - (p2z - p3z) * (p1y - p3y);
                GLfloat ny3 = (p2z - p3z) * (p1x - p3x) - (p2x - p3x) * (p1z - p3z);
                GLfloat nz3 = (p2x - p3x) * (p1y - p3y) - (p2y - p3y) * (p1x - p3x);

                GLfloat l3 = sqrtf(pow(nx3, 2) + pow(ny3, 2) + pow(ny3, 2));

                nx3 /= l3;
                ny3 /= l3;
                nz3 /= l3;

                object->normals += 9;
                object->normalsArray[i * 9] = nx1;
                object->normalsArray[i * 9 + 1] = ny1;
                object->normalsArray[i * 9 + 2] = nz1;
                object->normalsArray[i * 9 + 3] = nx2;
                object->normalsArray[i * 9 + 4] = ny2;
                object->normalsArray[i * 9 + 5] = nz2;
                object->normalsArray[i * 9 + 6] = nx3;
                object->normalsArray[i * 9 + 7] = ny3;
                object->normalsArray[i * 9 + 8] = nz3;
            }

        fclose(f);

        return object;
    } else {
        printf("Can't open file\n");
        return NULL;
    }
}

void init(){
    glEnable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);
    glEnable(GL_NORMALIZE);
  //  glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHT0);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);

	glMaterialfv(GL_FRONT, GL_SPECULAR, lightSpec);
	glMateriali(GL_FRONT, GL_SHININESS, 100);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glGenTextures(1, tex);

	glBindTexture(GL_TEXTURE_2D, tex[0]);
	LoadTGATexture("naj.tga");

	fly = load3DSObject("naj.3DS");
	if(fly != NULL) {
        printf("Udalo sie!!\n");
	}
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5, 0.5, 0.5, 1.0); //szary
   // glClearColor(1, 1, 1, 1.0);
    glLoadIdentity ();

    gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

   glPointSize(3);
   // glScalef(7, 7, 7);
   glScalef(1, 1, 1);
  // glScalef(0.05, 0.05, 0.05);
  glRotatef(180, 0, 1, 1);


	glRotatef(rotationy, 0, 1, 0);
	glRotatef(rotationx, 1, 0, 0);
	glRotatef(rotationz, 0, 0, 1);

    glBegin(GL_TRIANGLES);
        for(int i = 0; i < fly->indices * 3; i++){
            glTexCoord2fv(&fly->texCoordsArray[fly->indicesArray[i] * 2]);
            glNormal3fv(&fly->normalsArray[fly->indicesArray[i] * 3]);
            glVertex3fv(&fly->verticesArray[fly->indicesArray[i] * 3]);
        }
    glEnd();

    glFlush();
    glutPostRedisplay();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.5, 50.0);

    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case 27 :
        case 'q':
            free(fly);
            exit(0);
            break;
    }
}

void timerzp(int p){
    rotationz += -2;
    glutPostRedisplay();
    glutTimerFunc(czas, &timerzp, 0);
}

void timerzl(int p){
    rotationz += 2;
    glutPostRedisplay();
    glutTimerFunc(czas, &timerzl, 0);
}


void timerxp(int p){
    rotationx += -2;
    glutPostRedisplay();
    glutTimerFunc(czas, &timerxp, 0);
}

void timerxl(int p){
    rotationx += 2;
    glutPostRedisplay();
    glutTimerFunc(czas, &timerxl, 0);
}

void timeryp(int p){
    rotationy += -2;
    glutPostRedisplay();
    glutTimerFunc(czas, &timeryp, 0);
}

void timeryl(int p){
    rotationy += 2;
    glutPostRedisplay();
    glutTimerFunc(czas, &timeryl, 0);
    display();
}

void timerstop(int p){
    rotationy += 0;
    glutPostRedisplay();
    glutTimerFunc(czas, &timerstop, 0);
}

void trzydx()
{
     for(int i=0;i<360;i++) {
	          rotationx=i+6;
	          display();

     }
}

void trzydy()
{
     for(int i=0;i<360;i++) {
	          rotationy=i+6;
                display();
     }
}

void trzydz()
{
     for(int i=0;i<360;i++) {
	          rotationz=i+6;
                display();
     }
}


void menu(int value) {
    switch(value) {
        case 0:
            exit(0);
            break;
             case 1:
                glutTimerFunc(czas, &timeryl, 0);
        break;
                     case 2:

                    glutTimerFunc(czas, &timeryp, 0);
        break;
        case 3:
                glutTimerFunc(czas, &timerxl, 0);
        break;
                     case 4:

                    glutTimerFunc(czas, &timerxp, 0);
        break;
        case 5:
                glutTimerFunc(czas, &timerzl, 0);
        break;
                     case 6:

                    glutTimerFunc(czas, &timerzp, 0);
        break;
                  case 7:

                    trzydx();
        break;
                  case 8:

                    trzydy();
        break;
                  case 9:

                    trzydz();
        break;
                     case 10:
                         czas = 1;
                        break;

    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow(argv[0]);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(czas, &timerstop, 0);

    glutCreateMenu(menu);
       glutCreateMenu(menu);
    int menuAnim = glutCreateMenu(menu);

    glutAddMenuEntry("Animacja y lewo", 1);
    glutAddMenuEntry("Animacja y w prawo", 2);
    glutAddMenuEntry("Animacja x lewo", 3);
    glutAddMenuEntry("Animacja x w prawo", 4);
    glutAddMenuEntry("Animacja z lewo", 5);
    glutAddMenuEntry("Animacja z w prawo", 6);
    glutAddMenuEntry("Animacja x(360)",7);
    glutAddMenuEntry("Animacja y(360)",8);
    glutAddMenuEntry("Animacja z(360)",9);
    glutAddMenuEntry("Reset Animacji",10);

    int menuEfe = glutCreateMenu(menu);
    glutAddMenuEntry("Efekt 1", 3);
    glutAddMenuEntry("Efekt 2", 4);

    glutCreateMenu(menu);
    glutAddSubMenu("Animacje", menuAnim);
    glutAddSubMenu("Efekty", menuEfe);

    glutAddMenuEntry("Opusc", 0);

    glutAttachMenu (GLUT_RIGHT_BUTTON);

    init();

    glutMainLoop();

    return 0;
}
