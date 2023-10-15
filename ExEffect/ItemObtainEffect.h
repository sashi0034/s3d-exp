#pragma once

namespace ExEffect
{
	struct ItemObtainEffect : IEffect
	{
		struct Particle
		{
			Circular pos{};
			double angVel{};
			double scale{};
			double delay{};
			double t{};
		};

		Texture m_texture{};
		Array<Particle> m_particles{};
		Vec2 m_center{};
		static constexpr int particleSize = 30;
		static constexpr double lifespan = 1.5;
		static constexpr double rotSpeed = 2.0;
		static constexpr double maxRadius = 40;

		explicit ItemObtainEffect(const Texture& texture, const Vec2& center) :
			m_texture(texture),
			m_center(center)
		{
			m_particles.resize(particleSize);
			for (int i = 0; i < m_particles.size(); ++i)
			{
				auto&& particle = m_particles[i];
				particle.pos.theta = i * (360 / particleSize);
				particle.delay = 0.001 * particle.pos.theta;
			}
		}

		bool update(double t) override
		{
			bool hasCompleted = true;
			for (int i = 0; i < m_particles.size(); ++i)
			{
				auto&& particle = m_particles[i];
				if (particle.t > lifespan) continue;
				hasCompleted = false;

				if (particle.delay > 0)
				{
					particle.delay -= Scene::DeltaTime();
					continue;
				}

				particle.t += Scene::DeltaTime();
				particle.angVel += Scene::DeltaTime();
				particle.pos.theta += particle.angVel;
				const double r = Math::Sin((particle.t / lifespan) * Math::Pi)
					* (0.75 + 0.25 * Math::Sin((16.0 / 7) * (particle.t / lifespan) * Math::Pi));
				particle.pos.r = maxRadius * r;
				particle.scale = r;

				(void)m_texture.scaled(particle.scale).drawAt(m_center + particle.pos.toVec2(), ColorF{1.0, 0.3});
			}
			return not hasCompleted;
		}
	};
}
