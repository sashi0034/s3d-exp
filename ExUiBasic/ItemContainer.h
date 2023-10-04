#pragma once

namespace ExUiBasic
{
	class ItemContainer
	{
	public:
		ItemContainer();
		void Update();

	private:
		class Impl;
		std::shared_ptr<Impl> p_impl;
	};
}
