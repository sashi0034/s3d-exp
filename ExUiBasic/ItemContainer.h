#pragma once
#include "ActorBase.h"

namespace ExUiBasic
{
	class ItemContainer : public ActorBase
	{
	public:
		ItemContainer();
		void Update() override;

	private:
		struct Impl;
		std::shared_ptr<Impl> p_impl;
	};
}
