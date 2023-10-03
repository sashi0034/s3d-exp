#include "stdafx.h"
#include "MazeGenerator.h"

namespace ExMaze
{
	class MazeGenInternal;

	struct MazeGenState
	{
		Grid<bool> dugFlag;
	};
}

class ExMaze::MazeGenInternal
{
public:
	explicit MazeGenInternal(const MazeGenProps& props): props(props)
	{
	}

	void createRooms(MazeGenState& state) const
	{
		for (auto i : step(props.numRooms))
		{
			int x, y;
			while (true)
			{
				constexpr int padding = 3;
				x = padding + (Random(0, props.size.x - padding) / 2) * 2;
				y = padding + (Random(0, props.size.y - padding) / 2) * 2;
				if (canDigRoom(state, {{x - 2, y - 2}, {props.roomSize + 4, props.roomSize + 4}}))
					break;
			}

			for (auto p : step({x, y}, {props.roomSize, props.roomSize}))
			{
				digAt(state, p);
			}
		}
	}

private:
	const MazeGenProps& props;

	bool canDigRoom(const MazeGenState& state, const Rect& rect) const
	{
		for (auto p : step(rect.pos, rect.size / 2, {2, 2}))
		{
			if (canDigAt(state, p) == false) return false;
		}
		return true;
	}

	bool canDigAt(const MazeGenState& state, const Point& p) const
	{
		if (p.x < 0 || props.size.x <= p.x ||
			p.y < 0 || props.size.y <= p.y)
			return false;
		return state.dugFlag[p] == false;
	}

	static void digAt(MazeGenState& state, const Point& p)
	{
		state.dugFlag[p] = true;
	}
};

namespace ExMaze
{
	MapGrid GenerateFreshMaze(const MazeGenProps& props)
	{
		const MazeGenInternal internal{props};
		MazeGenState state{
			.dugFlag = Grid<bool>{props.size}
		};
		internal.createRooms(state);

		MapGrid grid{props.size};
		for (auto p : step({0, 0}, props.size))
		{
			if (state.dugFlag[p]) grid.At(p).kind = TerrainKind::Floor;
		}
		return grid;
	}
}
