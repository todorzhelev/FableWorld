extern float4x4 WVP;

struct VS_OUTPUT
{
    float4 pos : POSITION0;
};


struct VS_INPUT
{
	float3 pos : POSITION0;
};


VS_OUTPUT TransformVS(VS_INPUT inp)
{
	VS_OUTPUT o;

	o.pos = mul(float4(inp.pos, 1.0f), WVP);

    return o;
}

float4 TransformPS() : COLOR
{
	float4 color = float4(1, 1, 0, 0);
    return color;
    
}


technique text3DTech
{

	pass P0
    {
       vertexShader = compile vs_2_0 TransformVS();
	   pixelShader  = compile ps_2_0 TransformPS();
    }
}
