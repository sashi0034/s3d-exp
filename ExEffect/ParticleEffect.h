#pragma once

namespace ExEffect
{
	struct SparkParticle
	{
		Vec2 start;
		Vec2 velocity;
	};

	struct Spark : IEffect
	{
		Array<SparkParticle> m_particles;

		explicit Spark(const Vec2& start)
			: m_particles(50)
		{
			for (auto& particle : m_particles)
			{
				particle.start = start + RandomVec2(10.0);

				particle.velocity = RandomVec2(1.0) * Random(80.0);
			}
		}

		bool update(double t) override
		{
			for (const auto& particle : m_particles)
			{
				const Vec2 pos = particle.start
					+ particle.velocity * t + 0.5 * t * t * Vec2{0, 240};

				Triangle{pos, 16.0, (pos.x * 5_deg)}.draw(HSV{pos.y - 40, (1.0 - t)});
			}

			return (t < 1.0);
		}
	};
}
