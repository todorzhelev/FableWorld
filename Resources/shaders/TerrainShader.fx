extern float4x4 WVP;
extern float3 lightVector;
extern texture tex0;
extern texture tex1;
extern texture tex2;
extern texture blendMap;

//we repeat the same texture so we can have better quality. why this works?
static float texScale = 50;

//in the terrain we got 3 textures, which are controlled by the blend map

////////////////////////////////////////////////////////////////////

//stone texture
sampler TexStoneSampler = sampler_state {
	Texture = <tex0>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

////////////////////////////////////////////////////////////////////

//dirt texture
sampler TexDirtSampler = sampler_state {
	Texture = <tex1>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

////////////////////////////////////////////////////////////////////

//grass texture
sampler TexGrassSampler = sampler_state {
	Texture = <tex2>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

////////////////////////////////////////////////////////////////////

sampler BlendMapS = sampler_state {
	Texture = <blendMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = POINT;
	AddressU  = WRAP;
    AddressV  = WRAP;
};
 
////////////////////////////////////////////////////////////////////

struct VS_OUTPUT {
    float4 pos          : POSITION0;
    float2 tiledTex     : TEXCOORD0;
    float2 blendMapText : TEXCOORD1;
    float  light        : TEXCOORD2;
};

////////////////////////////////////////////////////////////////////

struct VS_INPUT {
	float3 pos		    : POSITION0;
	float3 norm			: NORMAL0;
	float2 blendMapTex  : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////

VS_OUTPUT TerrainVS(VS_INPUT inp) {
	VS_OUTPUT outVS;

	//saturate makes the result between [0,1]. Outside this interval the light is strange.
    outVS.light = saturate(dot(inp.norm, lightVector) + 0.3);
    
	outVS.pos		    = mul(float4(inp.pos, 1.0f), WVP);//on huge steep terrains this instruction takes most of the time
	outVS.tiledTex		= inp.blendMapTex * texScale;
	outVS.blendMapText  = inp.blendMapTex;
	
    return outVS;
}

////////////////////////////////////////////////////////////////////

float4 TerrainPS(VS_OUTPUT inp) : COLOR {
    float3 texStoneColor = tex2D(TexStoneSampler, inp.tiledTex);
    float3 texDirtColor  = tex2D(TexDirtSampler,  inp.tiledTex);
    float3 texGrassColor = tex2D(TexGrassSampler, inp.tiledTex);
    
    float3 blendMapColor = tex2D(BlendMapS, inp.blendMapText);
    
	texStoneColor *= blendMapColor.b;
	texDirtColor  *= blendMapColor.r;
	texGrassColor *= blendMapColor.g*0.8;
	
    float3 final = (texStoneColor + texDirtColor + texGrassColor) * inp.light*1.3;
 
    return float4(final, 1.0f);
}

////////////////////////////////////////////////////////////////////

technique TerrainTech {
    pass P0 {
        vertexShader = compile vs_2_0 TerrainVS();
        pixelShader  = compile ps_2_0 TerrainPS();
    }
}

////////////////////////////////////////////////////////////////////
