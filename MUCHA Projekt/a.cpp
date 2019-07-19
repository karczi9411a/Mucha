#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <sys/stat.h>





#define MAX_VERTICES 8000
#define MAX_POLYGONS 8000


typedef struct{
    float x,y,z;
}vertex_type;


typedef struct{
    int a,b,c;
}polygon_type;


typedef struct{
    float u,v;
}mapcoord_type;


typedef struct {
	char name[20];

	int vertices_qty;
    int polygons_qty;

    vertex_type vertex[MAX_VERTICES];
    polygon_type polygon[MAX_POLYGONS];
    mapcoord_type mapcoord[MAX_VERTICES];

} obj_type, *obj_type_ptr;




typedef struct {
	GLuint bpp;
	GLuint width;
	GLuint height;
} TARGAINFO;



GLubyte *LoadTGAImage(char *filename, TARGAINFO *info)
{
	GLubyte	TGAHeader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
	GLubyte	TGACompare[12];
	GLubyte	Header[6];
	GLubyte *Bits = NULL;

	FILE *plik = fopen(filename, "rb");
	if(plik)
	{
		fread(TGACompare, 1, sizeof(TGACompare), plik);
		if(memcmp(TGAHeader, TGACompare, sizeof(TGAHeader)) == 0)
		{
			fread(Header, 1, sizeof(Header), plik);


			info->width  = Header[1] * 256 + Header[0];
			info->height = Header[3] * 256 + Header[2];
			info->bpp = Header[4];

			if(info->width>0 && info->height>0 && (info->bpp==24 || info->bpp==32))			{
				long ImageSize = info->height * info->width * info->bpp / 8;
				Bits = (GLubyte*)malloc(ImageSize);
				if(Bits)
				{
					fread(Bits, 1, ImageSize, plik);
					int i;
					GLubyte tmp;
					for(i=0;i < ImageSize;i += info->bpp / 8)
					{
						tmp = Bits[i];
						Bits[i] = Bits[i+2];
						Bits[i+2] = tmp;
					}
				}
			}
    	}

		fclose(plik);
	}

	return(Bits);
}

bool LoadTGATexture(char *filename)
{
	TARGAINFO info;	// Dane o bitmapie
	GLubyte *bits;	// Dane o pikselach

	// 3adowanie pliku
	bits = LoadTGAImage(filename, &info);	// Próba wczytania tekstury
	if(bits == NULL)	return(FALSE);	// ERROR

	// Ustawienie parametrów tekstury
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	if(info.bpp == 24)	// Bitmapa z danymi RGB
		glTexImage2D(GL_TEXTURE_2D, 0, 3, info.width, info.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bits);
	else	// Bitmapa z danymi RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, 4, info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);

	free(bits);
	return(TRUE);
}

bool LoadTGAMipmap(char *filename)
{
	TARGAINFO info;	// Dane o bitmapie
	GLubyte *bits;	// Dane o pikselach

	// 3adowanie pliku
	bits = LoadTGAImage(filename, &info);	// Próba wczytania tekstury
	if(bits == NULL)	return(FALSE);	// ERROR

	// Ustawienie parametrów tekstury
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	if(info.bpp == 24)	// Bitmapa z danymi RGB
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, info.width, info.height, GL_RGB, GL_UNSIGNED_BYTE, bits);
	else	// Bitmapa z danymi RGBA
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, info.width, info.height, GL_RGBA, GL_UNSIGNED_BYTE, bits);

	free(bits);
	return(TRUE);
}


int num_texture=-1;


int screen_width=640;
int screen_height=480;


double rotation_x=0, rotation_x_increment=0.1;
double rotation_y=0, rotation_y_increment=0.05;
double rotation_z=0, rotation_z_increment=0.03;

int filling=1;

obj_type object;



long filelength(int f)
{
    struct stat buf;

    fstat(f, &buf);

    return(buf.st_size);
}



char Load3DS (obj_type_ptr p_object, char *p_filename)
{
	int i;
	FILE *l_file;
	unsigned short l_chunk_id;
	unsigned int l_chunk_lenght;
	unsigned char l_char;
	unsigned short l_qty;

	unsigned short l_face_flags;

	if ((l_file=fopen (p_filename, "rb"))== NULL) return 0;

	while (ftell (l_file) < filelength (fileno (l_file)))
	{

		fread (&l_chunk_id, 2, 1, l_file);
		fread (&l_chunk_lenght, 4, 1, l_file);

		switch (l_chunk_id)
        {
			case 0x4d4d:
			break;

			case 0x3d3d:
			break;

			case 0x4000:
				i=0;
				do
				{
					fread (&l_char, 1, 1, l_file);
                    p_object->name[i]=l_char;
					i++;
                }while(l_char != '\0' && i<20);
			break;

			case 0x4100:
			break;


			case 0x4110:
				fread (&l_qty, sizeof (unsigned short), 1, l_file);
                p_object->vertices_qty = l_qty;

                for (i=0; i<l_qty; i++)
                {
					fread (&p_object->vertex[i].x, sizeof(float), 1, l_file);
                    fread (&p_object->vertex[i].y, sizeof(float), 1, l_file);
					fread (&p_object->vertex[i].z, sizeof(float), 1, l_file);
				}
				break;

			case 0x4120:
				fread (&l_qty, sizeof (unsigned short), 1, l_file);
                p_object->polygons_qty = l_qty;

                for (i=0; i<l_qty; i++)
                {
					fread (&p_object->polygon[i].a, sizeof (unsigned short), 1, l_file);
					fread (&p_object->polygon[i].b, sizeof (unsigned short), 1, l_file);
					fread (&p_object->polygon[i].c, sizeof (unsigned short), 1, l_file);
					fread (&l_face_flags, sizeof (unsigned short), 1, l_file);
				}
                break;


			case 0x4140:
				fread (&l_qty, sizeof (unsigned short), 1, l_file);
				for (i=0; i<l_qty; i++)
				{
					fread (&p_object->mapcoord[i].u, sizeof (float), 1, l_file);
                    fread (&p_object->mapcoord[i].v, sizeof (float), 1, l_file);
				}
                break;

			default:
				 fseek(l_file, l_chunk_lenght-6, SEEK_CUR);
        }
	}
	fclose (l_file);
	return (1);
}



GLuint tex[1];



void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

    glViewport(0,0,screen_width,screen_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,(GLfloat)screen_width/(GLfloat)screen_height,10.0f,10000.0f);

    glEnable(GL_DEPTH_TEST);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_TEXTURE_2D);

    Load3DS (&object,"obiekt.3DS");

    glGenTextures(2, tex);
	glBindTexture(GL_TEXTURE_2D, tex[0]);

    if (LoadTGATexture("statek2.tga")==FALSE)
    {
    	printf("nie znaleziono pliku .tga \n");
        exit (0);
    }
}




void resize (int width, int height)
{
    screen_width=width;
    screen_height=height;

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,screen_width,screen_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,(GLfloat)screen_width/(GLfloat)screen_height,10.0f,10000.0f);

    glutPostRedisplay ();
}



void keyboard (unsigned char key, int x, int y)
{

    switch (key)
    {
        case ' ':
            rotation_x_increment=0;
            rotation_y_increment=0;
            rotation_z_increment=0;
        break;

    }
}



void keyboard_s (int key, int x, int y)
{

    switch (key)
    {
        case GLUT_KEY_UP:
            rotation_x += 2.5;
        break;
        case GLUT_KEY_DOWN:
            rotation_x -= 2.5;
        break;
        case GLUT_KEY_LEFT:
            rotation_y += 2.5;
        break;
        case GLUT_KEY_RIGHT:
            rotation_y -= 2.5;
        break;
    }
}




void display(void)
{
    int l_index;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.0,0.0,-300);


    if (rotation_x > 359) rotation_x = 0;
    if (rotation_y > 359) rotation_y = 0;
    if (rotation_z > 359) rotation_z = 0;

    glRotatef(rotation_x,1.0,0.0,0.0);
    glRotatef(rotation_y,0.0,1.0,0.0);
    glRotatef(rotation_z,0.0,0.0,1.0);

    glBindTexture(GL_TEXTURE_2D, tex[0]);

    glBegin(GL_TRIANGLES);
    for (l_index=0;l_index<object.polygons_qty;l_index++)
    {
        glTexCoord2f( object.mapcoord[ object.polygon[l_index].a ].u, object.mapcoord[ object.polygon[l_index].a ].v);
        glVertex3f( object.vertex[ object.polygon[l_index].a ].x, object.vertex[ object.polygon[l_index].a ].y, object.vertex[ object.polygon[l_index].a ].z);
        glTexCoord2f( object.mapcoord[ object.polygon[l_index].b ].u, object.mapcoord[ object.polygon[l_index].b ].v);
        glVertex3f( object.vertex[ object.polygon[l_index].b ].x, object.vertex[ object.polygon[l_index].b ].y, object.vertex[ object.polygon[l_index].b ].z);
        glTexCoord2f( object.mapcoord[ object.polygon[l_index].c ].u, object.mapcoord[ object.polygon[l_index].c ].v);
        glVertex3f( object.vertex[ object.polygon[l_index].c ].x, object.vertex[ object.polygon[l_index].c ].y, object.vertex[ object.polygon[l_index].c ].z);
    }
    glEnd();

    glFlush();
    glutSwapBuffers();
}



int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(screen_width,screen_height);
    glutInitWindowPosition(0,0);
    glutCreateWindow("3DMAX");
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc (resize);
    glutKeyboardFunc (keyboard);
    glutSpecialFunc (keyboard_s);
    init();
    glutMainLoop();

    return(0);
}
