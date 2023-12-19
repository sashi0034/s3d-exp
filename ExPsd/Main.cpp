# include <Siv3D.hpp> // Siv3D v0.6.12

#include "PsdReader.h"
#include "PsdSample.h"

using namespace ExPsd;

void Main()
{
	Scene::SetBackground(ColorF{0.3});

	// int result = StartSample();
	// Print(result);

	PsdReader psdReader{};
	psdReader.ReadPsd();
	const auto textures = psdReader.Textures();

	int mode = 0;
	int textureIndex = 0;

	Camera2D camera2D{};

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
					textureIndex = (textureIndex + 1) % textures.size();
				}

				(void)textures[textureIndex].draw();
				break;
			case 1:
				for (auto&& t : textures) (void)t.draw();
				break;
			}
		}

		if (MouseR.down()) mode = (mode + 1) % 2;
	}
}
