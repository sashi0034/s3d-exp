#include "stdafx.h"
#include "DungeonGenerator.h"

namespace ExRogue
{
	using SeparatedArea = Rect;

	constexpr uint32 maxProgramLoopCount = 100000;
	constexpr uint32 minAreaSize = 20;
	constexpr uint32 minAreaWidthHeight = 4;

	SeparatedArea PopAreaForSeparate(Array<SeparatedArea>& areas)
	{
		for (auto in : step(maxProgramLoopCount))
		{
			const int index = Random(0llu, areas.size() - 1);
			const auto area = areas[index];
			if (area.area() * 2 <= minAreaSize) continue;
			areas.remove_at(index);
			// TODO: 最低サイズを設定
			return area;
		}
		throw DunGenError();
	}

	std::pair<SeparatedArea, SeparatedArea> SeparateArea(const SeparatedArea& area)
	{
		for (auto in : step(maxProgramLoopCount))
		{
			std::pair<SeparatedArea, SeparatedArea> result;
			const bool isHorizontal = Random(0, 1);
			if (isHorizontal)
			{
				const int sepY = Random(area.y, area.y + area.h - 1);
				const int sepH = sepY - area.y;
				if (sepH <= minAreaWidthHeight || (area.h - sepH) <= minAreaWidthHeight) continue;
				result = std::pair{
					SeparatedArea{area.x, area.y, area.w, sepH},
					SeparatedArea{area.x, sepH, area.w, area.h - sepH}
				};
			}
			else
			{
				const int sepX = Random(area.x, area.x + area.w - 1);
				const int sepW = sepX - area.x;
				if (sepW <= minAreaWidthHeight || (area.w - sepW) <= minAreaWidthHeight) continue;
				result = std::pair{
					SeparatedArea{area.x, area.y, sepW, area.h},
					SeparatedArea{sepX, area.y, area.w - sepW, area.h}
				};
			}

			if (result.first.area() < minAreaSize || result.second.area() < minAreaSize) continue;

			// TODO: 最低サイズを設定
			return result;
		}
		throw DunGenError();
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
