// Gizmo.hlsl

cbuffer GizmoBuffer : register(b0)
{
    matrix World;
    matrix ViewProjection;
    float4 Color;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

//********************************************************
// Vertex Shader
//********************************************************
PS_INPUT VS_Main(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 worldPos = mul(float4(input.Position, 1.0f), World);
    output.Position = mul(worldPos, ViewProjection);
    output.Color = Color;
    
    return output;
}

//********************************************************
// Vertex Shader
//********************************************************
float4 PS_Main(PS_INPUT input) : SV_TARGET
{
    return input.Color;
}