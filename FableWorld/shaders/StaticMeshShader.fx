struct Mtrl
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float  specPower;
};

struct Light
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 lightVector;  
};

extern float4x4 WVP;
extern Mtrl mtrl;
extern Light light;
extern texture text;
extern bool picked;

sampler STex = sampler_state
{
	Texture = <text>;
	MinFilter = Anisotropic;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	MaxAnisotropy = 8;
	AddressU  = WRAP;
    AddressV  = WRAP;
};
 
struct VS_OUTPUT
{
    float4 pos    : POSITION0;
    float3 norm   : TEXCOORD0;
    float2 tex    : TEXCOORD2;
};
struct VS_INPUT
{
	float3 pos : POSITION0;
	float3 norm : NORMAL0;
	float2 tex : TEXCOORD0;
};

VS_OUTPUT StaticMeshVS(VS_INPUT inp)
{
	VS_OUTPUT o;
	
	//0.0 because it is a vector
	o.norm = mul(float4(inp.norm, 0.0f), WVP);
	
	//1.0 because it is a point
	o.pos = mul(float4(inp.pos, 1.0f), WVP);
	o.tex = inp.tex;

    return o;
}

float4 StaticMeshPS(VS_OUTPUT inp) : COLOR
{
	inp.norm = normalize(inp.norm);
	
	float lightAmount = saturate(dot(light.lightVector, inp.norm));
	
	float3 diffuse = lightAmount*(mtrl.diffuse*light.diffuse);
	float3 ambient = mtrl.ambient*light.ambient;
	

	float4 texColor = tex2D(STex, inp.tex);
	
	float3 color;

	if( !picked )
	{
		color = (ambient + diffuse)*texColor;
	}
	else
	{
		color = (ambient + diffuse*5.0)*texColor;
	}
	
	return float4(color, texColor.a);
    
}

technique StaticMeshTech
{
    pass P0
    {
        vertexShader = compile vs_2_0 StaticMeshVS();
        pixelShader  = compile ps_2_0 StaticMeshPS();
    }
}