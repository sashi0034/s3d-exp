#include "stdafx.h"

namespace
{
	struct IllustMotionVsCb
	{
		int divideCount;
		int padding_0x20[3];
		Float4 meshGridOffset[32 * 32];
	};

	size_t getMeshGridIndex(size_t divideCount, int x, int y)
	{
		return x + y * (divideCount + 1);
	}
}

void Main()
{
	const Texture mainTexture(Emoji(U"🐟"));
	constexpr size_t divideCount = 8 - 1;
	constexpr Vec2 drawSize{120, 120};

	const FilePath shaderPath{U"asset/illust_motion.hlsl"};
	const VertexShader vs = HLSL{shaderPath};
	const PixelShader ps = HLSL{shaderPath};
	ConstantBuffer<IllustMotionVsCb> illustMotionCb{};
	illustMotionCb->divideCount = divideCount;

	Array<Vec2> baseMeshGrid{};
	baseMeshGrid.resize(8 * 8);

	for (int x = 0; x <= divideCount; ++x)
	{
		for (int y = 0; y <= divideCount; ++y)
		{
			baseMeshGrid[getMeshGridIndex(divideCount, x, y)] = Vec2{x, y} * drawSize / divideCount;
			illustMotionCb->meshGridOffset[getMeshGridIndex(divideCount, x, y)] =
				Float4(baseMeshGrid[getMeshGridIndex(divideCount, x, y)], Vec2{});
		}
	}

	while (System::Update())
	{
		for (int x = 0; x <= divideCount; ++x)
		{
			for (int y = 0; y <= divideCount; ++y)
			{
				const Vec2 delta = Circular(
					4 + 2 * Periodic::Sine1_1(1.0s),
					ToRadians(getMeshGridIndex(divideCount, x, y) * 30 + 60 * Scene::Time()));
				illustMotionCb->meshGridOffset[getMeshGridIndex(divideCount, x, y)] =
					Float4(baseMeshGrid[getMeshGridIndex(divideCount, x, y)] + delta,
					       Vec2{});
			}
		}

		// シェーダー設定
		const ScopedCustomShader2D shader{vs, ps};

		Graphics2D::SetVSConstantBuffer(1, illustMotionCb);
		Graphics2D::SetPSTexture(0, mainTexture);

		Graphics2D::DrawTriangles(divideCount * divideCount * 2);
	}
}
