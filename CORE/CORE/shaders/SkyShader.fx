extern float4x4 WVP;
extern texture tex0;

sampler SkyS = sampler_state
{
    Texture   = <tex0>;
    MinFilter = LINEAR; 
    MagFilter = LINEAR;
    MipFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};

struct VS_OUTPUT
{
	float4 pos : POSITION0;
	float3 tex : TEXCOORD0;
	
};

struct VS_INPUT
{
	float3 pos : POSITION0;
	float3 tex : TEXCOORD0;
};

VS_OUTPUT SkyVS(VS_INPUT inp)
{
	VS_OUTPUT o;
	
    o.pos = mul(float4(inp.pos, 1.0f), WVP);
	//we set z = w, because after the perspective divide (z/w), which will be done later by the gpu, 
	//the z/w value will be 1, which means that the skysphere will be always rendered behind everything
	o.pos.z = o.pos.w;
	
	//here inp.pos is in the local space of the skysphere 
	//and we can use these coordinates for the cubemap
    o.tex = inp.pos;
	
	return o;
}

float4 SkyPS(VS_OUTPUT inp) : COLOR
{
	//instead of tex2D we use texCUBE, because this is cubemap, not regular texture.
    return texCUBE(SkyS, inp.tex);
}

technique SkyTech
{
    pass P0
    {
        vertexShader = compile vs_2_0 SkyVS();
        pixelShader  = compile ps_2_0 SkyPS();

		//the skybox will be outside of the frustum so dont cull it
		CullMode = None;
    }
}

