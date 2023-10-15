# include "stdafx.h" // OpenSiv3D v0.6.10

#include "Firework.h"
#include "ParticleEffect.h"
#include "StartEffect.h"
#include "TouchEffect.h"
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

	const Texture particle{U"example/particle.png", TextureDesc::Mipped};

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

		if (Key5.down()) additiveEffect.add<VerticalFlare>(Cursor::PosF(), particle);
	}
}
