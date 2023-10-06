#pragma once
#include "ActorBase.h"

namespace ExUiBasic
{
	class ItemLabel;

	struct ItemButtonParam
	{
		ItemLabel* label;
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

	class ItemLabel : public ActorBase
	{
	public:
		ItemLabel();
		void Update() override;
		void SetCenter(const Vec2& point);
		void ShowMessage(const DrawableText& text);
		void HideMessage();

	private:
		struct Impl;
		std::shared_ptr<Impl> p_impl;
	};
}
