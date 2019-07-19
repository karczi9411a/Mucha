// PER-PIXEL Lighting
// (C) 2017 Grzegorz �ukawski

// Przekazanie do fragment shadera:
varying vec3 normal;

void main() {
	// Przekazanie wektora normalnego:
	normal = gl_NormalMatrix * gl_Normal;
	// Kolor w�asny obiektu 3D
	// (ustawiany w aplikacji funkcj� glColor):
	gl_FrontColor = gl_Color;
	// Standardowe transformacje wierzcho�ka:
	gl_Position = ftransform();
}
