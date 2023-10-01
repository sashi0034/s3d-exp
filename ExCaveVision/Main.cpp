#include "stdafx.h" // OpenSiv3D v0.6.10

struct CaveVisionCb
{
	float animRate = 0;
};

struct SoftShapeCb
{
	float time;
	Float2 centerPos;
};

void Main()
{
	Scene::SetBackground(ColorF{0.7, 0.7, 0.7});

	const Texture tex_aqua_noise{U"asset/tex_aqua_noise.png"};
	const Texture tex_cosmos_noise{U"asset/tex_cosmos_noise.png"};
	const PixelShader caveVisionPs = HLSL{U"asset/cave_vision.hlsl", U"PS"};
	const VertexShader softShapeVs = HLSL{U"asset/soft_shape.hlsl"};
	ConstantBuffer<CaveVisionCb> caveVisionCb{};
	ConstantBuffer<SoftShapeCb> softShapeCb{};
	RenderTexture maskTexture{Scene::Size(), ColorF{1.0}};

	while (System::Update())
	{
		// マスク描画
		{
			ScopedRenderTarget2D target{maskTexture};
			softShapeCb->time += Scene::DeltaTime() * 2;
			softShapeCb->centerPos = Scene::Center();
			maskTexture.clear(Color(0, 0, 0, 255));
			Graphics2D::SetVSConstantBuffer(1, softShapeCb);
			const ScopedCustomShader2D shader{softShapeVs};

			Graphics2D::DrawTriangles(360);
		}

		// スクリーン描画
		{
			constexpr float animSpeed = 0.3f;
			caveVisionCb->animRate += Scene::DeltaTime() * animSpeed;
			Graphics2D::SetPSConstantBuffer(1, caveVisionCb);
			const ScopedCustomShader2D shader{caveVisionPs};

			Graphics2D::SetPSTexture(1, tex_aqua_noise);
			Graphics2D::SetPSTexture(2, tex_cosmos_noise);
			Graphics2D::SetPSTexture(3, maskTexture);

			(void)maskTexture({0, 0}, Scene::Size()).draw(Color(U"#333333FF"));
		}
	}
}
