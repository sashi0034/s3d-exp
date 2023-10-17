#include "stdafx.h"
#include "FragmentTextureEffect.h"

namespace ExEffect
{
	struct FragmentTextureEffect : IEffect
	{
		Vec2 m_center;
		Texture m_texture;
		Rect m_region;
		Array<Trail> m_trails{};

		FragmentTextureEffect(const Vec2& center, const Texture& texture, const Rect& region):
			m_center(center),
			m_texture(texture),
			m_region(region)
		{
			m_trails.resize(4, Trail(0.3));
		}

		bool update(double timeSec) override
		{
			for (auto&& trail : m_trails)
			{
				trail.draw();
				trail.update();
			}

			const auto half = m_region.size / 2;
			drawFragment(timeSec, 0, 1.0, m_trails[0], Point{}, {-1, -1});
			drawFragment(timeSec, 0, 1.0, m_trails[1], half.y0().yx(), {-1, 1});
			drawFragment(timeSec, 0, 1.0, m_trails[2], half.x0(), {1, -1});
			drawFragment(timeSec, 0, 1.0, m_trails[3], half, {1, 1});

			return timeSec < 1.0 + m_trails[0].getLifeTime();
		}

		void drawFragment(
			double timeSec, double startSec, double endSec,
			Trail& trail, const Point& clip, const Vec2& dir) const
		{
			if (InRange(timeSec, startSec, endSec) == false) return;
			const double rate = (timeSec - startSec) / (endSec - startSec);

			const auto offset1 = clip + dir * 120 * rate;
			const auto offset2 = (1 - 4 * (0.5 - rate) * (0.5 - rate)) * Vec2{0, -m_region.size.y};
			const auto scale = rate < 0.5 ? 1 : 1 - EaseInBack(2 * (rate - 0.5));
			const auto drawn = m_texture(Rect{m_region.pos + clip, m_region.size / 2})
			                   .scaled(1, scale)
			                   .drawAt(m_center - m_region.size / 4 + offset1 + offset2);

			trail.add(drawn.center().moveBy(-dir * drawn.size / 2), ColorF(0.9), 10 * scale);
		}
	};

	std::unique_ptr<IEffect> MakeFragmentTextureEffect(const Vec2& center, const Texture& texture, const Rect& region)
	{
		return std::make_unique<FragmentTextureEffect>(center, texture, region);
	}
}
