#include "stdafx.h"
#include "ImpactEffect.h"

namespace ExEffect
{
	struct ImpactEffect : IEffect
	{
		Vec2 m_center;

		explicit ImpactEffect(const Vec2& center): m_center(center)
		{
		}

		bool update(double timeSec) override
		{
			constexpr double lifetime = 0.1;
			const double scaleRate = std::max(0.0, 1 - EaseInExpo(timeSec / lifetime));

			Shape2D::NStar(8, 80 * scaleRate, 50 * scaleRate, m_center)
				.draw(Color(U"#fff156"))
				.drawFrame(4, Color(U"#4a4a4a"));
			Shape2D::NStar(6, 40 * scaleRate, 20 * scaleRate, m_center)
				.draw(Color(U"#fcfbdd"));

			return timeSec < lifetime;
		}
	};


	std::unique_ptr<IEffect> MakeImpactEffect(const Vec2& center)
	{
		return std::make_unique<ImpactEffect>(center);
	}
}
