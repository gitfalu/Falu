//Basic.hlsl - 基本的な頂点・ピクセルシェーダー

//定数バッファ
cbuffer PerObjectBuffer : register(b0)
{
    matrix World;
    matrix WorldInvTranspose;
}

cbuffer PerFrameBuffer : register(b1)
{
    matrix View;
    matrix Projection;
    matrix ViewProjection;
    float4 CameraPosition;
    float4 AmbientLight;
    float Time;
    float DeltaTime;
    float2 Padding;
}

cbuffer MaterialBuffer : register(b2)
{
    float4 MaterialAlbedo;
    float4 MaterialProperties;//x: metallic, y:roughness, z: ao, w: unused
    float4 MaterialEmissive;
}

cbuffer LightBuffer : register(b3)
{
    float4 LightPosition;
    float4 LightDirection;
    float4 LightColor;
    float4 LightParams;// x: intensity, y: range, z: type, w: unused
}

//頂点シェーダー入力
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float4 Color : COLOR;
};

//ピクセルシェーダー入力
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 WorldPos : POSITIONO;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR;
};

//テクスチャとサンプラー
Texture2D AlbedoTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D MetallicTexture : register(t2);
Texture2D RoughnessTexture : register(t3);
SamplerState DefaultSampler : register(s0);


//*********************************************
//
// 頂点シェーダー
//
//*********************************************
PS_INPUT VS_Main(VS_INPUT input)
{
    PS_INPUT output;
    
    //ワールド座標変換
    float4 worldPos = mul(float4(input.Position, 1.0f), World);
    output.WorldPos = worldPos.xyz;
    
    // ビュー・プロジェクション変換
    float4 viewPos = mul(worldPos, View);
    output.Position = mul(viewPos, Projection);
    
    // 法線のワールド変換
    output.Normal = normalize(mul(input.Normal, (float3x3) WorldInvTranspose));
    
    // テクスチャ座標と頂点カラー
    output.TexCoord = input.TexCoord;
    output.Color = input.Color;
    
    return output;
}

//**********************************************
//
// ピクセルシェーダー - 基本ライティング
//
//**********************************************
float4 PS_Main(PS_INPUT input) : SV_TARGET
{
    // 法線の正規化
    float3 normal = normalize(input.Normal);
    
    // ライト方向（ディレクショナルライト）
    float3 lightDir = normalize(-LightDirection.xyz);
    
    // カメラ方向
    float3 viewDir = normalize(CameraPosition.xyz - input.WorldPos);
    
    // アルベド（拡散反射色）
    float4 albedo = MaterialAlbedo * input.Color;
    
    // テクスチャがあればサンプリング
    if(MaterialProperties.w > 0.5f)
    {
        float4 texColor = AlbedoTexture.Sample(DefaultSampler, input.TexCoord);
        albedo *= texColor;
    }
    
    // アンビエントライト
    float3 ambient = AmbientLight.rgb * AmbientLight.a * albedo.rgb;
    
    // ディフューズライティング（ランバート）
    float diffuseFactor = max(dot(normal, lightDir), 0.0f);
    float3 diffuse = diffuseFactor * LightColor.rgb * LightParams.x * albedo.rgb;
    
    // スぺキュラライティング（Blinn-Phong）
    float3 halfVector = normalize(lightDir + viewDir);
    float specularFactor = pow(max(dot(normal, halfVector), 0.0f), 32.0f);
    float3 specular = specularFactor * LightColor.rgb * LightParams.x * (1.0f - MaterialProperties.y);
    
    // 最終カラー
    float3 finalColor = ambient + diffuse + specular + MaterialEmissive.rgb;
    
    return float4(finalColor, albedo.a);
}

//**********************************************
//
// ピクセルシェーダー - 単色（デバッグ用）
//
//**********************************************
float4 PS_Unlit(PS_INPUT input) : SV_TARGET
{
    float albedo = MaterialAlbedo * input.Color;
    if(MaterialProperties.w > 0.5f)
    {
        float4 texColor = AlbedoTexture.Sample(DefaultSampler, input.TexCoord);
        albedo *= texColor;
    }
    
    return albedo;
}

//**********************************************
//
// ピクセルシェーダー - 法線可視化
//
//**********************************************
float4 PS_NormalVisualize(PS_INPUT input) : SV_TARGET
{
    float3 normal = normalize(input.Normal);
    float3 color = normal * 0.5f + 0.5f; // -1 .. 1 を 0 .. 1 に変換
    return float4(color, 1.0f);
}

//**********************************************
// 
// ピクセルシェーダー - UV可視化
// 
//**********************************************
float4 PS_UVVisualize(PS_INPUT input) : SV_TARGET
{
    return float4(input.TexCoord.x, input.TexCoord.y, 0.0f, 1.0f);
}