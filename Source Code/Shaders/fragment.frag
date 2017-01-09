#version 330 core
in vec2 TexCoord;
in vec3 FragPos;  
in vec3 Normal; 

out vec4 color;

uniform sampler2D ourTexture1;

//-- We have an array of light positions --//
uniform vec3 lightPosArr[5]; 


uniform vec3 lightColor;
uniform vec3 objectColor;


void main()
{
	// Ambient
    float ambientStrength = 0.2f;

	//-- No ambient color !! --//
    vec3 ambient = ambientStrength * vec3(0.0f,0.0f,0.0f);

	//-- Pre define the variables and then loop --//
	vec3 norm;
	vec3 lightDir;
	float diff;
	vec3 diffuse;
	vec3 result;

	for(int i = 0; i <= 4; i++)
	{
		// Diffuse 
		norm = normalize(Normal);
		lightDir = normalize(lightPosArr[i] - FragPos);
		//-- We exponentiate using the distance between the fragment and the light source --//
		diff = max(dot(norm, lightDir), 0.0) / pow(distance(lightPosArr[i], FragPos),2.0f);
		diffuse = diff * lightColor * 2.0f;

		result = (ambient + diffuse);

		color += vec4(result, 1.0f);
	}

	color += (texture(ourTexture1, TexCoord)/2);
    

    //color = texture(ourTexture1, TexCoord);
}