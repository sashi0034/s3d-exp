#include "stdafx.h"
#include "DungeonGenerator.h"

namespace ExRogue
{
	using SeparatedArea = Rect;

	struct AreaRoom
	{
		SeparatedArea area;
		Rect room;
	};

	constexpr uint32 maxProgramLoopCount = 100000;
	constexpr uint32 minAreaSize = 20;
	constexpr uint32 minAreaWidthHeight = 5;
	constexpr uint32 minRoomSize = 10;
	constexpr uint32 minRoomWidthHeight = 3;

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

	Array<AreaRoom> DistributeRoomFromArea(const Array<SeparatedArea>& areas)
	{
		Array<AreaRoom> areaRooms{Arg::reserve(areas.size())};
		for (auto&& area : areas)
		{
			bool isSucceeded = false;
			for (auto in : step(maxProgramLoopCount))
			{
				const int x = Random(area.leftX() + 1, area.rightX() - 1);
				const int w = Random(x, area.rightX() + 1) - x;
				if (w <= minRoomWidthHeight) continue;

				const int y = Random(area.topY() + 1, area.bottomY() - 1);
				const int h = Random(y, area.bottomY() - 1) - y;
				if (h <= minRoomWidthHeight) continue;

				if (w * h <= minRoomSize) continue;

				// 部屋割り当て成功
				areaRooms.push_back(AreaRoom{
					.area = area,
					.room = Rect{x, y, w, h}
				});
				isSucceeded = true;
				break;
			}
			if (isSucceeded == false) throw DunGenError();
		}
		return areaRooms;
	}

	MapGrid GenerateFreshDungeon(const DungGenProps& props)
	{
		MapGrid dung{props.size};

		Array<SeparatedArea> separatedAreas{Arg::reserve(props.areaDivision)};
		separatedAreas.push_back(SeparatedArea({0, 0}, props.size));

		// 全体をエリアに分割
		while (separatedAreas.size() < props.areaDivision)
		{
			// 分割対象を引っ張る
			const auto separateTarget = PopAreaForSeparate(separatedAreas);
			// その対象を2分割
			const auto separated = SeparateArea(separateTarget);
			separatedAreas.push_back(separated.first);
			separatedAreas.push_back(separated.second);
		}

		// 分割されたエリアに部屋を作る
		auto areaRoomList = DistributeRoomFromArea(separatedAreas);

		// 部屋を反映
		for (auto&& areaRoom : areaRoomList)
		{
			for (auto&& p : step(areaRoom.room.pos, areaRoom.room.size))
			{
				dung.At(p).kind = TerrainKind::Floor;
			}
		}

		return dung;
	}
}
