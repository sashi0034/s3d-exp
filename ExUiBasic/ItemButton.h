#pragma once

namespace ExUiBasic
{
	struct ItemButtonParam
	{
		int index;
		Font font;
		Point center;
		Texture icon;
	};

	class ItemButton
	{
	public:
		ItemButton();

		void Update(const ItemButtonParam& param);

	private:
		class Impl;
		std::shared_ptr<Impl> p_impl;
	};
}
