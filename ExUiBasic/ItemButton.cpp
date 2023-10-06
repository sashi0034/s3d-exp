#include "stdafx.h"
#include "ItemButton.h"

#include "CoroUtil.h"
#include "EasingAnimation.h"

class ExUiBasic::ItemButton::Impl
{
public:
	void Tick(ActorBase& self, const ItemButtonParam& param)
	{
		const int w = 60 * m_scale;
		constexpr int r = 10;

		const auto rect = RoundRect{param.center - Point{w / 2, w / 2}, w, w, r};
		const bool entered = rect.rect.mouseOver();
		(void)rect
		      .drawShadow(Vec2{6, 6}, 24, 3)
		      .draw(Color{U"#3b3b3b"}.lerp(Palette::White, entered ? 0.7 : 0.0));
		const auto textPos = param.center - Point{w / 3, w / 2};
		Circle(textPos, w / 4)
			.drawShadow(Vec2{2, 2}, 8, 2)
			.draw(Color{U"#404040"});
		(void)param.font(U"{}"_fmt(param.index)).drawAt(textPos);
		(void)param.icon.resized(Vec2{w, w} * 0.8f).drawAt(
			param.center, ColorF(entered ? 0.7 : 1.0));

		if (entered && not m_enteredBefore)
		{
			m_scale = 1;
			AnimateEasing<BoomerangParabola>(self, &m_scale, 1.1, 0.2);
		}
		else if (not entered && m_enteredBefore)
		{
			m_scale = 1;
			AnimateEasing<BoomerangParabola>(self, &m_scale, 0.9, 0.2);
		}
		m_enteredBefore = entered;
	}

private:
	bool m_enteredBefore{};
	double m_scale{1};
};

namespace ExUiBasic
{
	ItemButton::ItemButton() :
		p_impl(std::make_shared<Impl>())
	{
	}

	void ItemButton::Tick(const ItemButtonParam& param)
	{
		ActorBase::Update();
		p_impl->Tick(*this, param);
	}
}
