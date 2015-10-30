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
extern float4x4 FinalBonesMatrices[60];
extern Mtrl mtrl;
extern Light light;
extern texture text;
extern bool picked;

sampler TexS = sampler_state
{
	Texture = <text>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP;
    AddressV  = WRAP;
};
 
struct VS_OUTPUT
{
    float4 pos   : POSITION0;
    float3 norm  : TEXCOORD0;
    float2 tex   : TEXCOORD2;
};

struct VS_INPUT
{
	float3 pos    	: POSITION0;
    float3 norm 	: NORMAL0;
    float2 tex    	: TEXCOORD0;
    float weight0  	: BLENDWEIGHT0; 
    int4 boneIndex 	: BLENDINDICES0;
};

VS_OUTPUT SkinnedMeshVS(VS_INPUT inp)
{
	VS_OUTPUT o;

	//in all models in the game only 2 bones can influence single vertex.
	//by the weight it is controlled how much certain bone influences this vertex
	//weight can be maximum 1.0 which means that the bone with 1.0 weight influences at 100% this vertex 
	//and the second bone has 0.0 weight and it doesnt influence the vertex
	//so in our case we can find the weight of the second bone by subtracting 1 from the first weight 
	float weight1 = 1.0f - inp.weight0;
    
	//set the influence to the vertex from the first bone
    float4 pos = inp.weight0 * mul(float4(inp.pos, 1.0f), FinalBonesMatrices[inp.boneIndex[0]]);
	
	//add the influence to the vertex from the second bone
    pos += weight1 * mul(float4(inp.pos, 1.0f), FinalBonesMatrices[inp.boneIndex[1]]);
	
	//pos will be used as a point in 3d space, this means that we need to set the w component to 1
	//so when we perform translations, the point will move
    pos.w = 1.0f;
    
	//set the influence to the vertex from the first bone
    float4 normal = inp.weight0 * mul(float4(inp.norm, 0.0f), FinalBonesMatrices[inp.boneIndex[0]]);
	
	//add the influence to the vertex from the second bone
    normal += weight1 * mul(float4(inp.norm, 0.0f), FinalBonesMatrices[inp.boneIndex[1]]);
	
	//normal will be used as a vector in 3d space, this means that we need to set the w component to 0
	//so when we perform translations, the vector wont move
    normal.w = 0.0f;

	o.norm = mul(normal, WVP);
	o.pos  = mul(pos, WVP);
	o.tex  = inp.tex;

    return o;
}

float4 SkinnedMeshPS(VS_OUTPUT inp) : COLOR
{
	inp.norm = normalize(inp.norm);
	
	float lightAmount = saturate(dot(light.lightVector, inp.norm));
	
	float3 diffuse = lightAmount*(mtrl.diffuse*light.diffuse);
	float3 ambient = mtrl.ambient*light.ambient;

	float4 texColor = tex2D(TexS, inp.tex);
	
	float3 color;
	if( !picked )
	{
		color = (ambient + diffuse)*texColor;
	}
	else
	{
		color = (ambient + diffuse*5.0)*texColor;
	}

  	return float4(color, mtrl.diffuse.a*texColor.a);
    
}


technique SkinnedMeshTech
{
	pass P0
	{
		vertexShader = compile vs_2_0 SkinnedMeshVS();
        pixelShader  = compile ps_2_0 SkinnedMeshPS();

		//FillMode          = WireFrame;
	}
}