# include <Siv3D.hpp> // Siv3D v0.6.15

#define DISCORD (void)

namespace
{
	constexpr double gaugeWidth = 1000;
	constexpr double gaugeHeight = 40;

	constexpr double gaugeHalfH = gaugeHeight / 2;

	constexpr double outerLeftX = -80;
	constexpr double innerLeftX = -40;

	constexpr ColorF sideEndsGray{0.5};

	/// @brief 体幹ゲージの描画
	void DrawPostureGauge(double postureRate)
	{
		constexpr Triangle outerTri =
			Triangle::FromPoints(Vec2{outerLeftX, 0}, Vec2{0, -gaugeHalfH}, Vec2{0, gaugeHalfH});
		constexpr Triangle innerTri =
			Triangle::FromPoints(Vec2{innerLeftX, 0}, Vec2{0, -gaugeHalfH}, Vec2{0, gaugeHalfH});

		const ColorF transparentBgColor = ColorF{0.1, 0.3}.lerp(ColorF(0.8, 0.2, 0.1, 0.5), postureRate);

		// 背景の半透明部分を描画 (外側)
		{
			const auto c = transparentBgColor;
			const double w = gaugeWidth * 1.02;
			constexpr double sy = 1.5f;

			DISCORD RectF(Arg::center = Vec2{0, 0}, w, gaugeHeight).scaled(1, sy).draw(c);

			DISCORD outerTri.scaled(3, sy).movedBy(-w / 2, 0).draw(c);
			DISCORD outerTri.scaled(-3, sy).movedBy(w / 2, 0).draw(c);
		}

		// 背景の半透明部分を描画 (内側)
		{
			const ColorF c = transparentBgColor.withA(0.3);
			const double w = gaugeWidth * 0.96;
			constexpr double sy = 0.8f;

			DISCORD RectF(Arg::center = Vec2{0, 0}, w, gaugeHeight).scaled(1, sy).draw(c);

			DISCORD innerTri.scaled(0.7, sy).movedBy(-w / 2, 0).draw(c);
			DISCORD innerTri.scaled(-0.7, sy).movedBy(w / 2, 0).draw(c);
		}

		// 両端の参加
		{
			const Polygon sideEndL = Geometry2D::Subtract(outerTri.asPolygon(), innerTri.asPolygon()).front();
			sideEndL.movedBy(-gaugeWidth / 2, 0).draw(ColorF{sideEndsGray});

			const Polygon sideEndR = sideEndL.scaled(-1, 1);
			sideEndR.movedBy(gaugeWidth / 2, 0).draw(ColorF{sideEndsGray});
		}

		// バーの描画
		if (postureRate > 0)
		{
			const Color barColor = ColorF{0.7, 0.7, 0.3}.lerp(ColorF(0.9, 0.6, 0.1), postureRate);;
			const double barWidth = gaugeWidth * postureRate;
			DISCORD RectF::FromPoints(Vec2{-barWidth / 2, -gaugeHalfH}, Vec2{barWidth / 2, gaugeHalfH})
				.draw(barColor);

			DISCORD innerTri.movedBy(-barWidth / 2, 0).draw(barColor);
			DISCORD innerTri.scaled(-1, 1).movedBy(barWidth / 2, 0).draw(barColor);
		}

		// バー中心の縦棒
		{
			RectF(Arg::center = Vec2{0, 0}, 12, 8 + gaugeHalfH * 2).rounded(8).draw(ColorF{0.8}).drawShadow(
				{}, 8, 6, ColorF{0.3, 0.3});
		}
	}

	/// @brief 体幹ゲージの値
	class PostureGaugeValue
	{
	public:
		void Tick(double dt)
		{
			m_current = Math::Lerp(m_current, m_target, 1 - Math::Exp(-dt * 20));

			if (m_current < 1e-3 && m_target < 1e-3)
			{
				m_current = 0;
				m_target = 0;
			}
		}

		void Add(double delta)
		{
			m_target = Math::Clamp(m_target + delta, 0.0, 1.0);
		}

		double Current() const { return m_current; }

	private:
		double m_current{};
		double m_target{};
	};
}

void Main()
{
	Scene::SetBackground(ColorF{0.2, 0.2, 0.3});

	const Font font{FontMethod::MSDF, 48, Typeface::Medium};

	PostureGaugeValue posture{};

	while (System::Update())
	{
		posture.Tick(Scene::DeltaTime());

		if (SimpleGUI::Button(U"ヒット", Vec2{80, 40}))
		{
			posture.Add(0.1);
		}
		if (SimpleGUI::Button(U"回復", Vec2{80, 100}))
		{
			posture.Add(-0.3);
		}

		// 体幹ゲージ描画
		{
			Transformer2D t{Mat3x2::Scale(0.5f).translated(Scene::Center())};
			DrawPostureGauge(posture.Current());
		}
	}
}
