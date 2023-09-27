#include "stdafx.h"
#include "DungeonGenerator.h"

namespace ExRogue
{
	MapGrid GenerateFreshDungeon(const DungGenProps& props)
	{
		MapGrid dung{props.size};

		// TODO: ちゃんと実装
		dung.At({1, 1}).kind = TerrainKind::Floor;
		dung.At({2, 2}).kind = TerrainKind::Floor;

		return dung;
	}
}
