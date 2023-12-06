Texture2D g_texture0 : register(t0);
SamplerState g_sampler0 : register(s0);

namespace s3d
{
    //
    //  VS Input (nothing)
    //

    //
    //  VS Output
    //
    struct PSInput
    {
        float4 position : SV_POSITION;
        float4 color : COLOR0;
        float2 uv : TEXCOORD0;
    };

    //
    //  Siv3D Functions
    //
    float4 Transform2D(float2 pos, float2x4 t)
    {
        return float4((t._13_14 + (pos.x * t._11_12) + (pos.y * t._21_22)), t._23_24);
    }
}

//
//  Constant Buffer
//
cbuffer VSConstants2D : register(b0)
{
    row_major float2x4 g_transform;
    float4 g_colorMul;
}

cbuffer IllustMotionVsCb : register(b1)
{
    int g_divideCount;
}

cbuffer PSConstants2D : register(b0)
{
    float4 g_colorAdd;
    float4 g_sdfParam;
    float4 g_sdfOutlineColor;
    float4 g_sdfShadowColor;
    float4 g_internal;
}

uint2 offsetMod6(uint mod6)
{
    const float2 offsets[6] = {
        float2(0, 0),
        float2(1, 0),
        float2(0, 1),
        float2(0, 1),
        float2(1, 0),
        float2(1, 1)
    };

    return offsets[mod6];
}

// 頂点シェーダー
s3d::PSInput VS(uint id: SV_VERTEXID)
{
    s3d::PSInput result;

    // 四角形一つの頂点は6 
    const uint quadId = id / 6;
    const uint mod6 = id % 6;

    // 座標を適当に計算
    float2 pos = float2(0, 0);
    const uint xId0 = (int)(quadId % g_divideCount);
    const uint yId0 = (int)(quadId / g_divideCount);

    // uv座標を計算
    const uint2 xyId1 = uint2(xId0, yId0) + offsetMod6(mod6);
    const float step = 1.0 / g_divideCount;
    const uint2 uv = step * xyId1;

    const float2 size = float2(100, 100);
    pos += uv * size;

    result.position = s3d::Transform2D(pos, g_transform);
    result.uv = uv;
    result.color = g_colorMul;

    return result;
}

// ピクセルシェーダー
float4 PS(s3d::PSInput input) : SV_TARGET
{
    const float4 texColor = g_texture0.Sample(g_sampler0, input.uv);
    return (texColor * input.color) + g_colorAdd;
}
