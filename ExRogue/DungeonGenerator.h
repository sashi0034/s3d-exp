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

	MapGrid GenerateFreshDungeon(const DungGenProps& props);
}
