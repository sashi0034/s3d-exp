#include "stdafx.h"
#include "ItemContainer.h"
#include "ItemButton.h"

class ExUiBasic::ItemContainer::Impl
{
public:
	void Update()
	{
		const auto center = Point{Scene::Center().x, 60};

		for (int i = 0; i < m_items.size(); ++i)
		{
			m_items[i].Update({
				.index = (i + 1),
				.font = m_font20,
				.center = center.movedBy({(i - m_items.size() / 2) * 80, 0}),
				.icon = m_icon1
			});
		}
	}

private:
	std::array<ItemButton, 9> m_items;
	Font m_font20{20};
	Texture m_icon1{Emoji(U"🍎")};
};

namespace ExUiBasic
{
	ItemContainer::ItemContainer() :
		p_impl(std::make_shared<Impl>())
	{
	}

	void ItemContainer::Update()
	{
		p_impl->Update();
	}
}
