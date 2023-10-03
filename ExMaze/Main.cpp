#include "stdafx.h" // OpenSiv3D v0.6.10

#include "MazeGenerator.h"

using namespace ExMaze;

void Main()
{
	Scene::SetBackground(ColorF{0.3, 0.3, 0.3});

	const Texture emojiWall{U"🧱"_emoji};
	const Texture emojiFloor{U"🟦"_emoji};
	const Texture emojiPathway{U"🟩"_emoji};

	auto mazeGenProps = MazeGenProps{
		.size = {81, 81},
	};

	MapGrid grid{{}};
	Camera2D camera{Scene::Center(), 1.0};

	while (System::Update())
	{
		camera.update();
		{
			const auto t = camera.createTransformer();

			for (int x = 0; x < grid.Data().size().x; ++x)
			{
				for (int y = 0; y < grid.Data().size().y; ++y)
				{
					auto&& cell = grid.At({x, y});
					constexpr int size = 12;
					switch (cell.kind)
					{
					case TerrainKind::Wall:
						(void)emojiWall.resized(size, size).draw(x * size, y * size);
						break;
					case TerrainKind::Floor:
						(void)emojiFloor.resized(size, size).draw(x * size, y * size);
						break;
					default: ;
					}
				}
			}
		}

		if (SimpleGUI::Button(U"生成", {20, 20}) ||
			grid.HasData() == false)
		{
			grid = GenerateFreshMaze(mazeGenProps);
		}
	}
}
