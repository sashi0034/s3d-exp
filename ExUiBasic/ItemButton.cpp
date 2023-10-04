#include "stdafx.h"
#include "ItemButton.h"

class ExUiBasic::ItemButton::Impl
{
public:
	void Update(const ItemButtonParam& param)
	{
		constexpr int w = 60;
		constexpr int r = 10;

		RoundRect{param.center - Point{w / 2, w / 2}, w, w, r}
			.drawShadow(Vec2{6, 6}, 24, 3)
			.draw(Color{U"#3b3b3b"});
		const auto textPos = param.center - Point{w / 3, w / 2};
		Circle(textPos, w / 4)
			.drawShadow(Vec2{2, 2}, 8, 2)
			.draw(Color{U"#404040"});
		(void)param.font(U"{}"_fmt(param.index)).drawAt(textPos);
		(void)param.icon.resized(Vec2{w, w} * 0.8f).drawAt(param.center);
	}

private:
};

namespace ExUiBasic
{
	ItemButton::ItemButton() :
		p_impl(std::make_shared<Impl>())
	{
	}

	void ItemButton::Update(const ItemButtonParam& param)
	{
		p_impl->Update(param);
	}
}
