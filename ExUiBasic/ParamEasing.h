#pragma once
#include "ActorBase.h"

namespace ExUiBasic
{
	template <double easing(double), typename T>
	class ParamEasing : public ActorBase
	{
	public:
		explicit ParamEasing(T* valuePtr, T endValue, double duration):
			m_state(std::make_shared<State>(State{
				.valuePtr = (valuePtr),
				.startValue = (*valuePtr),
				.endValue = (endValue),
				.time = 0,
				.duration = (duration)
			}))
		{
		}

		void Update() override
		{
			m_state->time += Scene::DeltaTime();
			const double e = easing(m_state->time / m_state->duration);
			*(m_state->valuePtr) = m_state->startValue * (1 - e) + m_state->endValue * e;
			if (m_state->time >= m_state->duration) Kill();
		}

	private:
		struct State
		{
			T* valuePtr;
			T startValue;
			T endValue;
			double time;
			double duration;
		};

		std::shared_ptr<State> m_state{};
	};
}
