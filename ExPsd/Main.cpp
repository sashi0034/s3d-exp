# include <Siv3D.hpp> // Siv3D v0.6.12

#include "PsdReader.h"
#include "PsdSample.h"

using namespace ExPsd;

void Main()
{
	Window::SetTitle(U"ExPsd");

	Window::SetStyle(WindowStyle::Sizable);
	Scene::Resize(720, 1280);
	Scene::SetBackground(ColorF{0.3});

	// int result = StartSample();
	// Print(result);

	Stopwatch sw{};
	sw.start();
	PsdReader psdReader{
		{
			.storeTarget = StoreTarget::Image
		}
	};
	auto psdObject = psdReader.getObject();
	sw.pause();
	Console.writeln(U"Passed: {}"_fmt(sw.sF()));

	int mode = 0;
	int textureIndex = 0;

	Camera2D camera2D{psdObject.documentSize / 2};

	while (System::Update())
	{
		camera2D.update();
		{
			Transformer2D t{camera2D.createTransformer()};

			switch (mode)
			{
			case 0:
				if (MouseL.down())
				{
					textureIndex = (textureIndex + 1) % psdObject.layers.size();
				}
				(void)psdObject.layers[textureIndex].texture.draw();
				break;
			case 1:
				psdObject.draw();
				break;
			default:
				break;
			}
		}

		SimpleGUI::Headline(U"Mode: {}"_fmt(mode), Rect(Scene::Size()).tr().movedBy(-150, 50));
		SimpleGUI::Headline(U"All layers: {}"_fmt(
			                    psdObject.layers.size()), Rect(Scene::Size()).tr().movedBy(-150, 100));

		if (MouseR.down()) mode = (mode + 1) % 2;
	}
}
