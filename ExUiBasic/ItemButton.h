#pragma once
#include "ActorBase.h"

namespace ExUiBasic
{
	struct ItemButtonParam
	{
		int index;
		Font font;
		Point center;
		Texture icon;
	};

	class ItemButton : public ActorBase
	{
	public:
		ItemButton();

		void Tick(const ItemButtonParam& param);

	private:
		class Impl;
		std::shared_ptr<Impl> p_impl;
	};
}
