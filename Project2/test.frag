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
uniform int has_relief;


out vec4 color;


vec2 relief_linear_bin(vec2 texCoord, vec3 viewDir)
{
	//steep test

	const float minLayer = 500;
	const float maxLayer = 1000;
	
		
	const float numLayers =  mix(maxLayer, minLayer, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
	float layerDepth = 1.0 / numLayers;

	float currentDepth = 0.0; //initial value

	vec2 P = viewDir.xy/viewDir.z * Strength;
	vec2 deltaTex = P / numLayers;

	vec2 currentUV = texCoord;
	float depthVal = texture(HeightMap, currentUV).r;

	while(currentDepth < depthVal)
	{
		currentUV -= deltaTex;
		depthVal = texture(HeightMap, currentUV).r;
		currentDepth += layerDepth;
	}

	vec2 deltaUV = deltaTex / 2;
	float deltaHeight = layerDepth /2;

	currentUV += deltaUV;
	currentDepth -= deltaHeight;

	const int binary_step = 2;
	for(int i = 0; i < binary_step; i++)
	{
		deltaUV /= 2;
		deltaHeight /= 2;

		depthVal = texture(HeightMap, currentUV).r;
		
		if(depthVal > currentDepth)
		{
			currentUV -= deltaUV;
			currentDepth += deltaHeight;
		}
		else
		{
			currentUV += deltaUV;
			currentDepth -= deltaHeight;
		}

	}

	return currentUV;


	
	
	/*vec2 previousTex = currentUV + deltaTex;
	
	float after = depthVal - currentDepth;
	float before = texture(HeightMap, previousTex).r - currentDepth + layerDepth;

	float weight = after / (after - before);
	vec2 finalTex = previousTex * weight + currentUV * (1.0 - weight);


	return finalTex;*/

}





void main(void)
{

	
	vec3 view = normalize(CamerainTan - FraginTan);
	vec2 texC = UV;
		
	if(has_relief == 1)
	{
		texC = relief_linear_bin(UV, view);
	}
		
	

	//if(texC.x > 1.0 || texC.y > 1.0 || texC.x < 0.0 || texC.y < 0.0)
	//	discard;

	vec3 normal = texture(normalMap, texC).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	vec3 Col = texture(tex, texC).rgb;

	vec3 ambient = 0.1 * Col;

	vec3 lightDir = normalize(LightinTan - FraginTan);
	float diff = max (dot(lightDir, normal),0.0);

	vec3 diffuse = diff * Col;

	vec3 H = normalize(lightDir + view);

	float spec = pow(max(dot(normal, H), 0.0), Shininess);
	vec3 specular = vec3(0.2) * spec;

	color = vec4(ambient + diffuse + specular, 1.0);



	


	
}