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

			const auto offset = clip + dir * 120 * rate;
			m_texture(Rect{m_region.pos + clip, m_region.size / 2})
				.drawAt(m_center - m_region.size / 4 + offset);
		}
	};

	std::unique_ptr<IEffect> MakeFragmentTextureEffect(const Vec2& center, const Texture& texture, const Rect& region)
	{
		return std::make_unique<FragmentTextureEffect>(center, texture, region);
	}
}
