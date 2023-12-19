# include <Siv3D.hpp> // Siv3D v0.6.12
#include "PsdSample.h"

void Main()
{
	Scene::SetBackground(ColorF{0.3});

	int result = StartSample();
	Print(result);

	while (System::Update())
	{
	}
}
