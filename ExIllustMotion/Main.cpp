# include "stdafx.h"

namespace
{
	struct IllustMotionVsCb
	{
		int divideCount;
	};
}

void Main()
{
	const Texture mainTexture(Emoji(U"🐟"));
	constexpr size_t divideCount = 4;

	const FilePath shaderPath{U"asset/illust_motion.hlsl"};
	const VertexShader vs = HLSL{shaderPath};
	const PixelShader ps = HLSL{shaderPath};
	ConstantBuffer<IllustMotionVsCb> illustMotionCb{};
	illustMotionCb->divideCount = divideCount;

	while (System::Update())
	{
		ClearPrint();
		Print << Profiler::FPS();

		{
			// シェーダー設定
			const ScopedCustomShader2D shader{vs, ps};

			Graphics2D::SetVSConstantBuffer(1, illustMotionCb);
			Graphics2D::SetPSTexture(0, mainTexture);

			Graphics2D::DrawTriangles(divideCount * divideCount * 2);
		}
	}
}
