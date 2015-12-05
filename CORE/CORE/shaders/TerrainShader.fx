extern float4x4 WVP;
extern float3 lightVector;
extern texture tex0;
extern texture tex1;
extern texture tex2;
extern texture blendMap;

//we repeat the same texture so we can have better quality
static float texScale = 48.0f;

//in the terrain we got 3 textures, which are controlled by the blend map

sampler Tex0S = sampler_state
{
	Texture = <tex0>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler Tex1S = sampler_state
{
	Texture = <tex1>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler Tex2S = sampler_state
{
	Texture = <tex2>;
	MinFilter = ANISOTROPIC;
	MaxAnisotropy = 8;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler BlendMapS = sampler_state
{
	Texture = <blendMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = POINT;
	AddressU  = WRAP;
    AddressV  = WRAP;
};
 
struct VS_OUTPUT
{
    float4 pos         : POSITION0;
    float2 tiledTex    : TEXCOORD0;
    float2 blendMapText : TEXCOORD1;
    float  light        : TEXCOORD2;
};
struct VS_INPUT
{
	float3 pos : POSITION0;
	float3 norm : NORMAL0;
	float2 tex: TEXCOORD0;
};

VS_OUTPUT TerrainVS(VS_INPUT inp)
{
	VS_OUTPUT outVS;

	//saturate makes the result between [0,1]. Outside this interval the light is strange.
    outVS.light = saturate(dot(inp.norm, lightVector) + 0.3);
    
	outVS.pos = mul(float4(inp.pos, 1.0f), WVP);//on huge steep terrains this instruction takes most of the time
	outVS.tiledTex = inp.tex * texScale; 
	outVS.blendMapText = inp.tex;
	
    return outVS;
}

float4 TerrainPS(VS_OUTPUT inp) : COLOR
{
    float3 tex0Color = tex2D(Tex0S, inp.tiledTex);
    float3 tex1Color = tex2D(Tex1S, inp.tiledTex);
    float3 tex2Color = tex2D(Tex2S, inp.tiledTex);
    
    float3 blendMapColor = tex2D(BlendMapS, inp.blendMapText);
    
    tex2Color *= blendMapColor.r;
    tex1Color *= blendMapColor.g;
    tex0Color *= blendMapColor.b;
	
    float3 final = (tex0Color + tex1Color + tex2Color) * inp.light;
 
    return float4(final, 1.0f);
}

technique TerrainTech
{
    pass P0
    {
        vertexShader = compile vs_2_0 TerrainVS();
        pixelShader  = compile ps_2_0 TerrainPS();
    }
}
