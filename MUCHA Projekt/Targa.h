/*
 *	Targa.h
 */

typedef struct {
	GLuint bpp;		// ilo�� bit�w na piksel
	GLuint width;	// rozmiary w pikselach
	GLuint height;
} TARGAINFO;

// Funkcje
GLubyte *LoadTGAImage(const char *filename, TARGAINFO *info);
bool LoadTGATexture(const char *filename);
bool LoadTGAMipmap(const char *filename);
