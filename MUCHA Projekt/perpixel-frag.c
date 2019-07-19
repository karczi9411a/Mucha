// PER-PIXEL Lighting
// (C) 2017 Grzegorz £ukawski

// Sta³y wektor "patrzenia" - oszustwo które standardowo stosuje OpenGL
// (równie¿ przy automatycznych obliczeniach wektora po³ówkowego):
const vec3 lookvec = vec3(0, 0, 1);

vec4 ObliczKolor(in vec3 norm) {
	// Wektor padania œwiat³a (dla ka¿dego wierzcho³ka taki sam):
	vec3 lightDir = normalize( vec3(gl_LightSource[0].position) );
	// Iloczyn skalarny znormalizowanych wektorów:
	float jasnosc = max( dot(lightDir, norm), 0.0 );

	// Odbicie zwierciadlane -> algorytm Blinna-Phonga:
	vec3 HV = normalize( lightDir + lookvec );
	float NdotHV = max( dot(norm, HV), 0.0 );
	float specular = pow( NdotHV, 16.0 );

	// Kolor wynikowy + rozb³yski:
	vec4 kolor = gl_Color * jasnosc;
	if(jasnosc > 0.0)
		kolor = (1-specular)*kolor + specular*vec4(5, 1, 1, 1);

	return clamp(kolor, 0, 1);
}

// Przekazanie z vertex shadera:
varying vec3 normal;

void main() {
	// Normalizacja interpolowanej normalnej:
	vec3 n = normalize(normal);
	gl_FragColor = ObliczKolor(n);
}
