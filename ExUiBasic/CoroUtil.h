#pragma once
#include "CoroActor.h"

namespace ExUiBasic
{
	namespace CoroUtil
	{
		CoroTask WaitForTime(CoroTaskYield& yield, double seconds);
		CoroTask WaitForTrue(CoroTaskYield& yield, const std::function<bool()>& discriminant);
		CoroTask WaitForFalse(CoroTaskYield& yield, const std::function<bool()>& discriminant);
		CoroTask WaitForCoro(CoroTaskYield& yield, const CoroActor& coro);
	};
}
