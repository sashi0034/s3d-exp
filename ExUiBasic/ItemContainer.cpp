#include "stdafx.h"
#include "ItemContainer.h"

#include "ActorContainer.h"
#include "CoroActor.h"
#include "CoroTask.h"
#include "CoroUtil.h"
#include "ItemButton.h"

namespace ExUiBasic
{
	struct ImplState
	{
		std::array<ItemButton, 9> items;
		Font font20{20};
		Texture icon1{Emoji(U"🍎")};
		Mat3x2 transform{Mat3x2::Identity()};
	};

	void startAnim(YieldExtended yield, ImplState& state)
	{
		for (int y = 0; y < 100; ++y)
		{
			state.transform = state.transform.translated({0, -1});
			yield();
		}

		yield.WaitForTime(1.0);

		for (int y = 0; y < 100; ++y)
		{
			state.transform = state.transform.translated({0, 1});
			yield();
		}
	}

	void update(ActorBase& self, ImplState& state)
	{
		const Transformer2D transform{state.transform};
		const auto center = Point{Scene::Center().x, 60};

		for (int i = 0; i < state.items.size(); ++i)
		{
			state.items[i].Update({
				.index = (i + 1),
				.font = state.font20,
				.center = center.movedBy({(i - state.items.size() / 2) * 80, 0}),
				.icon = state.icon1
			});
		}

		if (MouseL.down())
		{
			StartCoro(self, [&](CoroTaskYield& yield)
			{
				startAnim(YieldExtended(yield), state);
			});
		}
	}

	struct ItemContainer::Impl : ImplState
	{
	};

	ItemContainer::ItemContainer() :
		p_impl(std::make_shared<Impl>())
	{
	}

	void ItemContainer::Update()
	{
		ActorBase::Update();
		update(*this, *p_impl);
	}
}
