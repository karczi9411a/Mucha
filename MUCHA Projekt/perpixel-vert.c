// PER-PIXEL Lighting
// (C) 2017 Grzegorz £ukawski

// Przekazanie do fragment shadera:
varying vec3 normal;

void main() {
	// Przekazanie wektora normalnego:
	normal = gl_NormalMatrix * gl_Normal;
	// Kolor w³asny obiektu 3D
	// (ustawiany w aplikacji funkcj¹ glColor):
	gl_FrontColor = gl_Color;
	// Standardowe transformacje wierzcho³ka:
	gl_Position = ftransform();
}
