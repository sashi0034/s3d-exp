#include "stdafx.h"
#include "CoordinateVisualizer.h"
#include "CoordinateVisualizer.h"

namespace ExUiBasic
{
	class CoordinateVisualizer::Impl
	{
	public:
		void Update(const Param& param)
		{
			if (KeyF2.down()) m_isShow = !m_isShow;
			if (m_isShow == false) return;

			const auto& color = param.color;
			const auto gridSpace = param.gridSpace;
			const int dotOffset = 10 * Scene::Time();
			for (int x = 0; x < Scene::Size().x; x += gridSpace)
			{
				(void)Line(x, 0, x, Scene::Size().y).draw(LineStyle::SquareDot(dotOffset), 1, color);
				(void)m_(U"{}"_fmt(x)).draw(x, 0, color);
			}
			for (int y = 0; y < Scene::Size().y; y += gridSpace)
			{
				(void)Line(0, y, Scene::Size().x, y).draw(LineStyle::SquareDot(dotOffset), 1, color);
				(void)m_(U"{}"_fmt(y)).draw(0, y, color);
			}
		}

	private:
		bool m_isShow = true;
		Font m_{10};
	};

	CoordinateVisualizer::CoordinateVisualizer() :
		p_impl(std::make_shared<Impl>())
	{
	}

	void CoordinateVisualizer::Update(const Param& param)
	{
		p_impl->Update(param);
	}
}
