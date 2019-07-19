#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <sys/stat.h>

#include "3DSObject.h"

_3DSOBJ *Load3DSObject(const char *filename){
    FILE *f = fopen(filename, "rb");
    _3DSOBJ object;

    struct stat buf;
    fstat(fileno(f), &buf);
    long filelength = buf.st_size;

    int id, nextpointer;

    while(ftell(f) < filelength){
		fread(&id, 2, 1, f);
		fread(&nextpointer, 4, 1, f);

		switch (id){
			case 0x4d4d:
                break;
			case 0x3d3d:
                break;
			case 0x4000:
				char c;
				do fread(&c, 1, 1, f); while(c != '\0');
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

	fclose(f);

    if(f){
        printf("object loaded");
    } else perror("error");
}
