#pragma once

namespace ExUiBasic
{
	class CoordinateVisualizer
	{
	public:
		CoordinateVisualizer();

		struct Param
		{
			ColorF color{0.7, 0.7, 0.7};
			int gridSpace = 40;
		};

		void Update(const Param& param);

	private:
		class Impl;
		std::shared_ptr<Impl> p_impl;
	};
}
