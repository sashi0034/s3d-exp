#include "stdafx.h" // OpenSiv3D v0.6.10

struct CaveVisionCb
{
	float animRate = 0;
	Float2 openCenter;
	float openRadius = 0;
};

void Main()
{
	Scene::SetBackground(ColorF{0.7, 0.7, 0.7});

	const Texture tex_aqua_noise{U"asset/tex_aqua_noise.png"};
	const Texture tex_cosmos_noise{U"asset/tex_cosmos_noise.png"};
	const PixelShader psCaveVision = HLSL{U"asset/cave_vision.hlsl", U"PS"};
	ConstantBuffer<CaveVisionCb> caveVisionCb{};

	while (System::Update())
	{
		constexpr float animSpeed = 0.3f;
		caveVisionCb->animRate += Scene::DeltaTime() * animSpeed;
		caveVisionCb->openCenter = Scene::Center();
		caveVisionCb->openRadius = 128 + 8 * std::sin(Math::Pi * caveVisionCb->animRate / animSpeed);
		caveVisionCb->openRadius = caveVisionCb->openRadius * caveVisionCb->openRadius;
		Graphics2D::SetPSConstantBuffer(1, caveVisionCb);
		const ScopedCustomShader2D shader{psCaveVision};

		Graphics2D::SetPSTexture(1, tex_aqua_noise);
		Graphics2D::SetPSTexture(2, tex_cosmos_noise);

		(void)Rect({0, 0}, Scene::Size()).draw(Color(U"#333333FF"));
	}
}
