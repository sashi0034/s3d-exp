#pragma once

namespace ExEffect
{
	struct ItemObtainEffect2 : IEffect
	{
		Vec2 m_center{};

		explicit ItemObtainEffect2(const Vec2& center) : m_center(center)
		{
		}

		bool update(double timeSec) override
		{
			drawCircleFrame(timeSec, 0.0, 0.5, 80);
			drawCircleFrame(timeSec, 0.2, 0.5, 160);
			drawRadiation(timeSec, 0.1, 0.4, 160, 5, 0.0);
			drawRadiation(timeSec, 0.0, 0.5, 160, 5, 0.5);

			return timeSec < 0.5;
		}

		void drawCircleFrame(double timeSec, double startSec, double endSec, double r) const
		{
			if (InRange(timeSec, startSec, endSec))
			{
				const double alpha = timeSec < endSec - 0.1 ? 1.0 : 1.0 + (endSec - 0.1 - timeSec) / 0.1;
				Circle(m_center, r * (timeSec - startSec) / endSec)
					.drawFrame(10,
					           ColorF{1.0f, 0.0f}, ColorF{1.0f, 0.3f * alpha});
			}
		}

		void drawRadiation(double timeSec, double startSec, double endSec, double r, int number, double offset) const
		{
			if (InRange(timeSec, startSec, endSec) == false) return;

			for (const auto i : step(number))
			{
				const double rate = (timeSec - startSec) / endSec;
				const double alpha = timeSec < endSec - 0.1 ? 1.0 : 1.0 + (endSec - 0.1 - timeSec) / 0.1;
				Circle(m_center, r * std::min(2 * rate, 1.0))
					.drawPie(
						Math::ToRadians((i + offset) * (360.0 / number)),
						Math::ToRadians(15),
						ColorF{1.0f, 0.7f * alpha},
						ColorF{1.0f, 0.0f});
			}
		}
	};
}
