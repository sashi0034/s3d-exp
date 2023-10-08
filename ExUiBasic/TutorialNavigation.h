#pragma once
#include "ActorBase.h"

namespace ExUiBasic
{
	class TutorialNavigation : public ActorBase
	{
	public:
		TutorialNavigation();
		void Update() override;

	private:
		struct Impl;
		std::shared_ptr<Impl> p_impl;
	};
}
