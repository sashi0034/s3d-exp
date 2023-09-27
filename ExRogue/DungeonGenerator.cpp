#include "stdafx.h"
#include "DungeonGenerator.h"

namespace ExRogue
{
	using SeparatedArea = Rect;

	SeparatedArea PopAreaForSeparate(Array<SeparatedArea>& areas)
	{
		const int index = Random(0llu, areas.size() - 1);
		const auto area = areas[index];
		areas.remove_at(index);
		// TODO: 最低サイズを設定
		return area;
	}

	std::pair<SeparatedArea, SeparatedArea> SeparateArea(const SeparatedArea& area)
	{
		const std::pair<SeparatedArea, SeparatedArea> result = [&]()
		{
			const bool isHorizontal = Random(0, 1);
			if (isHorizontal)
			{
				const int sepY = Random(area.y, area.y + area.h - 1);
				const int sepH = sepY - area.y;
				return std::pair{
					SeparatedArea{area.x, area.y, area.w, sepH},
					SeparatedArea{area.x, sepH, area.w, area.h - sepH}
				};
			}
			else
			{
				const int sepX = Random(area.x, area.x + area.w - 1);
				const int sepW = sepX - area.x;
				return std::pair{
					SeparatedArea{area.x, area.y, sepW, area.h},
					SeparatedArea{sepX, area.y, area.w - sepW, area.h}
				};
			}
		}();
		// TODO: 最低サイズを設定
		return result;
	}

	MapGrid GenerateFreshDungeon(const DungGenProps& props)
	{
		MapGrid dung{props.size};

		Array<SeparatedArea> separatedAreas{Arg::reserve(props.areaDivision)};
		separatedAreas.push_back(SeparatedArea({0, 0}, props.size));

		while (separatedAreas.size() < props.areaDivision)
		{
			const auto separateTarget = PopAreaForSeparate(separatedAreas);
			const auto separated = SeparateArea(separateTarget);
			separatedAreas.push_back(separated.first);
			separatedAreas.push_back(separated.second);
		}

		// TODO: ちゃんと実装
		dung.At({1, 1}).kind = TerrainKind::Floor;
		dung.At({2, 2}).kind = TerrainKind::Floor;

		return dung;
	}
}
