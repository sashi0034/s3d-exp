# include "stdafx.h" // OpenSiv3D v0.6.10

#include "Firework.h"
#include "FragmentTextureEffect.h"
#include "ItemObtainEffect.h"
#include "ItemObtainEffect2.h"
#include "ParticleEffect.h"
#include "StartEffect.h"
#include "TouchEffect.h"
#include "TwinkleEffect.h"
#include "VerticalFlare.h"

using namespace ExEffect;

void Main()
{
	Scene::SetBackground(ColorF{0.3, 0.3, 0.3});

	Window::SetStyle(WindowStyle::Sizable);
	Scene::SetResizeMode(ResizeMode::Keep);
	Scene::Resize(1920, 1080);
	Window::Resize(1280, 720);

	const Effect effect{};
	const Effect additiveEffect{};

	TextureAsset::Register(U"example/particle.png", U"example/particle.png", TextureDesc::Mipped);
	TextureAsset::Register(U"example/texture/uv.png", U"example/texture/uv.png");

	Print(U"Effect test");

	while (System::Update())
	{
		effect.update();

		{
			const ScopedRenderStates2D blend{BlendState::Additive};
			additiveEffect.update();
		}

		if (Key1.down()) effect.add<Spark>(Cursor::Pos());

		if (Key2.down()) effect.add<StarEffect>(Cursor::Pos(), Random(0.0, 360.0));

		if (Key3.down()) additiveEffect.add<TouchEffect>(Cursor::PosF());

		if (Key4.down()) additiveEffect.add<FirstFirework>(additiveEffect, Cursor::Pos(), Vec2{0, -400});

		if (Key5.down()) additiveEffect.add<VerticalFlare>(Cursor::PosF(), TextureAsset(U"example/particle.png"));

		if (Key6.down()) additiveEffect.add<ItemObtainEffect>(TextureAsset(U"example/particle.png"), Cursor::Pos());

		if (Key7.down()) additiveEffect.add(MakeItemObtainEffect2(Cursor::Pos()));

		if (Key8.down())
			effect.add(MakeFragmentTextureEffect(
				Cursor::Pos(),
				TextureAsset(U"example/texture/uv.png"),
				{120, 120, 480, 480}));

		if (KeyQ.down())
			effect.add(MakeTwinkleEffect(effect, Cursor::Pos()));
	}
}
