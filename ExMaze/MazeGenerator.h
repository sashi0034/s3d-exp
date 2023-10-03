#pragma once
#include "MapGrid.h"

namespace ExMaze
{
	struct MazeGenProps
	{
		Size size;
		int numRooms = 10;
		int roomSize = 5;
	};

	MapGrid GenerateFreshMaze(const MazeGenProps& props);
}
