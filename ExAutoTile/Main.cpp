# include "stdafx.h" // OpenSiv3D v0.6.10

#include "AutoTiler.h"

using namespace ExAutoTile;

void Main()
{
	Scene::SetBackground(ColorF{0.3, 0.3, 0.3});

	const Texture texture{U"asset/brick_stylish_24x24.png"};

	const Array<std::string> tileMap{
		" 11                             ",
		" 11   11        11    11111     ",
		"   11111111    1111   11111     ",
		"      11      1 11    11111     ",
		"   1     1        1111          ",
		"   1     1           1          ",
		"   1111   11         1         1",
		"                             111",
	};

	Camera2D camera{Scene::Center(), 1.0};

	while (System::Update())
	{
		camera.update();
		const auto t = camera.createTransformer();
		for (int y = 0; y < tileMap.size(); ++y)
		{
			const auto& checking = tileMap[y];
			for (int x = 0; x < checking.size(); ++x)
			{
				constexpr uint32 tileSize{24};
				(void)Rect(Point{x * tileSize, y * tileSize}, {tileSize, tileSize})
					.draw(Palette::Antiquewhite);

				constexpr char targetChar{'1'};
				if (checking[x] == targetChar)
				{
					AutoTileConnect connect{
						.connectL = x == 0 || tileMap[y][x - 1] == targetChar,
						.connectT = y == 0 || tileMap[y - 1][x] == targetChar,
						.connectR = x == checking.size() - 1 || tileMap[y][x + 1] == targetChar,
						.connectB = y == tileMap.size() - 1 || tileMap[y + 1][x] == targetChar,
					};
					const auto src = GetAutoTileSrcPoint(tileSize, connect);
					(void)texture(src, Size{tileSize, tileSize})
						.draw(Point{x * tileSize, y * tileSize});
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
