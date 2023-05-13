////////////////////////////////////////////////////////////////////

struct Material {
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float  specPower;
};

////////////////////////////////////////////////////////////////////

struct Light {
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 dirW;
};

////////////////////////////////////////////////////////////////////

uniform extern float4x4 gWorld;
uniform extern float4x4 gWorldInv;
uniform extern float4x4 gWVP;
uniform extern Material gMtrl;
uniform extern Light    gLight;
uniform extern float3   gEyePosW;
uniform extern float2   gWaveMapOffset0;
uniform extern float2   gWaveMapOffset1;
uniform extern texture  gWaveMap0;
uniform extern texture  gWaveMap1;
uniform extern texture  gEnvMap;

////////////////////////////////////////////////////////////////////

sampler EnvMapS = sampler_state {
	Texture = <gEnvMap>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

////////////////////////////////////////////////////////////////////

sampler WaveMapS0 = sampler_state {
	Texture = <gWaveMap0>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

////////////////////////////////////////////////////////////////////

sampler WaveMapS1 = sampler_state {
	Texture = <gWaveMap1>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

////////////////////////////////////////////////////////////////////

struct OutputVS {
	float4 posH      : POSITION0;
	float3 toEyeT    : TEXCOORD0;
	float3 lightDirT : TEXCOORD1;
	float2 tex0      : TEXCOORD2;
	float2 tex1      : TEXCOORD3;
	float3 toEyeW    : TEXCOORD4;
	float3 normalW   : TEXCOORD5;
};

////////////////////////////////////////////////////////////////////

OutputVS WaterVS(float3 posL : POSITION0, float2 tex0 : TEXCOORD0) {
	OutputVS outVS;

	// Build TBN-basis. For flat water grid in the xz-plane in object space, the TBN-basis has a very simple form.
	float3x3 TBN;
	TBN[0] = float3(1.0f, 0.0f, 0.0f); // Tangent goes along object space x-axis.
	TBN[1] = float3(0.0f, 0.0f, -1.0f);// Binormal goes along object space -z-axis
	TBN[2] = float3(0.0f, 1.0f, 0.0f); // Normal goes along object space y-axis

	// Matrix transforms from object space to tangent space.
	float3x3 toTangentSpace = transpose(TBN);

	// Transform eye position to local space.
	float3 eyePosL = mul(float4(gEyePosW, 1.0f), gWorldInv).xyz;

	// Transform to-eye vector to tangent space.
	float3 toEyeL = eyePosL - posL;
	outVS.toEyeT = mul(toEyeL, toTangentSpace);

	// Transform light direction to tangent space.
	float3 lightDirL = mul(float4(gLight.dirW, 0.0f), gWorldInv).xyz;
	outVS.lightDirT = mul(lightDirL, toTangentSpace);

	// Get to-eye in world space also for environment map look-up.
	outVS.toEyeW = gEyePosW - mul(float4(posL, 1.0f), gWorld);

	// Water plane lies in xz-plane, so geometric normal is (0, 1, 0).
	outVS.normalW = mul(float4(0.0f, 1.0f, 0.0f, 0.0f), gWorld);

	// Transform to homogeneous clip space.
	outVS.posH = mul(float4(posL, 1.0f), gWVP);

	// Scroll texture coordinates.
	outVS.tex0 = tex0 + gWaveMapOffset0;
	outVS.tex1 = tex0 + gWaveMapOffset1;

	return outVS;
}

////////////////////////////////////////////////////////////////////

float4 WaterPS(float3 toEyeT:TEXCOORD0, float3 lightDirT:TEXCOORD1, float2 tex0:TEXCOORD2, float2 tex1:TEXCOORD3, float3 toEyeW:TEXCOORD4, float3 normalW:TEXCOORD5):COLOR {
	// Interpolated normals can become unnormal--so normalize.
	// Note that toEyeW and normalW do not need to be normalized
	// because they are just used for a reflection and environment
	// map look-up and only direction matters.
	toEyeT = normalize(toEyeT);
	lightDirT = normalize(lightDirT);

	// Light vector is opposite the direction of the light.
	float3 lightVecT = -lightDirT;

	// the texture contains vectors in the [-1,1] range for x,y,z,
	// stored in the RGB components in the range of [0,255] via the f(x) = (0.5x+0.5)*255 formula,
	// automatically done by the program, which exported the normal map
	float3 normalT0 = tex2D(WaveMapS0, tex0); // tex2D returns result in the [0,1] range after dividing internally by 255
	float3 normalT1 = tex2D(WaveMapS1, tex1);

	// to convert from [a,b] to [c,d] we use f(x) = c + ((d-c)/(b-a)) * (x-a)
	// Expand from [0,1] compressed interval to [-1,1] interval.
	normalT0 = 2.0f * normalT0 - 1.0f;
	normalT1 = 2.0f * normalT1 - 1.0f;

	// Average the two vectors.
	float3 normalT = normalize(0.5f * (normalT0 + normalT1));

	// Compute the reflection vector.
	float3 r = reflect(-lightVecT, normalT);

	// Determine how much (if any) specular light makes it into the eye.
	float t = pow(max(dot(r, toEyeT), 0.0f), gMtrl.specPower);

	// Determine the diffuse light intensity that strikes the vertex.
	float s = max(dot(lightVecT, normalT), 0.0f);

	// If the diffuse light intensity is low, kill the specular lighting term.
	// It doesn't look right to add specular light when the surface receives little diffuse light.
	if (s <= 0.0f) {
		t = 0.0f;
	}

	// Get the reflected color.  Also add some scale of normalT to the environment map look-up vector. This perturbs the look-up
	// vector in a different way each frame (since normalT changes), which gives a ripple effect.
	float3 envMapTex = reflect(-toEyeW, normalW) + (normalT * 10.0f);
	float3 reflectedColor = texCUBE(EnvMapS, envMapTex);

	// Weighted average between the reflected color and usual diffuse material.
	float3 ambientMtrl = (0.5f * reflectedColor + 0.5f * gMtrl.ambient);
	float3 diffuseMtrl = (0.5f * reflectedColor + 0.5f * gMtrl.diffuse);

	float3 specular = t * (gMtrl.spec * gLight.spec).rgb;
	float3 diffuse = s * (diffuseMtrl * gLight.diffuse.rgb);
	float3 ambient = ambientMtrl * gLight.ambient;

	float3 final = ambient + diffuse + specular;

	return float4(final, gMtrl.diffuse.a);
}

////////////////////////////////////////////////////////////////////

technique WaterTech {
	pass P0 {
		vertexShader = compile vs_2_0 WaterVS();
		pixelShader  = compile ps_2_0 WaterPS();

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}

////////////////////////////////////////////////////////////////////
