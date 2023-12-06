# include <Siv3D.hpp> // OpenSiv3D v0.6.3


struct Particle
{
	Vec2 pos;

	Vec2 velocity;

	double startTime;

	double radius;

	double weight;

	double offset;
};

void Main()
{
	Window::Resize(1280, 720);

	// 使用する画像
	const Image image{U"example/siv3d-kun.png"};

	// テクスチャ
	const Texture texture{image};
	// アルファ値 1 以上の領域を Polygon 化
	const Polygon polygon = image.alphaToPolygon(1, AllowHoles::No);

	// Polygon 単純化時の基準距離（ピクセル）
	double maxDistance = 4.0;

	// 単純化した Polygon
	Polygon simplifiedPolygon = polygon.simplified(maxDistance);

	simplifiedPolygon.moveBy(100, 100);

	PerlinNoise noise;


	Array<Particle> particles;


	const double lifeTime = 2.0;


	const Size sceneSize{Scene::Size()};
	RenderTexture gaussianA1{sceneSize}, gaussianB1{sceneSize};
	RenderTexture gaussianA4{sceneSize / 4}, gaussianB4{sceneSize / 4};
	RenderTexture gaussianA8{sceneSize / 8}, gaussianB8{sceneSize / 8};

	double a1 = 1.0, a4 = 1.0, a8 = 1.0;

	auto drawFunction = [&]()
	{
		for (const auto& p : particles)
		{
			const double t = (Scene::Time() - p.startTime) / lifeTime;
			const double alpha = Sin(t * Math::Pi) * Sin(t * Math::Pi);
			const double wave = (Sin((t + p.offset) * Math::TwoPi * 5)) * 1.0;

			Circle(p.pos, p.radius).draw(ColorF(1.0, 0.5, 0.1, alpha + alpha * wave));
		}
	};

	while (System::Update())
	{
		if (MouseL.down())
		{
			Array<Particle> newParticles(1000);

			const double direction = Random() * Math::TwoPi;

			for (auto& particle : newParticles)
			{
				particle.pos = Cursor::Pos() + RandomVec2(Circle(30.0));

				particle.velocity = (Vec2::Right().rotated(direction) + RandomVec2(Circle(0.2))) * 10.0;

				particle.startTime = Scene::Time();

				particle.radius = Random(1.0, 3.0);

				particle.weight = Random();

				particle.offset = Random();
			}

			std::copy(newParticles.begin(), newParticles.end(), std::back_inserter(particles));
		}


		Erase_if(particles, [](const Particle& p) { return p.startTime + lifeTime < Scene::Time(); });

		{
			const double delta = 0.1;

			for (auto& particle : particles)
			{
				const Vec2& p = particle.pos;
				const double x0 = noise.normalizedOctave2D0_1(p / 128.0, 1, 0.0);
				const double x1 = noise.normalizedOctave2D0_1((p + Vec2(delta, 0.0)) / 128.0, 1, 0.0);
				const double y0 = noise.normalizedOctave2D0_1(p / 128.0, 1, 0.0);
				const double y1 = noise.normalizedOctave2D0_1((p + Vec2(0.0, delta)) / 128.0, 1, 0.0);
				const double distance = (1.0 - 1.0 / (Max(1.0, Pow(Geometry2D::Distance(p, simplifiedPolygon), 1.0))));
				const Vec2 curl = distance * Vec2((y1 - y0) / delta, -(x1 - x0) / delta);
				const Vec2 add = curl * Scene::DeltaTime() * 50000;

				particle.velocity += -10 * (1.0 - particle.weight * 0.5) * particle.velocity * Scene::DeltaTime();

				particle.pos += particle.velocity * distance + add * particle.velocity * 0.5 + add * 0.5;
			}
		}

		drawFunction();

		texture.draw(100, 100);

		{
			// ガウスぼかし用テクスチャにもう一度シーンを描く
			{
				const ScopedRenderTarget2D target{gaussianA1.clear(ColorF{0.0})};
				const ScopedRenderStates2D blend{BlendState::Additive};

				drawFunction();
			}

			// オリジナルサイズのガウスぼかし (A1)
			// A1 を 1/4 サイズにしてガウスぼかし (A4)
			// A4 を 1/2 サイズにしてガウスぼかし (A8)
			Shader::GaussianBlur(gaussianA1, gaussianB1, gaussianA1);
			Shader::Downsample(gaussianA1, gaussianA4);
			Shader::GaussianBlur(gaussianA4, gaussianB4, gaussianA4);
			Shader::Downsample(gaussianA4, gaussianA8);
			Shader::GaussianBlur(gaussianA8, gaussianB8, gaussianA8);
		}

		{
			const ScopedRenderStates2D blend{BlendState::Additive};

			if (a1)
			{
				gaussianA1.resized(sceneSize).draw(ColorF{a1});
			}

			if (a4)
			{
				gaussianA4.resized(sceneSize).draw(ColorF{a4});
			}

			if (a8)
			{
				gaussianA8.resized(sceneSize).draw(ColorF{a8});
			}
		}
	}
}
