# include <Siv3D.hpp> // Siv3D v0.6.15

namespace
{
	constexpr double gaugeWidth = 200;

	constexpr ColorF sideEndsGray{ 0.5 };

	/// @brief 体幹ゲージの描画
	struct PostureGaugeDrawer
	{
		void Draw(double rate)
		{
			Triangle t1 = Triangle::FromPoints(Vec2{ -150, 0 }, Vec2{ 0, -50 }, Vec2{ 0, 50 });
			Triangle t2 = Triangle::FromPoints(Vec2{ -100, 0 }, Vec2{ 0, -50 }, Vec2{ 0, 50 });

			Polygon left = Geometry2D::Subtract(t1.asPolygon(), t2.asPolygon()).front();
			left.moveBy(-gaugeWidth, 0).draw(ColorF{ sideEndsGray });

			Polygon right = left.scale(-1, 1);
			right.moveBy(gaugeWidth, 0).draw(ColorF{ sideEndsGray });
		}
	};
}

void Main()
{
	Scene::SetBackground(ColorF{ 0.3, 0.3, 0.3 });

	const Font font{ FontMethod::MSDF, 48, Typeface::Medium };

	PostureGaugeDrawer postureGaugeDrawer{};

	while (System::Update())
	{
		{
			Transformer2D t{ Mat3x2::Scale(0.5f).translated(Scene::Center()) };
			postureGaugeDrawer.Draw(0.3f);
		}
	}
}
