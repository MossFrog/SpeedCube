#version 330 core
in vec2 TexCoord;

in vec3 FragPos;  
in vec3 Normal; 

out vec4 color;

uniform sampler2D ourTexture1;

uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float lightDist;


void main()
{
	// Ambient
    float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) / (lightDist / 2);
    color = vec4(result, 1.0f) + (texture(ourTexture1, TexCoord)/2);

	//color = texture(ourTexture1, TexCoord); //* vec4(ourColor, 1.0f);  
}