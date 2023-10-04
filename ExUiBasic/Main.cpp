# include "stdafx.h" // OpenSiv3D v0.6.10

#include "CoordinateVisualizer.h"

using namespace ExUiBasic;

void Main()
{
	// 背景の色を設定する | Set the background color
	Scene::SetBackground(ColorF{0.3, 0.3, 0.3});

	CoordinateVisualizer coordinateVisualizer{};

	while (System::Update())
	{
		coordinateVisualizer.Update({.gridSpace = 40});
	}
}
