#version 430 core


in vec2 UV;
in vec3 LightinTan;
in vec3 CamerainTan;
in vec3 FraginTan;
in vec3 FragPos;
in vec3 N;


uniform sampler2D tex;
uniform sampler2D normalMap;
uniform sampler2D HeightMap;


uniform vec4 Ambient;
uniform vec3 LightColor;

uniform float Shininess;
uniform float Strength; //serve as height scale

out vec4 color;


vec2 relief_linear_bin(vec2 texCoord, vec3 viewDir)
{
	const int linear_step = 10;
	const int binary_step = 100;
	

	float numLayers =  mix (linear_step, binary_step,  abs(dot(vec3(0,0,1), viewDir)));


	float height_inc = 1.0 / linear_step;

	float currentHeight = 0;

	vec2 tex2 = Strength * viewDir.xy/viewDir.z/numLayers;

	vec2 currentTex = texCoord;

	float fetch_height = texture(HeightMap, currentTex).r;

	while(fetch_height > currentHeight)
	{
		currentHeight += height_inc;
		currentTex -= tex2;

		fetch_height = texture(HeightMap, currentTex).r;

	}



	vec2 deltaCoord = tex2/2;
	float deltaHeight = height_inc/2;

	currentTex  += deltaCoord;
	currentHeight -= deltaHeight;

	for(int i = 0; i  < binary_step ; i++)
	{
		deltaCoord /= 2;
		deltaHeight /= 2;

		fetch_height = texture(HeightMap, currentTex).r;

		if(fetch_height > currentHeight)
		{
			currentTex -= deltaCoord;
			currentHeight += deltaHeight;
		}
		else
		{
			currentTex += deltaCoord;
			currentHeight -= deltaHeight;
		}
	}


	return currentTex;

}





void main(void)
{

	
	vec3 view = normalize(FraginTan-CamerainTan);
	
		
	vec2 texC = relief_linear_bin(UV, view);
		

	if(texC.x > 1.0 || texC.y > 1.0 || texC.x < 0.0 || texC.y < 0.0)
		discard;



	vec3 normal = texture(normalMap, texC).rgb;

	normal = normalize(normal*2.0 -1.0);

	normal = vec3(1,-1,1) * normal;


	vec3 Color = texture(tex, texC ).rgb;

	vec3 amb = 0.5 * Color;

	vec3 light = normalize(LightinTan - FraginTan);
	float diff = max(dot(light, normal), 0.0);

	vec3 diffuse = diff * normal;

	vec3 reflectDir = reflect(-light, normal);

	vec3 H = normalize(light + view);

	float spec = pow (max( dot(normal, H), 0.0) , 32.0); 

	vec3 specular = vec3(0.2)*spec;


	color = vec4(amb + diffuse + specular, 1.0f);


	


	
}