#include "stdafx.h"
#include "TutorialNavigation.h"

#include "CoroUtil.h"
#include "EasingAnimation.h"
#include "TomlParametersWrapper.h"

namespace ExUiBasic
{
	namespace
	{
		struct ImplState
		{
			CoroActor performTask{};
			Texture arrow{Emoji{U"👇"}};
			Vec2 arrowPos{};
			double messageScale{};
			Font font{FontMethod::SDF, 40, Typeface::Bold};
		};
	}

	struct TutorialNavigation::Impl : ImplState
	{
	};

	TutorialNavigation::TutorialNavigation() :
		p_impl(std::make_shared<Impl>())
	{
		p_impl->font.setBufferThickness(3);
	}

	void performAsync(YieldExtended yield, ActorBase& self, ImplState& state, const Point& center)
	{
		AnimateEasing<EaseOutBack>(self, &state.messageScale, 1.0, 0.3);

		for (auto i : step(10))
		{
			state.arrowPos = center
				.movedBy(0, -state.arrow.height() / 2 - GetTomlParameter<double>(U"Ui.TutorialNavigation.arrowSpace"));
			yield.WaitForDead(AnimateEasing<BoomerangParabola>(
				self,
				&state.arrowPos,
				state.arrowPos.movedBy(Vec2{0.0, -GetTomlParameter<double>(U"Ui.TutorialNavigation.arrowSpring")}),
				GetTomlParameter<double>(U"Ui.TutorialNavigation.arrowInterval")));
		}

		yield.WaitForDead(AnimateEasing<EaseInBack>(self, &state.messageScale, 0.0, 0.3));
	}

	void TutorialNavigation::Update()
	{
		ActorBase::Update();

		if (not p_impl->performTask.IsDead())
		{
			p_impl->arrow
			      .scaled(GetTomlParameter<double>(U"Ui.TutorialNavigation.arrowScale"))
			      .drawAt(p_impl->arrowPos);
			Transformer2D t{Mat3x2::Scale({1, p_impl->messageScale})};
			const auto messageCenter = Vec2{
				Scene::Center().x, GetTomlParameter<double>(U"Ui.TutorialNavigation.messageTop")
			};
			// const auto backSize = GetTomlParameter<Vec2>(U"Ui.TutorialNavigation.messageBackSize");
			// RectF(messageCenter - backSize / 2, backSize)
			// 	.rounded(10)
			// 	.draw(Arg::top = ColorF{0.1, 0.7}, Arg::bottom = ColorF{0.1, 0.3});
			(void)p_impl->font(U"チュートリアルメッセージを表示")
			            .drawAt(TextStyle::Shadow({3, 3}, Palette::Black), messageCenter)
			            .stretched(40, 20)
			            .shearedX(20)
			            .drawFrame(3, Palette::White);;
		}

		if (MouseR.down())
		{
			p_impl->performTask = StartCoro(*this, [&](auto&& yield)
			{
				performAsync(yield, *this, *p_impl, Cursor::Pos());
			});
		}
	}
}
