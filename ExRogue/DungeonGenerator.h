#pragma once
#include "MapGrid.h"

namespace ExRogue
{
	struct DungGenProps
	{
		Size size;
		int areaDivision;
	};

	MapGrid GenerateFreshDungeon(const DungGenProps& props);
}
