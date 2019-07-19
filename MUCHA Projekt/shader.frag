varying vec3 normal, lightDir;
uniform sampler2D tex0, tex1;
vec4 kolor0, kolor1;

void main() {
    float intensity = max(dot(normal, lightDir), 0.0);

	vec4 diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	vec4 ambient = gl_FrontMaterial.ambient* gl_LightSource[0].ambient;
	vec4 globalAmbient = gl_LightModel.ambient * gl_FrontMaterial.ambient;

   	vec4 specular = vec4(0.0);
	if (intensity > 0.0) {
		float intensityHV = max(dot(normal, gl_LightSource[0].halfVector.xyz),0.0);
		specular = gl_FrontMaterial.specular * gl_LightSource[0].specular *
				pow(intensityHV,gl_FrontMaterial.shininess);
	}


	kolor0 = texture2D(tex0, gl_TexCoord[0].st);

	gl_FragColor = kolor0 * intensity * diffuse + ambient + globalAmbient + specular;
}

