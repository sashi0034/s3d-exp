#include "stdafx.h"
#include "ItemContainer.h"

#include "ActorContainer.h"
#include "CoroActor.h"
#include "CoroTask.h"
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

	void startAnim(CoroTaskYield& yield, ImplState& state)
	{
		Print(U"1");
		for (int y = 0; y < 100; ++y)
		{
			state.transform = state.transform.translated({0, -1});
			yield();
		}

		Print(U"2");
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
			self.AsParent().BirthAs(CoroActor([&](auto&& yield)
			{
				startAnim(yield, state);
			}));
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
