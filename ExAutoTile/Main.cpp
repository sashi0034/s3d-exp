# include "stdafx.h" // OpenSiv3D v0.6.10

#include "AutoTiler.h"

using namespace ExAutoTile;

void drawTileAt(const Array<std::string>& tileMap, int y, int x, const Texture& texture)
{
	constexpr uint32 tileSize{24};
	constexpr uint32 tileHalf{tileSize / 2};
	(void)Rect(Point{x * tileSize, y * tileSize}, {tileSize, tileSize})
		.draw(Palette::Antiquewhite);

	constexpr char targetChar{'1'};
	if (tileMap[y][x] == targetChar)
	{
		const AutoTileConnect connect{
			.connectL = x == 0 || tileMap[y][x - 1] == targetChar,
			.connectT = y == 0 || tileMap[y - 1][x] == targetChar,
			.connectR = x == tileMap[y].size() - 1 || tileMap[y][x + 1] == targetChar,
			.connectB = y == tileMap.size() - 1 || tileMap[y + 1][x] == targetChar,
		};
		const auto src = GetAutoTileSrcPoint(tileSize, connect);

		const bool isSafeRange = 0 < x && x < tileMap[y].size() - 1 && 0 < y && y < tileMap.size() - 1;
		const bool diagonalLT = isSafeRange && connect.connectL && connect.connectT &&
			tileMap[y - 1][x - 1] != targetChar;
		const bool diagonalRT = isSafeRange && connect.connectR && connect.connectT &&
			tileMap[y - 1][x + 1] != targetChar;
		const bool diagonalLB = isSafeRange && connect.connectL && connect.connectB &&
			tileMap[y + 1][x - 1] != targetChar;
		const bool diagonalRB = isSafeRange && connect.connectR && connect.connectB &&
			tileMap[y + 1][x + 1] != targetChar;

		if (diagonalLT || diagonalRT || diagonalLB || diagonalRB)
		{
			// 隅付き描画
			(void)texture(
					diagonalLT
						? GetAutoTileDiagonalSrc(tileSize, AutoTileDiagonal::LT)
						: src.movedBy(0, 0),
					Size{tileHalf, tileHalf})
				.draw(Point{x * tileSize, y * tileSize}.movedBy(0, 0));
			(void)texture(
					diagonalRT
						? GetAutoTileDiagonalSrc(tileSize, AutoTileDiagonal::RT)
						: src.movedBy(tileHalf, 0),
					Size{tileHalf, tileHalf})
				.draw(Point{x * tileSize, y * tileSize}.movedBy(tileHalf, 0));
			(void)texture(
					diagonalLB
						? GetAutoTileDiagonalSrc(tileSize, AutoTileDiagonal::LB)
						: src.movedBy(0, tileHalf),
					Size{tileHalf, tileHalf})
				.draw(Point{x * tileSize, y * tileSize}.movedBy(0, tileHalf));
			(void)texture(
					diagonalRB
						? GetAutoTileDiagonalSrc(tileSize, AutoTileDiagonal::RB)
						: src.movedBy(tileHalf, tileHalf),
					Size{tileHalf, tileHalf})
				.draw(Point{x * tileSize, y * tileSize}.movedBy(tileHalf, tileHalf));
		}
		else
		{
			// 通常描画
			(void)texture(src, Size{tileSize, tileSize})
				.draw(Point{x * tileSize, y * tileSize});
		}
	}
}

void Main()
{
	Scene::SetBackground(ColorF{0.3, 0.3, 0.3});

	const Texture texture{U"asset/brick_stylish_24x24.png"};
	auto emoji = Texture{U"🧵"_emoji};
	Vec2 emojiPos{};

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
		const ScopedRenderStates2D sampler{SamplerState::BorderNearest};

		const auto inversed = Graphics2D::GetCameraTransform().inverse();
		auto mapTl = inversed.transformPoint(Vec2{0, 0}).asPoint() / 24;
		auto mapBr = inversed.transformPoint(Scene::Size()).asPoint() / 24;
		for (int y = std::max(0, mapTl.y); y < std::min(mapBr.y + 1, static_cast<int>(tileMap.size())); ++y)
		{
			for (int x = std::max(0, mapTl.x); x < std::min(mapBr.x + 1, static_cast<int>(tileMap[y].size())); ++x)
			{
				drawTileAt(tileMap, y, x, texture);
			}
		}

		emojiPos.moveBy({Scene::DeltaTime(), 0});

		if (emojiPos.x > 200) emojiPos.x = 0;
		(void)emoji.resized({32, 32}).draw(emojiPos);
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
