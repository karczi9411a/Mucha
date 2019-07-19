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
#include <GL/freeglut.h>
#include "Targa.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>


float czas = 1000/60;

int screenWidth = 1200;
int screenHeight = 600;

GLdouble cameraPos[] = {0.0, 0.0, 25.0};
//GLfloat obrot = 0;

GLfloat xRotated, yRotated, zRotated=0;
GLdouble radius=100;
GLfloat qaBlack[] = {0.0, 0.0, 0.0, 1.0}; //Black Color
GLfloat qaGreen[] = {0.0, 1.0, 0.0, 1.0}; //Green Color
GLfloat qaWhite[] = {1.0, 1.0, 1.0, 1.0}; //White Color
GLfloat qaRed[] = {1.0, 0.0, 0.0, 1.0}; //White Color

    // Set lighting intensity and color
GLfloat qaAmbientLight[]    = {0.3, 0.3, 0.3, 1.0};
GLfloat qaDiffuseLight[]    = {0.7, 0.7, 0.7, 1.0};
GLfloat qaSpecularLight[]    = {0.0, 0.0, 0.0, 0.0};
GLfloat emitLight[] = {0.9, 0.9, 0.9, 0.01};
GLfloat Noemit[] = {0.0, 0.0, 0.0, 1.0};
    // Light source position
GLfloat qaLightPosition[]    = {-1,1.5,0,1};
//GLuint tex[4];

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

	//glGenTextures(1, tex);

	//glBindTexture(GL_TEXTURE_2D, tex[0]);
//	LoadTGATexture("naj.tga");

//	fly = load3DSObject("naj.3DS"); //z skrzydlami statycznymi
fly = load3DSObject("bezskrzydel.3DS");
	if(fly != NULL) {
        printf("Udalo sie!!\n");
	}
}

void initLighting()
{

    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

     // Set lighting intensity and color
       glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, qaDiffuseLight);
     glLightfv(GL_LIGHT0, GL_SPECULAR, qaSpecularLight);

    // Set the light position


}

void disablelight()
{
glDisable(GL_LIGHTING);
glDisable(GL_LIGHT0);
}

void display() {
glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5, 0.5, 1, 1.0);
    glEnable(GL_COLOR_MATERIAL);
  //  glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glMatrixMode (GL_MODELVIEW);
 //   glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glPushMatrix();


    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, qaGreen);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, qaGreen);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, qaWhite);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10);
  //  glRotatef(zRotated,0.0,0.0,1.0);

    gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
 glLightfv(GL_LIGHT0, GL_POSITION, qaLightPosition);
   //  glPointSize(3);
   glTranslatef(0, -5, 0);
    glScalef(3, 3, 3);
    glRotatef(zRotated,0,1,0);
  // glScalef(0.05, 0.05, 0.05);
 glPushMatrix();
  glPushMatrix();
  glColor3f(1,1,1);
 glTranslatef(0.6,0.2, 0.2);
 glScalef(2,1,2);
 glRotatef(-13,0,0,1);
 glRotatef(13,0,1,0);
  glBegin(GL_QUADS); //skrzydlo z przodu
   glColor4f(1,1,1,0.4);
      glVertex3f( 0.1,  -0.01, 0.0f); //prawy g wierzcholek //prawy dol
      glVertex3f( -0.3,  -0.2, 0.0f);//prawy d //prawa gora
      glVertex3f(-0.3, 0.2, 0.0f); //lewy d   //lewa gora
      glVertex3f( 0.3, 0.01,0.0f);  //lewy g   //lewy dol
glEnd();
 glPopMatrix();

  glPushMatrix();
  glColor3f(1,1,1);
 glTranslatef(0.6,0.2, -0.2);
 glScalef(2,1,2);
 glRotatef(-13,0,0,1);
 glRotatef(-13,0,1,0);
  glBegin(GL_QUADS); //skrzydlo z tylu
   glColor4f(1,1,1,0.4);
      glVertex3f( 0.3,  -0.01, 0.0f); //prawy g wierzcholek //prawy dol
      glVertex3f( -0.3,  -0.2, 0.0f);//prawy d //prawa gora
      glVertex3f(-0.3, 0.2, 0.0f); //lewy d   //lewa gora
      glVertex3f( 0.3, 0.01,0.0f);  //lewy g   //lewy dol
glEnd();
 glPopMatrix();

    glBegin(GL_TRIANGLES); //odtwarza model 3ds
     glColor3f(0,0,0);
        for(int i = 0; i < fly->indices * 3; i++){
            glTexCoord2fv(&fly->texCoordsArray[fly->indicesArray[i] * 2]);
            glNormal3fv(&fly->normalsArray[fly->indicesArray[i] * 3]);
            glVertex3fv(&fly->verticesArray[fly->indicesArray[i] * 3]);

        }
    glEnd();
 glPopMatrix();
    glPushMatrix();
    glTranslatef(-3, 2, 0);
    glColor3f(1,1,0);
    glutSolidSphere(0.3,20,10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -0.5, 0);
  glRotatef(90,1,0,0);
    glBegin(GL_QUADS);
    glColor3f(0,1,0);
      glVertex3f( 5,  -5, 0.0f); //prawy g wierzcholek //prawy dol
      glColor3f(0,0,0);
      glVertex3f( -5,  -5, 0.0f);//prawy d //prawa gora
      glColor3f(0,1,0);
      glVertex3f(-5, 5, 0.0f); //lewy d   //lewa gora
      glColor3f(0,1,0);
      glVertex3f( 5, 5.0,0.0f);  //lewy g   //lewy dol
glEnd();
    glPopMatrix();


glPopMatrix();
    glFlush();
    glutSwapBuffers();
glutPostRedisplay();

}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.5, 50.0);

    glMatrixMode(GL_MODELVIEW);
    display();
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case 27 :
        case 'q':
            free(fly);
            exit(0);
            break;
   reshape (glutGet (GLUT_WINDOW_WIDTH),glutGet (GLUT_WINDOW_HEIGHT));
    }
}

void idle()
{
    // zmiana k¹ta obrotu wzglêdem osi X i Y
   // obrot+= 0;
    zRotated += 1;
}


void idlem()
{
    // zmiana k¹ta obrotu wzglêdem osi X i Y
   // obrot+= 0;
    zRotated += 0.0;
}

void reset()
{
    // zmiana k¹ta obrotu wzglêdem osi X i Y
   // obrot= 0;
   zRotated = 0.0;
}


void menu(int value) {
    switch(value) {
        case 0:
            exit(0);
            break;
        case 1:
            glutIdleFunc(idle);
            break;
        case 2:
            glutIdleFunc(idlem);
            break;
        case 3:
            glutIdleFunc(idlem);
           glutIdleFunc(reset);
            break;
        case 4:
            initLighting();
            break;
        case 5:
            disablelight();
            break;

    }
    reshape (glutGet (GLUT_WINDOW_WIDTH),glutGet (GLUT_WINDOW_HEIGHT));
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(5,5);
    glutCreateWindow(argv[0]);

    glMatrixMode(GL_PROJECTION);
     glMatrixMode(GL_MODELVIEW);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutCreateMenu(menu);

    int menuAnim = glutCreateMenu(menu);
     glutAddMenuEntry("Wlacz animacje", 1);
     glutAddMenuEntry("Zatrzymaj animacje", 2);
     glutAddMenuEntry("Reset animacji", 3);

    int menuEfe = glutCreateMenu(menu);
    glutAddMenuEntry("Wlacz Oswietlenie", 4);
    glutAddMenuEntry("Wylacz Oswietlenie", 5);

    glutCreateMenu(menu);
    glutAddSubMenu("Animacje", menuAnim);
    glutAddSubMenu("Efekty", menuEfe);
    glutAddMenuEntry("Opusc", 0);

    glutAttachMenu (GLUT_RIGHT_BUTTON);

    xRotated = yRotated = zRotated = 0.0;
    init();

    glutMainLoop();

    return 0;
}
