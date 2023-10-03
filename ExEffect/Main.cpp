# include "stdafx.h" // OpenSiv3D v0.6.10

template <double easing(double)>
class ParamEasing : public IEffect
{
public:
	explicit ParamEasing(double* valuePtr, double endValue, double duration):
		m_valuePtr(valuePtr),
		m_startValue(*valuePtr),
		m_endValue(endValue),
		m_duration(duration)
	{
	}

	bool update(double timeSec) override
	{
		const double e = easing(timeSec / m_duration);
		*m_valuePtr = m_startValue * (1 - e) + m_endValue * e;
		return timeSec < m_duration;
	}

private:
	double* m_valuePtr;
	double m_startValue;
	double m_endValue;
	double m_duration;
};

void Main()
{
	Scene::SetBackground(ColorF{0.7, 0.7, 0.7});
	const Effect effect{};

	const Texture emoji{U"📺"_emoji};
	double scale = 1.0f;

	while (System::Update())
	{
		effect.update();

		if (MouseL.down())
		{
			scale = 1.0;
			effect.add<ParamEasing<EaseOutBack>>(&scale, 2.0, 1.3);
		}

		// プレイヤーを描く | Draw the player
		emoji.scaled(scale).drawAt(Scene::Center());
	}
}
