#version 430 core


in vec2 UV;
in vec3 LightinTan;
in vec3 CamerainTan;
in vec3 FraginTan;
in vec3 FragPos;

uniform sampler2D tex;
uniform sampler2D normalMap;
uniform sampler2D HeightMap;


uniform vec4 Ambient;
uniform vec3 LightColor;

uniform float Shininess;
uniform float Strength; //serve as height scale

out vec4 color;



vec2 parallaxMapping(vec2 texCoord, vec3 viewDir)
{
	const float mmin = 20;
	const float mmax = 40;

	float numLayer = mix( mmax, mmin, abs(dot(vec3(0.0, 0.0, 1.0), viewDir))) ;

	float layerSize = 1.0 / numLayer;


	float currLayer = 0.0;

	vec2 P = viewDir.xy/ viewDir.z * Strength;

	vec2 change_in_UV = P/numLayer;

	vec2 currUV = texCoord;
	float currH = texture(HeightMap, currUV).r;

	while(currLayer < currH )
	{
		currUV -= change_in_UV;
		currH = texture(HeightMap, currUV).r;
		currLayer += layerSize;
	}

	vec2 prevCoord = currUV + change_in_UV;

	float after = currH - currLayer;
	float before = texture(HeightMap, prevCoord).r  - currLayer + layerSize;

	float weight = after / (after - before);

	vec2 out_UV = prevCoord * weight + currUV * (1.0 - weight);

	return out_UV;
	
}



float parallaxSoftShadow(vec3 L, vec2 T, float initialHeight)
{
	float shadescale = 1;

	const float mmin = 15;
	const float mmax = 30;

	if( dot(vec3(0,0,1), L)>0 )
	{
		float numsamples = 0;
		shadescale = 0;
		float numLayers = mix(mmax, mmin, abs (dot(vec3(0,0,1),L)));
		float layerHeight = initialHeight/numLayers;

		vec2 texStep = Strength * L.xy / L.z / numLayers;

		float currentLayerH = initialHeight - layerHeight;
		vec2 currentTex = T + texStep;
		float heightFromTexture = texture(HeightMap, currentTex).r;
		int stepIndex = 1;

		while(currentLayerH > 0)
		{
			if(heightFromTexture < currentLayerH)
			{
				numsamples += 1;
				float newshadescale = (currentLayerH - heightFromTexture) * (1.0 - stepIndex / numLayers);
				shadescale = max(shadescale, newshadescale);
			}

			stepIndex += 1;
			currentLayerH -= layerHeight;
			currentTex += texStep;
			heightFromTexture = texture(HeightMap, currentTex).r;
		}
	
		if(numsamples < 1)	
		{	
			shadescale = 1;
		}
		else
		{
			shadescale = 1.0 - shadescale;
		}

		return shadescale;
		
	
	}




}




void main(void)
{

	
	vec3 view = normalize(CamerainTan - FraginTan);
	
	
	vec2 texC = parallaxMapping(UV, view);


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