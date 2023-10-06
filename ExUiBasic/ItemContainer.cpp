#include "stdafx.h"
#include "ItemContainer.h"

#include "ActorContainer.h"
#include "CoroActor.h"
#include "CoroTask.h"
#include "CoroUtil.h"
#include "ItemButton.h"
#include "EasingAnimation.h"

namespace ExUiBasic
{
	struct ImplState
	{
		std::array<ItemButton, 9> items;
		Font font20{20};
		Texture icon1{Emoji(U"🍎")};
		Vec2 offset{};
		double scale = 1.0;
		CoroActor animation{};
		ItemLabel itemLabel{};
	};

	void startAnim(YieldExtended yield, ActorBase& self, ImplState& state)
	{
		StartCoro(self, [&](YieldExtended yield1)
		{
			yield1.WaitForDead(
				AnimateEasing<EaseOutBack>(self, &state.scale, 2.0, 0.6));
			yield1.WaitForDead(
				AnimateEasing<EaseInBack>(self, &state.scale, 1.0, 0.6));
		});

		yield.WaitForDead(
			AnimateEasing<EaseOutBack>(self, &state.offset, {0, 80}, 0.6));
		yield.WaitForDead(
			AnimateEasing<EaseOutBack>(self, &state.offset, {0, 0}, 0.6));
	}

	void update(ActorBase& self, ImplState& state)
	{
		const auto center = Point{Scene::Center().x, 60};
		const Transformer2D transform{Mat3x2::Translate(state.offset).scaled(state.scale, center)};

		for (int i = 0; i < state.items.size(); ++i)
		{
			state.items[i].Tick({
				.label = &state.itemLabel,
				.index = (i + 1),
				.font = state.font20,
				.center = center.movedBy({(i - state.items.size() / 2) * 80, 0}),
				.icon = state.icon1
			});
		}

		state.itemLabel.SetCenter(center + Point{0, 80});
		state.itemLabel.Update();

		if (KeyZ.down() && state.animation.IsDead())
		{
			state.animation = StartCoro(self, [&](CoroTaskYield& yield)
			{
				startAnim(yield, self, state);
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
