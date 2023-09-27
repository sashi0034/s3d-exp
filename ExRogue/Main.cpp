#include "stdafx.h" // OpenSiv3D v0.6.10

#include "DungeonGenerator.h"
#include "MapGrid.h"

using namespace ExRogue;

void Main()
{
	Scene::SetBackground(ColorF{0.3, 0.3, 0.3});

	const Texture emojiWall{U"🧱"_emoji};
	const Texture emojiFloor{U"🟦"_emoji};

	const Font font{FontMethod::MSDF, 48, Typeface::Bold};

	const MapGrid grid = GenerateFreshDungeon(DungGenProps{
		.size = {80, 80},
		.areaDivision = 8,
	});

	Camera2D camera{Scene::Center(), 1.0};

	while (System::Update())
	{
		// Transformer2D transformer{Mat3x2::Scale(1.0, 1.0).translated(Point{0, 0})};

		camera.update();
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
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
