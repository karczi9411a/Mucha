#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/freeglut.h>

#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

//#include <GL/GLee.h>
#ifdef _WIN32
#include "windows.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctime>
#include <malloc.h>
#include <math.h>

#include "Targa.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;


GLuint shaderVert, shaderFrag;	// Shadery
GLuint shaderProg;	// Program

float a = 0;
//float time = 0;

float losx,losy,losz;

float porkla;
float rotacja, movex, movey, movez;
float tablicaPunktow[3][5000];
int licznikTablicy = 1;
GLfloat zanx=0;
GLfloat zany=0;
GLfloat zanz=0;
// kolor mgły = kolor tła
GLfloat kolor_mgly [4] = {0.25f, 0.25f, 0.25f, 1.0f};

// gęstość mgły w trybie narastanie EXP i EXP2
GLfloat d = 0.1f;
GLbyte  enablemgla = false;
GLbyte  EnableLight = false;
GLbyte  motionblur = false;
GLbyte  punkts = false;
GLbyte  siatka = false;
GLuint listid;
GLint zskrz=0;
GLuint mgla=0;

GLfloat rozmiarskyboxa=60; //45 //50
GLfloat korekcja=0;

int screenWidth = 1200;
int screenHeight = 600;

float k=0.006f;

//int kolo=1;

GLdouble cameraPos[] = {0.0,0, 30.0}; //30 dodaj skyboxa

int i=0;

GLfloat xRotated, yRotated, zRotated=0;

GLuint tex[20];
//z +- 5 granica
//x +- 5 granica
//y +- 5 granica


bool anti_efekt = false;

enum
{
    /* obiekty              */
    SPHERE,               // kula
    EXIT                  // wyjście
};

int obiekt = SPHERE;

/* obliczenie współrzędnych wierzchołków
   Beziera na podstawie wierzchołków
   B-spline (algorytm Boehma) */

/* Funkcja ustawia parametry renderowania i oświetlenie */
void texturing(){
	//glEnable(GL_DEPTH_TEST);    /* Włączenie Z-Bufora */
	glEnable(GL_TEXTURE_2D);    /* Włączenie teksturowania */
    /* Wygenerowanie trzech tzw. "obiektów" dla tekstur */
	glGenTextures(20, tex);
    /* Aktywacja każdej tekstury po kolei i ładowanie z plików TGA */
	glBindTexture(GL_TEXTURE_2D, tex[0]);
	LoadTGATexture("trawa.tga");
	glBindTexture(GL_TEXTURE_2D, tex[1]);
	LoadTGATexture("wing.tga");
	glBindTexture(GL_TEXTURE_2D, tex[2]);
	LoadTGATexture("czarny.tga");
	glBindTexture(GL_TEXTURE_2D, tex[3]);
	LoadTGATexture("zolty.tga");
	glBindTexture(GL_TEXTURE_2D, tex[4]);
	LoadTGATexture("red.tga");
	glBindTexture(GL_TEXTURE_2D, tex[5]);
	LoadTGATexture("pink.tga");
	//do obiektow

	//do skyboxa
	glPushMatrix();
	LoadTGAMipmap("right.tga");
	glBindTexture(GL_TEXTURE_2D, tex[6]);
	LoadTGATexture("up.tga");
	glBindTexture(GL_TEXTURE_2D, tex[7]);
	LoadTGATexture("down.tga");
	glBindTexture(GL_TEXTURE_2D, tex[8]);
	LoadTGATexture("left.tga");
	glBindTexture(GL_TEXTURE_2D, tex[9]);
	LoadTGATexture("right.tga");
	glBindTexture(GL_TEXTURE_2D, tex[10]);
	LoadTGATexture("front.tga");
	glBindTexture(GL_TEXTURE_2D, tex[11]);
	LoadTGATexture("back.tga");

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);// Linear Filtering

	glPopMatrix();
}


void tworzpunkt(float losx,float losy,float losz)
{
     glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, tex[4]);

		  glPointSize(1);
	//	  glLineWidth(10.0f);

    glBegin(GL_POINTS);
    glVertex3f(losx,losy,losz);
    glEnd();

      glPopMatrix();
}

void tworzskyboxa(){
    glDisable(GL_LIGHTING);
	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);

	glPushMatrix();

	//glLoadIdentity();
	//back
	glBindTexture(GL_TEXTURE_2D, tex[11]);
	glBegin (GL_QUADS);
	{
		//back
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-rozmiarskyboxa, -rozmiarskyboxa,  rozmiarskyboxa);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f( rozmiarskyboxa, -rozmiarskyboxa,  rozmiarskyboxa);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f( rozmiarskyboxa,  rozmiarskyboxa,  rozmiarskyboxa);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-rozmiarskyboxa,  rozmiarskyboxa,  rozmiarskyboxa);
	}
	glEnd();

	//front
	glBindTexture(GL_TEXTURE_2D, tex[10]);
	glBegin (GL_QUADS);
	{
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-rozmiarskyboxa, -rozmiarskyboxa, -rozmiarskyboxa);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-rozmiarskyboxa,  rozmiarskyboxa, -rozmiarskyboxa);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f( rozmiarskyboxa,  rozmiarskyboxa, -rozmiarskyboxa);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f( rozmiarskyboxa, -rozmiarskyboxa, -rozmiarskyboxa);
	}
	glEnd();

	//Skybox up
	glBindTexture(GL_TEXTURE_2D, tex[6]);
	glBegin (GL_QUADS);
	{
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-rozmiarskyboxa,  rozmiarskyboxa, -rozmiarskyboxa);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-rozmiarskyboxa,  rozmiarskyboxa,  rozmiarskyboxa);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f( rozmiarskyboxa,  rozmiarskyboxa,  rozmiarskyboxa);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f( rozmiarskyboxa,  rozmiarskyboxa, -rozmiarskyboxa);
	}
	glEnd();

	//Skybox down
	glBindTexture(GL_TEXTURE_2D, tex[7]);

	glPushMatrix();
	glRotatef(180.0f, 0, 1, 0); // trick dla tekstury by byla liniowa
	glBegin (GL_QUADS);
	{
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-rozmiarskyboxa, -rozmiarskyboxa, -rozmiarskyboxa);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f( rozmiarskyboxa, -rozmiarskyboxa, -rozmiarskyboxa);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f( rozmiarskyboxa, -rozmiarskyboxa,  rozmiarskyboxa);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-rozmiarskyboxa, -rozmiarskyboxa,  rozmiarskyboxa);
	}
	glEnd();
	glPopMatrix();

	//Skybox right face
	glBindTexture(GL_TEXTURE_2D, tex[9]);
	glBegin (GL_QUADS);
	{
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f( rozmiarskyboxa-korekcja, -rozmiarskyboxa+korekcja, -rozmiarskyboxa+korekcja);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f( rozmiarskyboxa-korekcja,  rozmiarskyboxa-korekcja, -rozmiarskyboxa+korekcja);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f( rozmiarskyboxa-korekcja,  rozmiarskyboxa-korekcja,  rozmiarskyboxa-korekcja);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f( rozmiarskyboxa-korekcja, -rozmiarskyboxa+korekcja,  rozmiarskyboxa-korekcja);
	}
	glEnd();

	//Skybox left face
	glBindTexture(GL_TEXTURE_2D, tex[8]);
	glBegin (GL_QUADS);
	{
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-rozmiarskyboxa+korekcja, -rozmiarskyboxa+korekcja, -rozmiarskyboxa+korekcja);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-rozmiarskyboxa+korekcja, -rozmiarskyboxa+korekcja,  rozmiarskyboxa-korekcja);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-rozmiarskyboxa+korekcja,  rozmiarskyboxa-korekcja,  rozmiarskyboxa-korekcja);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-rozmiarskyboxa+korekcja,  rozmiarskyboxa-korekcja, -rozmiarskyboxa+korekcja);
	}

	glEnd();

	glPopMatrix();

	glEnable(GL_LIGHTING);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glDisable(GL_TEXTURE_2D);
}

void initLight(){
    GLfloat lightAmb[] = {0.3, 0.3, 0.3, 1.0};
    GLfloat lightDif[] = {0.7, 0.7, 0.7, 1.0};
    GLfloat lightPos[] = {0, 0, 0, 1.0};
    GLfloat lightSpec[] = {1, 1, 1, 1};

    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
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
}

void rozwalswiatlo(){
    glDisable(GL_NORMALIZE);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_AUTO_NORMAL);
    glDisable(GL_NORMALIZE);
    glDisable(GL_LIGHT0);
}

void mucha(){
    GLUquadricObj *obiekt;
    obiekt = gluNewQuadric();

    glPushMatrix(); //cala mucha
    glBindTexture(GL_TEXTURE_2D, tex[2]); //czarna
  //  glScalef(0.5,0.5,0.5);
    //glScalef(4,4,4);
    glPushMatrix(); //nasza mucha tyl
    //glColor3f(0,0,0);
  //  glTranslatef(0.3,0.3, 0);
    gluSphere(obiekt, 0.05, 20, 10);
    //glutSolidSphere(0.2,20,10);
    glPopMatrix();


    glPopMatrix();
}

void skrzydlap(){
    GLUquadricObj *obiekt;
    obiekt = gluNewQuadric();
    glPushMatrix(); //skrzydlo przednie
    //glScalef(4,4,4);
    glScalef(0.5,0.5,0.5);
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    //glColor3f(1,1,1);
    glTranslatef(-0.3,0.5, 0.2);
    glRotatef(90,1,0,0);
    glRotatef(-35,0,0,1);
    glScalef(2, 1, 0);
    gluSphere(obiekt,0.2,20,10);
    glPopMatrix();
}

void skrzydlat(){
    /*
    GLUquadricObj *obiekt;
    obiekt = gluNewQuadric();

    glPushMatrix(); //skrzydlo przednie
    //glScalef(4,4,4);
    glScalef(0.5,0.5,0.5);
    //glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    glTranslatef(0.3,0.5, -0.2);
    glRotatef(90,1,0,0);
    glRotatef(35,0,0,1);
    glScalef(2, 1, 0);
    gluSphere(obiekt,0.2,20,10);
    glPopMatrix();
    */
     GLUquadricObj *obiekt;
    obiekt = gluNewQuadric();
    glPushMatrix(); //skrzydlo przednie
    //glScalef(4,4,4);
    glScalef(0.5,0.5,0.5);
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    //glColor3f(1,1,1);
    glTranslatef(0.3,0.5, 0.2);
    glRotatef(90,1,0,0);
    glRotatef(90,0,0,1);
    glRotatef(-35,0,0,1);
    glScalef(2, 1, 0);
    gluSphere(obiekt,0.2,20,10);
    glPopMatrix();
}

void zoltakula(){
    GLUquadricObj *obiekt;
    obiekt = gluNewQuadric();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, tex[3]);
    glTranslatef(-4, 2, 0);
    //glColor3f(1,1,0);
    gluSphere(obiekt,0.1,20,10);  //nasza zolta kula
    glPopMatrix();
}

void trawa(){
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glTranslatef(0, -1.5, 0);
    glRotatef(90,1,0,0);
    glBegin(GL_QUADS);  //plansza
    //glColor3f(0,1,0);
    glTexCoord2f(0, 0);glVertex3f( 5,  -5, 0.0f); //prawy g wierzcholek //prawy dol
    //glColor3f(0,0,0);
    glTexCoord2f(1, 0); glVertex3f( -5,  -5, 0.0f);//prawy d //prawa gora
    //glColor3f(0,1,0);
    glTexCoord2f(1, 1);glVertex3f(-5, 5, 0.0f); //lewy d   //lewa gora
    //glColor3f(0,1,0);
    glTexCoord2f(0, 1); glVertex3f( 5, 5.0,0.0f);  //lewy g   //lewy dol
    glEnd();
    glPopMatrix();
}

void losowanie()
{
    for(int i=1; i<5000; i++){
    losx = ((rand() % 10) -5);
    losx = losx /100;
    //movex += losx;
     losy = ((rand() % 10) -5);
    losy = losy /500;
    //movey += losy;
     losz = ((rand() % 10) -5);
    losz = losz /100;
    //movez += losz;


    tablicaPunktow[0][i] = tablicaPunktow[0][i-1]+losx;
    tablicaPunktow[1][i] = tablicaPunktow[1][i-1]+losy;
    tablicaPunktow[2][i] = tablicaPunktow[2][i-1]+losz;

    tworzpunkt(tablicaPunktow[0][i], tablicaPunktow[1][i], tablicaPunktow[2][i]);

   // printf("losx %f",losx);

//printf("\n");

    }
  //  printf("done");


   // tworzpunkt(losx, losy, losz);

   // glCallList( listid + 6 );

   //printf("losx %f",losx);
   //printf("\n");
   //system("cls");
}

void rysuj(){
    for(int i=1; i<5000; i++){
        tworzpunkt(tablicaPunktow[0][i], tablicaPunktow[1][i], tablicaPunktow[2][i]);
    }
}

void ruch(){
        movex = tablicaPunktow[0][licznikTablicy];
        movey = tablicaPunktow[1][licznikTablicy];
        movez = tablicaPunktow[2][licznikTablicy];
        licznikTablicy++;

}

void DisplayLists(){
    texturing();
    // określenie zarodka dla  ciągu liczb pseudolosowych
    srand( time( NULL ) );
    // wygenerowanie identyfikatorów trzech list wyświetlania
    listid = glGenLists( 8 );

    glNewList( listid + 0, GL_COMPILE );
 //   losowanie();
    mucha();
    glEndList();

    glNewList( listid + 1, GL_COMPILE );
    skrzydlap();
    glEndList();

    glNewList( listid + 2, GL_COMPILE );
    skrzydlat();
    glEndList();

    glNewList( listid + 3, GL_COMPILE );
    zoltakula();
    glEndList();

    glNewList( listid + 4, GL_COMPILE );
    trawa();
    glEndList();

    glNewList( listid + 5, GL_COMPILE );
    tworzskyboxa();
    glEndList();
}

void warunkimgly(){
    if (mgla==1){
        glClearColor(0.25, 0.25, 0.25, 1.0);
        glEnable (GL_FOG);
        glFogi (GL_FOG_MODE, GL_EXP);
        glFogf (GL_FOG_START, -20.0f);
        glFogf (GL_FOG_END, 80.0f);
        glFogf (GL_FOG_DENSITY, d);
        glFogfv (GL_FOG_COLOR, kolor_mgly);
    }

    if (mgla==2){
        glClearColor(0.25, 0.25, 0.25, 1.0);
        glEnable (GL_FOG);
        glFogi (GL_FOG_MODE, GL_EXP2);
        glFogf (GL_FOG_START, -20.0f);
        glFogf (GL_FOG_END, 80.0f);
        glFogf (GL_FOG_DENSITY, d);
        glFogfv (GL_FOG_COLOR, kolor_mgly);
    }

    if (mgla==3){
        glClearColor(0.25, 0.25, 0.25, 1.0);
        glEnable (GL_FOG);
        glFogi (GL_FOG_MODE, GL_LINEAR);
        glFogf (GL_FOG_START, -20.0f);
        glFogf (GL_FOG_END, 80.0f);
        glFogf (GL_FOG_DENSITY, d);
        glFogfv (GL_FOG_COLOR, kolor_mgly);
    }
}

void sleep(){
  clock_t target = clock() + 500;
  while (clock() < target) ;
}

void muszka(){

    glPushMatrix();
    glTranslatef(movex,0,0);
    glTranslatef(0,movey,0);
    glTranslatef(0,0,movez);

    glPushMatrix();

    glCallList( listid + 0 ); //mucha
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,-0.20,0);
    if(zskrz==30)zskrz=-30;
    glRotatef(zskrz,0,0,1);
    glCallList( listid + 1 ); //skrzydlap
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,-0.20,0);
    if(zskrz==30)zskrz=-30;
    glRotatef(zskrz,0,0,-1);
    glCallList( listid + 2 ); //skrzydlat
    glPopMatrix();

    glPopMatrix();
    rysuj();
    //sleep();
}

void listyikrzywe(){
    glPushMatrix();
    // lot muchy
    glPushMatrix();
    muszka();
    glPopMatrix();

    glCallList( listid + 3 ); //zoltakula
    //glCallList( listid + 4 ); //trawa
    glCallList( listid + 5);

//zrob punkty

    if(motionblur==true){
        glAccum(GL_MULT, 0.7);
        glAccum(GL_ACCUM, 0.3);
        glAccum(GL_RETURN, 1.0);
    }

    //to juz nie bedzie blurowac
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //if(kolo==1)  glClearColor(0.5, 0.5, 1, 1.0); //niebieski
    //if(kolo==0)  glClearColor(0, 0, 0, 1.0); //czarny
    if(enablemgla==true) glClearColor(0.25, 0.25, 0.25, 1.0);
    //glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glPushMatrix();
    glEnable (GL_DEPTH_TEST);
    glMatrixMode (GL_MODELVIEW);
    //glRotatef(zRotated,0.0,0.0,1.0);
    gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    if(EnableLight==true) initLight();
    if(EnableLight==false) rozwalswiatlo();
    glTranslatef(0, -5, 0);
    glScalef(3, 3, 3);
    glRotatef(porkla,0,1,0);  ///skyboxem

    if (anti_efekt==false){
    // Figury();
    warunkimgly();
    listyikrzywe();

    }

   if (anti_efekt==true){
        // przesunięcie w lewo
        glTranslatef (-k, 0.0f, 0.0f);
        // Figury();
        warunkimgly();
        listyikrzywe();
        glAccum (GL_LOAD, 0.1f);

        // przesunięcie w prawo
        glTranslatef (2*k, 0.0f, 0.0f);
        // Figury();
        warunkimgly();
        listyikrzywe();
        glAccum (GL_ACCUM, 0.2f);

        // przesunięcie w górę
        glTranslatef (-k, k, 0.0f);
        // Figury();
        warunkimgly();

        listyikrzywe();
        glAccum (GL_ACCUM, 0.3f);

        // przesunięcie w dół
        glTranslatef (0.0f, -2*k, 0.0f);
        // Figury();
        warunkimgly();
        listyikrzywe();
        glAccum (GL_ACCUM, 0.4f);
        // wyświetlenie zawartości bufora
        glAccum (GL_RETURN, 1.0f);
    }
    glPopMatrix();
    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();
}

void reshape(int w, int h){
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.5, 350.0);

    glMatrixMode(GL_MODELVIEW);
    display();
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case 27 :
            exit(0);
            break;
        case 'q':
           // losowanie();
            porkla-=1;
            break;
        case 'e':
            porkla+=1;
            break;
    reshape (glutGet (GLUT_WINDOW_WIDTH),glutGet (GLUT_WINDOW_HEIGHT));
    }
}

void idle(){//animacja
    ruch();
    xRotated += 1;
    yRotated += 1;
    zRotated += 1;
    zskrz+=5;
}

void idlem(){//zatrzymanie animacji

    losx+=0;
    losy+=0;
    losz+=0;
    zanx+=0;
    zany+=0;
    zanz+=0;
    xRotated+=0;
    yRotated+=0;
    zRotated+=0;
    zskrz+=0;
}

void reset(){
     licznikTablicy=0;
    zRotated = 0.0;
    zskrz=0;
    zanx=0;
    zany=0;
    zanz=0;
    xRotated = 0.0;
    losx=0;
    losy=0;
    losz=0;
    movex=0;
    movey=0;
    movez=0;
}

void animacjskrz(){
    xRotated += 1;
    zanx+=1;
    zany+=0;
    zanz+=0.1;
    //zanx+=0.1;
    zskrz+=5;
}

void menu(int value){
    switch(value) {
        case 0:
            exit(0);
            break;
        case 1:
             //   losowanie();
            //ruch();
            glutIdleFunc(idle); //glutidlem dziala caly czas
            break;
        case 2:
            glutIdleFunc(idlem);
            break;
        case 3:
            glutIdleFunc(idlem);
            glutIdleFunc(reset);
            break;
        case 4:
           EnableLight = true;
            break;
        case 5:
            EnableLight = false;
            break;
        case 6:
            glEnable(GL_TEXTURE_2D);
            break;
        case 7:
            glDisable(GL_TEXTURE_2D);
            break;
        case 8:

            break;
        case 9:

            break;
        case 10:
        //    kolo=1;
            break;
        case 11:
          //  kolo=0;
            break;
        case 12:
            break;
        case 13:
            anti_efekt=true;
            motionblur=false;
            break;
        case 14:
            anti_efekt=false;
            break;
        case 15:
            glEnable (GL_FOG);
            enablemgla=true;
            break;
        case 16:
            glDisable (GL_FOG);
            mgla=0;
            enablemgla=false;
            break;
        case 17:
            mgla=1;
            break;
        case 18:
            mgla=2;
            break;
        case 19:
            mgla=3;
            break;
        case 20:
            d=0.025;
            break;
        case 21:
            d=0.050;
            break;
        case 22:
            d=0.075;
            break;
        case 23:
            d=0.1;
            break;
        case 24:
            glutIdleFunc(animacjskrz);
            break;
        case 25:

            break;
        case 26:

            break;
        case 27:
             motionblur = true;
             anti_efekt=false;
            break;
        case 28:
             motionblur = false;
            break;
        case 29:

            break;
        case 30:

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

    losowanie();

    int menuAnim = glutCreateMenu(menu);
    glutAddMenuEntry("Wlacz obrot nieskonczony o os X", 1);
    glutAddMenuEntry("Zatrzymaj obrot i animacje skrzydel", 2);
    glutAddMenuEntry("Reset obrotu i powrot to pozycji startowej", 3);
    glutAddMenuEntry("Animacja skrzydel 1", 24);

    int menuSwi = glutCreateMenu(menu);
    glutAddMenuEntry("Wlacz Oswietlenie", 4);
    glutAddMenuEntry("Wylacz Oswietlenie", 5);

    int menuText = glutCreateMenu(menu);
    glutAddMenuEntry("Wlacz Teksturowanie", 6);
    glutAddMenuEntry("Wylacz Teksturowanie", 7);
/*
    int menuKrzywa = glutCreateMenu(menu);
    glutAddMenuEntry("Wlacz wyswietlenie Krzywej", 8);
    glutAddMenuEntry("Wylacz wyswietlenie Krzywej", 9);

    int menuTlo = glutCreateMenu(menu);
    glutAddMenuEntry("Blekitne", 10);
    glutAddMenuEntry("Czarne", 11);
*/
    int menuAnti = glutCreateMenu(menu);
    glutAddMenuEntry("Wlacz", 13);
    glutAddMenuEntry("Wylacz", 14);

    int menuMgl = glutCreateMenu(menu);
    glutAddMenuEntry("Wlacz", 15);
    glutAddMenuEntry("Wylacz", 16);
    glutAddMenuEntry("Mgla z narastaniem EXP",17);
    glutAddMenuEntry("Mgla z narastaniem EXP2",18);
    glutAddMenuEntry("Mgla z narastaniem liniowym",19);

    int menuMglges = glutCreateMenu(menu);
    glutAddMenuEntry("25%",20);
    glutAddMenuEntry("50%",21);
    glutAddMenuEntry("75%",22);
    glutAddMenuEntry("100%",23);

    int menumotionblur = glutCreateMenu(menu);
    glutAddMenuEntry("Wlacz motion blur",27);
    glutAddMenuEntry("Wylacz motion blur",28);

    glutCreateMenu(menu);
    glutAddSubMenu("Animacje", menuAnim);
    glutAddSubMenu("Rozmycie w ruchu", menumotionblur);
    glutAddSubMenu("Swiatlo", menuSwi);
    glutAddSubMenu("Tekstury", menuText);
   // glutAddSubMenu("Krzywa", menuKrzywa);
    //glutAddSubMenu("Tlo", menuTlo);
    glutAddSubMenu("Wygladzanie krawedzi", menuAnti);
    glutAddSubMenu("Mgla", menuMgl);
    glutAddSubMenu("Gestosc Mgly(EXP1/2)", menuMglges);
    glutAddMenuEntry("Opusc", 0);

    glutAttachMenu (GLUT_RIGHT_BUTTON);
    xRotated = yRotated = zRotated = 0.0;

    DisplayLists();

    glutMainLoop();

    return 0;
}
