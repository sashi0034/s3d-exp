#include "stdafx.h"

namespace
{
	struct IllustMotionVsCb
	{
		int divideCount;
		int padding_0x20[3];
		Float4 meshGridOffset[32 * 32];
	};
}

void Main()
{
	const Texture mainTexture(Emoji(U"🐟"));
	constexpr size_t divideCount = 7;
	constexpr Vec2 drawSize{120, 120};

	const FilePath shaderPath{U"asset/illust_motion.hlsl"};
	const VertexShader vs = HLSL{shaderPath};
	const PixelShader ps = HLSL{shaderPath};
	ConstantBuffer<IllustMotionVsCb> illustMotionCb{};
	illustMotionCb->divideCount = divideCount;
	for (int x = 0; x <= divideCount; ++x)
	{
		for (int y = 0; y <= divideCount; ++y)
		{
			illustMotionCb->meshGridOffset[y * (divideCount + 1) + x] =
				Float4(Vec2{x, y} * drawSize, Vec2{}) / divideCount;
		}
	}

	while (System::Update())
	{
		// シェーダー設定
		const ScopedCustomShader2D shader{vs, ps};

		Graphics2D::SetVSConstantBuffer(1, illustMotionCb);
		Graphics2D::SetPSTexture(0, mainTexture);

		Graphics2D::DrawTriangles(divideCount * divideCount * 2);
	}
}
