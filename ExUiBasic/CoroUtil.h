#pragma once
#include "ActorContainer.h"
#include "CoroActor.h"

namespace ExUiBasic
{
	inline CoroActor StartCoro(ActorBase& parent, const CoroTaskFunc& coro)
	{
		return parent.AsParent().Birth(CoroActor(coro));
	}

	class YieldExtended : Uncopyable
	{
	public:
		YieldExtended(CoroTaskYield& y) : yield(y) { return; }

		void WaitForTime(double seconds);
		void WaitForTrue(const std::function<bool()>& discriminant);
		void WaitForFalse(const std::function<bool()>& discriminant);
		void WaitForCoro(const CoroActor& coro);
		void operator()() const { yield(); }

	private:
		CoroTaskYield& yield;
	};;
}
