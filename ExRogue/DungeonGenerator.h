#pragma once
#include "MapGrid.h"

namespace ExRogue
{
	struct DungGenProps
	{
		Size size;
	};

	MapGrid GenerateFreshDungeon(const DungGenProps& props);
}
