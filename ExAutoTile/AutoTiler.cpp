#include "stdafx.h"
#include "AutoTiler.h"

namespace ExAutoTile
{
	enum TileAngle
	{
		AngL = 1 << 0,
		AngT = 1 << 1,
		AngR = 1 << 2,
		AngB = 1 << 3,
	};

	constexpr Point diagonalStart{4, 0};

	consteval Array<Point> getAutoTileFormat()
	{
		Array<Point> format(16);
		format[0] = {0, 3};
		format[AngB] = {0, 0};
		format[AngB | AngT] = {0, 1};
		format[AngT] = {0, 2};
		format[AngR] = {1, 0};
		format[AngR | AngL] = {2, 0};
		format[AngL] = {3, 0};
		format[AngL | AngB] = {1, 1};
		format[AngR | AngB] = {3, 1};
		format[AngL | AngT] = {1, 3};
		format[AngR | AngT] = {3, 3};
		format[AngL | AngT | AngR] = {2, 1};
		format[AngT | AngR | AngB] = {1, 2};
		format[AngT | AngL | AngB] = {3, 2};
		format[AngL | AngB | AngR] = {2, 3};
		format[AngL | AngR | AngT | AngB] = {2, 2};
		return format;
	}

	static constexpr auto autoTileFormat = getAutoTileFormat();

	Point GetAutoTileSrcPoint(int tileSize, const AutoTileConnect& connection)
	{
		const auto& point = autoTileFormat[
			connection.connectL * AngL |
			connection.connectT * AngT |
			connection.connectR * AngR |
			connection.connectB * AngB
		];
		return point * tileSize;
	}

	Point GetAutoTileDiagonalSrc(int tileSize, AutoTileDiagonal diagonal)
	{
		switch (diagonal)
		{
		case AutoTileDiagonal::LT:
			return diagonalStart;
		case AutoTileDiagonal::RT:
			return diagonalStart.movedBy({tileSize / 2, 0});
		case AutoTileDiagonal::LB:
			return diagonalStart.movedBy({0, tileSize / 2});
		case AutoTileDiagonal::RB:
			return diagonalStart.movedBy({tileSize / 2, tileSize / 2});
		default: ;
			return {};
		}
	}
}
