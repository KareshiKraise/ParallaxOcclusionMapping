#version 430 core


in vec2 UV;
in vec3 toLightinTan;
in vec3 toCamerainTan;



uniform sampler2D tex;
uniform sampler2D normalMap;
uniform sampler2D HeightMap;

uniform vec4 Ambient;
uniform vec3 LightColor;

uniform float Shininess;
uniform float Strength;



out vec4 color;



vec2 parallaxMapping(in vec3 V, in vec2 T, out float outputHeight)
{
	const float mmin = 10;
	const float mmax = 50;

	float numLayers = mix(mmax, mmin, abs(dot(vec3(0,0,1),V)));

	float layerH = 1.0/numLayers;
	float currentH = 0;

	vec2 fetch = Strength * V.xy / V.z / numLayers;

	vec2 currentUV = T;

	float heightScalar = texture(HeightMap, currentUV).r;

	while(heightScalar > currentH)
	{
		currentH +=layerH;
		currentUV -= fetch;

		heightScalar = texture(HeightMap, currentUV).r;
	}
	

	vec2 deltaTex = fetch / 2;
	float deltaH = layerH / 2;


	currentUV += deltaTex;
	currentH -= deltaH;

	const int search = 5;
	for(int i = 0 ; i < search; i++)
	{
		deltaTex = deltaTex /  2;
		deltaH = deltaH / 2;

		heightScalar = texture(HeightMap, currentUV).r;

		if(heightScalar > currentH)
		{
			currentUV  -= deltaTex;
			currentH +- deltaH;

		} 
		else 
		{
			currentUV += deltaTex;
			currentH -= deltaH;
		}
	}

	outputHeight = currentH;
	return currentUV;
}



float parallaxSoftShadow(in vec3 L, in vec2 initialTexCoord, in float initialHeight)
{
	float shadeScale = 1;
	const float mmin = 15;
	const float mmax = 30;

	if(dot(vec3(0,0,1),L) > 0)
	{
		float numsamples = 0;
		shadeScale = 0;
		float numLayer = mix(mmax, mmin, abs(dot(vec3(0,0,1),L)));
		float layerH = initialHeight / numLayer;
		vec2 fetch = Strength * L.xy / L.z / numLayer;
		
		//current 
		float currentH = initialHeight - layerH;
		vec2 currentUV = initialTexCoord +  fetch;

		float scalarH = texture(HeightMap, currentUV).r;
		int step = 1;


		while(currentH > 0)
		{
			if(scalarH < currentH)
			{
				numsamples += 1;
				float neoshadeScale = (currentH - scalarH) * (1.0 - step / numLayer);
				shadeScale = max(shadeScale, neoshadeScale);
			}

			step += 1;
			currentH -= layerH;
			currentUV += fetch;
			scalarH = texture(HeightMap, currentUV).r;
		}

		if(numsamples < 1.0)
		{
			shadeScale = 1.0;
		}
		else
		{
			shadeScale = 1.0 - shadeScale;
		}
	}

	return shadeScale;

}


vec4 normalMapping(in vec2 T, in vec3 L, in vec3 V, float shadowMultiplier)
{
	vec3 N = normalize(texture(normalMap, T).xyz * 2 - 1);
	vec3 D = texture(tex, T).rgb;

	float diff = clamp(dot(N, L), 0 , 1);

	float spec = 0;

	if(dot(N, L) > 0.2)
	{
		//vec3 R = reflect(-L, N);
		//spec = pow(dot(R,V), Shininess) / 1.5;
		vec3 H = normalize(L + V);
		spec = pow(max(dot(N,H) , 0.0), Shininess );
	}


	vec4 resColor;
	resColor.rgb = D * (LightColor + (diff + spec) * pow(shadowMultiplier, 4));
	resColor.a = 1;

	return resColor;
}


void main(void)
{

	vec3 V = normalize(toCamerainTan);
	vec3 L = normalize(toLightinTan);

	float parallaxHeight;
	vec2 T = parallaxMapping(V, UV, parallaxHeight);

	float shadowMultiplier = parallaxSoftShadow(L, T, parallaxHeight - 0.05);

	color = normalMapping(T,  L , V, shadowMultiplier);





	
}