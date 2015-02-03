extern float4x4 WVP;
extern texture text;
extern float EffectTime;
extern float4 CameraPos;

sampler STex = sampler_state
{
	Texture = <text>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = POINT;
	AddressU  = CLAMP;
    AddressV  = CLAMP;
};
 
struct VS_OUTPUT
{
    float4 pos    : POSITION0;
	float2 text  : TEXCOORD0;
	float size   : PSIZE; 
};
struct VS_INPUT
{
	float3 pos : POSITION0;
	float3 velocity: TEXCOORD0;
	float size:TEXCOORD1;
	float time:TEXCOORD2;
};

VS_OUTPUT GunEffectVS(VS_INPUT inp)
{
	VS_OUTPUT o = (VS_OUTPUT)0;
	
	float t = EffectTime - inp.time;

	//float3 acceleration = float3(0,-9.8,0);

	//inp.pos = (inp.pos + inp.velocity*t + 0.5f * t * t);
	//inp.pos = (inp.pos + inp.velocity*t);

	o.pos = mul(float4(inp.pos, 1.0f), WVP);

	float dist = distance(inp.pos,CameraPos);

	//o.size = inp.size;

	o.size = 520*inp.size/(1+8*dist);

    return o;
}

float4 GunEffectPS(float2 text:TEXCOORD0) : COLOR0
{
	return tex2D(STex, text);
}

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
	    
	    // Don't write to depth buffer; that is, particles don't obscure
	    // the objects behind them.
	   // ZWriteEnable = false;
    }
}