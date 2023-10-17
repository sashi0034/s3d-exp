#include "stdafx.h"
#include "TwinkleEffect.h"

namespace
{
	struct TwinkleChild : IEffect
	{
		Vec2 pos;
		double angle;
		double scale;

		TwinkleChild(const Vec2& pos) : pos(pos), angle(Sample({0_deg, 45_deg})), scale(Random(0.2, 1.0))
		{
		}

		bool update(double t) override
		{
			scale *= 0.988;

			const double outer = (4 + 25 + 25 * Sin(t * 6_pi)) * scale;
			const double inner = (0.5 + 1 + 1 * Sin(t * 6_pi)) * scale;

			Shape2D::NStar(4, outer, inner, pos, angle).draw(Alpha(1 + 120 + 120 * Sin(t * 2_pi)));

			return t < 0.25;
		}
	};
}

namespace ExEffect
{
	struct TwinkleEffect : IEffect
	{
		const Effect& effect;
		int remaining{8};
		double accum{};
		Vec2 pos;

		explicit TwinkleEffect(const Effect& effect, const Vec2& pos) :
			effect(effect),
			pos(pos)
		{
		}

		bool update(double t) override
		{
			constexpr double interval = 0.1;

			while (remaining > 0 && t - accum > interval)
			{
				remaining--;
				accum += interval;

				effect.add<TwinkleChild>(pos + RandomVec2(Random(80.0)));
			}

			return remaining != 0;
		}
	};

	std::unique_ptr<IEffect> MakeTwinkleEffect(const Effect& effect, const Vec2& pos)
	{
		return std::make_unique<TwinkleEffect>(effect, pos);
	}
}
