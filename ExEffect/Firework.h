#pragma once

namespace ExEffect
{
	// 重力加速度
	constexpr Vec2 FireworkGravity{0, 240};

	// 火花の状態
	struct FireworkFire
	{
		// 初速
		Vec2 v0;

		// 色相のオフセット
		double hueOffset;

		// スケーリング
		double scale;

		// 破裂するまでの時間
		double nextFireSec;

		// 破裂して子エフェクトを作成したか
		bool hasChild = false;
	};

	// 火花エフェクト
	struct Firework : IEffect
	{
		// 火花の個数
		static constexpr int32 FireCount = 12;

		// 循環参照を避けるため、IEffect の中で Effect を持つ場合、参照またはポインタにすること
		const Effect& m_parent;

		// 花火の中心座標
		Vec2 m_center;

		// 火の状態
		std::array<FireworkFire, FireCount> m_fires;

		// 何世代目？ [0, 1, 2]
		int32 m_n;

		Firework(const Effect& parent, const Vec2& center, int32 n, const Vec2& v0)
			: m_parent{parent}, m_center{center}, m_n{n}
		{
			for (auto i : step(FireCount))
			{
				const double angle = (i * 30_deg + Random(-10_deg, 10_deg));
				const double speed = (60.0 - m_n * 15) * Random(0.9, 1.1) * (IsEven(i) ? 0.5 : 1.0);
				m_fires[i].v0 = Circular{speed, angle} + v0;
				m_fires[i].hueOffset = Random(-10.0, 10.0) + (IsEven(i) ? 15 : 0);
				m_fires[i].scale = Random(0.8, 1.2);
				m_fires[i].nextFireSec = Random(0.7, 1.0);
			}
		}

		bool update(double t) override
		{
			for (const auto& fire : m_fires)
			{
				const Vec2 pos = m_center + fire.v0 * t + 0.5 * t * t * FireworkGravity;
				pos.asCircle((10 - (m_n * 3)) * ((1.5 - t) / 1.5) * fire.scale)
				   .draw(HSV{10 + m_n * 120.0 + fire.hueOffset, 0.6, 1.0 - m_n * 0.2});
			}

			if (m_n < 2) // 0, 1 世代目なら
			{
				for (auto& fire : m_fires)
				{
					if (!fire.hasChild && (fire.nextFireSec <= t))
					{
						// 子エフェクトを作成
						const Vec2 pos = m_center + fire.v0 * t + 0.5 * t * t * FireworkGravity;
						m_parent.add<Firework>(m_parent, pos, (m_n + 1), fire.v0 + (t * FireworkGravity));
						fire.hasChild = true;
					}
				}
			}

			return (t < 1.5);
		}
	};

	// 打ち上げエフェクト
	struct FirstFirework : IEffect
	{
		// 循環参照を避けるため、IEffect の中で Effect を持つ場合、参照またはポインタにすること
		const Effect& m_parent;

		// 打ち上げ位置
		Vec2 m_start;

		// 打ち上げ初速
		Vec2 m_v0;

		FirstFirework(const Effect& parent, const Vec2& start, const Vec2& v0)
			: m_parent{parent}, m_start{start}, m_v0{v0}
		{
		}

		bool update(double t) override
		{
			const Vec2 pos = m_start + m_v0 * t + 0.5 * t * t * FireworkGravity;
			Circle{pos, 6}.draw();
			Line{m_start, pos}.draw(LineStyle::RoundCap, 8, ColorF{0.0}, ColorF{1.0 - (t / 0.6)});

			if (t < 0.6)
			{
				return true;
			}
			else
			{
				// 終了間際に子エフェクトを作成
				const Vec2 velocity = m_v0 + t * FireworkGravity;
				m_parent.add<Firework>(m_parent, pos, 0, velocity);
				return false;
			}
		}
	};
}
