/*
 *	Targa.h
 */

typedef struct {
	GLuint bpp;		// iloœæ bitów na piksel
	GLuint width;	// rozmiary w pikselach
	GLuint height;
} TARGAINFO;

// Funkcje
GLubyte *LoadTGAImage(const char *filename, TARGAINFO *info);
bool LoadTGATexture(const char *filename);
bool LoadTGAMipmap(const char *filename);
