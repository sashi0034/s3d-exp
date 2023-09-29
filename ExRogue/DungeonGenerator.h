#pragma once
#include "MapGrid.h"

namespace ExRogue
{
	class DungGenError : public std::exception
	{
	};

	struct DungGenProps
	{
		Size size;
		int areaDivision;
	};

	struct DungGenConfig
	{
		uint32 maxProgramLoopCount = 100000;
		uint32 minAreaSize = 40;
		uint32 minAreaWidthHeight = 12;
		uint32 minRoomSize = 20;
		uint32 minRoomWidthHeight = 6;
		uint32 areaRoomPadding = 2;
	};

	MapGrid GenerateFreshDungeon(const DungGenProps& props, const DungGenConfig& config);
}
