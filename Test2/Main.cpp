# include <Siv3D.hpp>

void Main()
{
	Scene::SetBackground(ColorF{1.0, 0.9, 0.7});

	constexpr Vec2 pos{220, 60};

	const Image image{U"🧑🏽‍🦯"_emoji};

	const Texture texture{image};

	// 画像の輪郭から Polygon を作成する
	const Polygon polygon = image.alphaToPolygon(160, AllowHoles::No);

	// 凸包を計算する
	const Polygon convexHull = polygon.computeConvexHull();

	// Polygon を太らせる
	const Polygon largeConvex = convexHull.calculateBuffer(20);

	// 影用のテクスチャ
	const RenderTexture shadowTexture{Scene::Size(), ColorF{1.0, 0.0}};
	const RenderTexture gaussianA4{shadowTexture.size() / 4};
	const RenderTexture gaussianB4{shadowTexture.size() / 4};

	while (System::Update())
	{
		// 影の形状を描く
		{
			const ScopedRenderTarget2D target{shadowTexture.clear(ColorF{1.0, 0.0})};
			const ScopedRenderStates2D blend{BlendState::MaxAlpha};
			const Transformer2D transform{Mat3x2::Translate(6, 6)};
			largeConvex.draw(pos);
		}

		// shadowTexture をダウンサンプリング + ガウスぼかし
		{
			Shader::Downsample(shadowTexture, gaussianA4);
			Shader::GaussianBlur(gaussianA4, gaussianB4, gaussianA4);
		}

		// ぼかした影を描く
		gaussianA4.resized(Scene::Size()).draw(ColorF{0.0, 0.5});

		largeConvex.draw(pos, ColorF{0.96, 0.98, 1.0});

		texture.draw(pos);
	}
}
