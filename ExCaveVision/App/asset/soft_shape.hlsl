namespace s3d
{
    //
    //	VS Input (nothing)
    //

    //
    //	VS Output
    //
    struct PSInput
    {
        float4 position : SV_POSITION;
        float4 color : COLOR0;
        float2 uv : TEXCOORD0;
    };

    //
    //	Siv3D Functions
    //
    float4 Transform2D(float2 pos, float2x4 t)
    {
        return float4((t._13_14 + (pos.x * t._11_12) + (pos.y * t._21_22)), t._23_24);
    }
}

//
//	Constant Buffer
//
cbuffer VSConstants2D : register(b0)
{
    row_major float2x4 g_transform;
    float4 g_colorMul;
}

cbuffer SoftShapeCb : register(b1)
{
    float g_time;
    float2 g_centerPos;
}

//
//	Functions
//
float2 Circular(float r, float theta)
{
    return float2(sin(theta), -cos(theta)) * r;
}

s3d::PSInput VS(uint id: SV_VERTEXID)
{
    s3d::PSInput result;
    result.color = float4(1, 1, 1, 1);

    float2 pos;

    if (id % 3 == 0)
    {
        pos = g_centerPos;
    }
    else
    {
        const float angle = radians((id / 3) + ((id % 3) - 1));
        const float r = 200
            + cos((angle * 2) + sin(g_time * 1.5)) * 40
            + sin((angle * 2) + sin(g_time * 2.0)) * 30
            + cos((angle * 3) + sin(g_time * 3.5)) * 20
            + sin((angle * 3) + sin(g_time * 4.0)) * 10;
        pos = g_centerPos + Circular(r, angle);
        result.color.a = (1.0 - r / 360.0);
    }

    result.position = s3d::Transform2D(pos, g_transform);
    result.uv = float2(0, 0);
    return result;
}