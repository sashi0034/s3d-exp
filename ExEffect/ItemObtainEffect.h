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
		static constexpr int particleSize = 12;
		static constexpr double lifespan = 0.3;
		static constexpr double delayDuration = 0.1;
		static constexpr double rotationSpeed = 20.0;
		static constexpr double maxRadius = 120;
		static constexpr double scaleMultiplier = 2;

		explicit ItemObtainEffect(const Texture& texture, const Vec2& center) :
			m_texture(texture),
			m_center(center)
		{
			m_particles.resize(particleSize);
			for (int i = 0; i < m_particles.size(); ++i)
			{
				auto&& particle = m_particles[i];
				particle.pos.theta = Math::ToRadians(i * (360 / particleSize));
				particle.delay = (i % 2) * delayDuration;
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
				particle.angVel += rotationSpeed * Scene::DeltaTime();
				particle.pos.theta += Math::ToRadians(particle.angVel * particle.angVel);
				const double r = Math::Sin((particle.t / lifespan) * Math::Pi);
				particle.pos.r = maxRadius * r;
				particle.scale = r * scaleMultiplier;

				(void)m_texture.scaled(particle.scale)
				               .drawAt(
					               m_center + particle.pos.toVec2(),
					               ColorF{1.0, 1.0, 0.3 * (1 + particle.scale), 0.3});
			}
			return not hasCompleted;
		}
	};
}
