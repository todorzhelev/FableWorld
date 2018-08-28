extern float4x4 WVP;
extern float4   cameraPos;
extern texture  pTexture;
extern float    pTime;
extern float4   pAccel;

///////////////////////////////////////////////////////////

sampler STex = sampler_state
{
	Texture = <pTexture>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = POINT;
	AddressU  = CLAMP;
    AddressV  = CLAMP;
};
 
///////////////////////////////////////////////////////////

struct VS_OUTPUT
{
    float4 pos    	: POSITION0;
	float2 pTexture : TEXCOORD0;
	float  size     : PSIZE; 
};

///////////////////////////////////////////////////////////

struct VS_INPUT
{
	float3 pos 		: POSITION0;
	float3 vel 		: TEXCOORD0;
	float  size		: TEXCOORD1;
	float  time		: TEXCOORD2;
	float  lifeTime : TEXCOORD3;
	float  mass		: TEXCOORD4;
	float4 color 	: COLOR0;
};

///////////////////////////////////////////////////////////

VS_OUTPUT GunEffectVS(VS_INPUT inp)
{
	VS_OUTPUT o = (VS_OUTPUT)0;
	
	float t = pTime - inp.time;

	inp.pos = inp.pos + inp.vel*t + 0.5f * pAccel * t * t;

	o.pos = mul(float4(inp.pos, 1.0f), WVP);

	float d = distance(inp.pos, cameraPos);

	int viewportHeight = 900;
	o.size = viewportHeight*inp.size/(1.0f + 8.0f*d);

    return o;
}

///////////////////////////////////////////////////////////

float4 GunEffectPS(float2 pTexture:TEXCOORD0) : COLOR0
{
	return tex2D(STex, pTexture);
}

///////////////////////////////////////////////////////////

technique GunEffectTech
{
    pass P0
    {
        vertexShader = compile vs_2_0 GunEffectVS();
        pixelShader  = compile ps_2_0 GunEffectPS();
        
        PointSpriteEnable = true;
        AlphaBlendEnable = true;
	    SrcBlend     = One;
	    DestBlend    = One;
	    
	    ZWriteEnable = false;
    }
}

///////////////////////////////////////////////////////////