#include "stdafx.h"
#include "FragmentTextureEffect.h"

namespace ExEffect
{
	struct FragmentTextureEffect : IEffect
	{
		Vec2 m_center;
		Texture m_texture;
		Rect m_region;

		FragmentTextureEffect(const Vec2& center, const Texture& texture, const Rect& region):
			m_center(center),
			m_texture(texture),
			m_region(region)
		{
		}

		bool update(double timeSec) override
		{
			const auto half = m_region.size / 2;
			drawFragment(timeSec, 0, 1.0, Point{}, {-1, -1});
			drawFragment(timeSec, 0, 1.0, half.y0().yx(), {-1, 1});
			drawFragment(timeSec, 0, 1.0, half.x0(), {1, -1});
			drawFragment(timeSec, 0, 1.0, half, {1, 1});

			return timeSec < 1.0;
		}

		void drawFragment(
			double timeSec, double startSec, double endSec,
			const Point& clip, const Vec2& dir) const
		{
			if (InRange(timeSec, startSec, endSec) == false) return;
			const double rate = (timeSec - startSec) / (endSec - startSec);

			const auto offset1 = clip + dir * 120 * rate;
			const auto offset2 = (1 - 4 * (0.5 - rate) * (0.5 - rate)) * Vec2{0, -m_region.size.y};
			m_texture(Rect{m_region.pos + clip, m_region.size / 2})
				.scaled(1, rate < 0.5 ? 1 : 1 - EaseInBounce(2 * (rate - 0.5)))
				.draw(m_center - m_region.size / 2 + offset1 + offset2);
		}
	};

	std::unique_ptr<IEffect> MakeFragmentTextureEffect(const Vec2& center, const Texture& texture, const Rect& region)
	{
		return std::make_unique<FragmentTextureEffect>(center, texture, region);
	}
}
