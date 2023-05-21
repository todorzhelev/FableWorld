////////////////////////////////////////////////////////////////////

struct Material {
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float specPower;
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
uniform extern Light gLight;
uniform extern float3 gEyePosW;

// Texture coordinate offset vectors for scrolling normal maps and displacement maps.
uniform extern float2 gWaveNMapOffset0;
uniform extern float2 gWaveNMapOffset1;
uniform extern float2 gWaveDMapOffset0;
uniform extern float2 gWaveDMapOffset1;

// Two normal maps and displacement maps.
uniform extern texture gWaveMap0;
uniform extern texture gWaveMap1;
uniform extern texture gWaveDispMap0;
uniform extern texture gWaveDispMap1;

// User-defined scaling factors to scale the heights sampled from the displacement map into a more convenient range.
uniform extern float2 gScaleHeights;

// Space between grids in x,z directions in local space used for finite differencing.
uniform extern float2 gGridStepSizeL;

////////////////////////////////////////////////////////////////////

static const float DMAP_SIZE = 128.0f;
static const float DMAP_DX = 1.0f / DMAP_SIZE;

////////////////////////////////////////////////////////////////////

sampler WaveMapS0 = sampler_state {
	Texture = <gWaveMap0>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 12;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

////////////////////////////////////////////////////////////////////

sampler WaveMapS1 = sampler_state {
	Texture = <gWaveMap1>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 12;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

////////////////////////////////////////////////////////////////////

sampler DispMapS0 = sampler_state {
	Texture = <gWaveDispMap0>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};

////////////////////////////////////////////////////////////////////

sampler DispMapS1 = sampler_state {
	Texture = <gWaveDispMap1>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};

////////////////////////////////////////////////////////////////////

struct VS_OUTPUT {
	float4 posH : POSITION0;
	float3 toEyeT : TEXCOORD0;
	float3 lightDirT : TEXCOORD1;
	float2 tex0 : TEXCOORD2;
	float2 tex1 : TEXCOORD3;
};

////////////////////////////////////////////////////////////////////

struct VS_INPUT {
	float3 posL : POSITION0;
	float2 normalMapCoord : TEXCOORD0;
	float2 displacementMapCoord : TEXCOORD1;
};

////////////////////////////////////////////////////////////////////

float DoDispMapping(float2 texC0, float2 texC1) {
	// Transform to texel space
	float2 texelpos = DMAP_SIZE * texC0;

	// Determine the lerp amounts.
	float2 lerps = frac(texelpos);

	float dmap0[4];
	dmap0[0] = tex2Dlod(DispMapS0, float4(texC0, 0.0f, 0.0f)).r;
	dmap0[1] = tex2Dlod(DispMapS0, float4(texC0, 0.0f, 0.0f) + float4(DMAP_DX, 0.0f, 0.0f, 0.0f)).r;
	dmap0[2] = tex2Dlod(DispMapS0, float4(texC0, 0.0f, 0.0f) + float4(0.0f, DMAP_DX, 0.0f, 0.0f)).r;
	dmap0[3] = tex2Dlod(DispMapS0, float4(texC0, 0.0f, 0.0f) + float4(DMAP_DX, DMAP_DX, 0.0f, 0.0f)).r;

	// Filter displacement map:
	float h0 = lerp(lerp(dmap0[0], dmap0[1], lerps.x), lerp(dmap0[2], dmap0[3], lerps.x), lerps.y);

	texelpos = DMAP_SIZE * texC1;
	lerps = frac(texelpos);

	float dmap1[4];
	dmap1[0] = tex2Dlod(DispMapS1, float4(texC1, 0.0f, 0.0f)).r;
	dmap1[1] = tex2Dlod(DispMapS1, float4(texC1, 0.0f, 0.0f) + float4(DMAP_DX, 0.0f, 0.0f, 0.0f)).r;
	dmap1[2] = tex2Dlod(DispMapS1, float4(texC1, 0.0f, 0.0f) + float4(0.0f, DMAP_DX, 0.0f, 0.0f)).r;
	dmap1[3] = tex2Dlod(DispMapS1, float4(texC1, 0.0f, 0.0f) + float4(DMAP_DX, DMAP_DX, 0.0f, 0.0f)).r;

	// Filter displacement map:
	float h1 = lerp(lerp(dmap1[0], dmap1[1], lerps.x), lerp(dmap1[2], dmap1[3], lerps.x), lerps.y);

	// Sum and scale the sampled heights.
	return gScaleHeights.x * h0 + gScaleHeights.y * h1;
}

////////////////////////////////////////////////////////////////////

VS_OUTPUT WaterVS(VS_INPUT inp) {
	VS_OUTPUT outVS;

	// Scroll vertex texture coordinates to animate waves.
	float2 vTex0 = inp.displacementMapCoord + gWaveDMapOffset0;
	float2 vTex1 = inp.displacementMapCoord + gWaveDMapOffset1;

	// Set y-coordinate of water grid vertices based on displacement mapping.
	inp.posL.y = DoDispMapping(vTex0, vTex1);

	// Estimate TBN-basis using finite differencing in local space.
	float r = DoDispMapping(vTex0 + float2(DMAP_DX, 0.0f), vTex1 + float2(0.0f, DMAP_DX));
	float b = DoDispMapping(vTex0 + float2(DMAP_DX, 0.0f), vTex1 + float2(0.0f, DMAP_DX));

	float3x3 TBN;
	TBN[0] = normalize(float3(1.0f, (r - inp.posL.y) / gGridStepSizeL.x, 0.0f));
	TBN[1] = normalize(float3(0.0f, (b - inp.posL.y) / gGridStepSizeL.y, -1.0f));
	TBN[2] = normalize(cross(TBN[0], TBN[1]));

	// Matrix transforms from object space to tangent space.
	float3x3 toTangentSpace = transpose(TBN);

	// Transform eye position to local space.
	float3 eyePosL = mul(float4(gEyePosW, 1.0f), gWorldInv).xyz;

	// Transform to-eye vector to tangent space.
	float3 toEyeL = eyePosL - inp.posL;
	outVS.toEyeT = mul(toEyeL, toTangentSpace);

	// Transform light direction to tangent space.
	float3 lightDirL = mul(float4(gLight.dirW, 0.0f), gWorldInv).xyz;
	outVS.lightDirT = mul(lightDirL, toTangentSpace);

	// Transform to homogeneous clip space.
	outVS.posH = mul(float4(inp.posL, 1.0f), gWVP);

	// Scroll texture coordinates.
	outVS.tex0 = inp.normalMapCoord + gWaveNMapOffset0;
	outVS.tex1 = inp.normalMapCoord + gWaveNMapOffset1;

	return outVS;
}

////////////////////////////////////////////////////////////////////

float4 WaterPS(float3 toEyeT : TEXCOORD0, float3 lightDirT : TEXCOORD1, float2 tex0 : TEXCOORD2, float2 tex1 : TEXCOORD3) : COLOR {
	// Interpolated normals can become unnormal
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

	float3 spec = t * (gMtrl.spec * gLight.spec).rgb;
	float3 diffuse = s * (gMtrl.diffuse * gLight.diffuse.rgb);
	float3 ambient = gMtrl.ambient * gLight.ambient;

	float3 final = ambient + diffuse + spec;

	return float4(final, gMtrl.diffuse.a);
}

////////////////////////////////////////////////////////////////////

technique WaterTech {
	pass P0 {
		vertexShader = compile vs_3_0 WaterVS();
		pixelShader  = compile ps_3_0 WaterPS();
	}
}

////////////////////////////////////////////////////////////////////