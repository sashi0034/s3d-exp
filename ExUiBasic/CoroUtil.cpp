#include "stdafx.h"
#include "CoroUtil.h"

namespace ExUiBasic
{
	CoroTask YieldExtended::WaitForTime(double seconds)
	{
		double totalTime = 0;
		while (totalTime < seconds)
		{
			totalTime += Scene::DeltaTime();
			yield();
		}
	}

	CoroTask YieldExtended::WaitForTrue(const std::function<bool()>& discriminant)
	{
		while (!discriminant())
		{
			yield();
		}
	}

	CoroTask YieldExtended::WaitForFalse(const std::function<bool()>& discriminant)
	{
		while (discriminant())
		{
			yield();
		}
	}

	CoroTask YieldExtended::WaitForCoro(const CoroActor& coro)
	{
		WaitForTrue([coro]() { return coro.IsDead(); });
	}
}
