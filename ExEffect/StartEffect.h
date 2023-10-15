#pragma once

namespace ExEffect
{
	struct StarEffect : IEffect
	{
		static constexpr Vec2 Gravity{0, 160};

		struct Star
		{
			Vec2 start;
			Vec2 velocity;
			ColorF color;
		};

		Array<Star> m_stars;

		StarEffect(const Vec2& pos, double baseHue)
		{
			for (int32 i = 0; i < 6; ++i)
			{
				const Vec2 velocity = RandomVec2(Circle{60});
				Star star{
					.start = (pos + velocity),
					.velocity = velocity,
					.color = HSV{baseHue + Random(-20.0, 20.0)},
				};
				m_stars << star;
			}
		}

		bool update(double t) override
		{
			t /= 0.4;

			for (auto& star : m_stars)
			{
				const Vec2 pos = star.start
					+ star.velocity * t + 0.5 * t * t * Gravity;

				const double angle = (pos.x * 3_deg);

				Shape2D::Star((30 * (1.0 - t)), pos, angle)
					.draw(star.color);
			}

			return (t < 1.0);
		}
	};
}
