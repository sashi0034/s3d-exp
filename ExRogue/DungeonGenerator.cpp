#include "stdafx.h"
#include "DungeonGenerator.h"

namespace ExRogue
{
	class SeparatedArea : public Rect
	{
		using Rect::Rect;

		bool HasLeftArea() const { return this->x > 0; }
		bool HasTopArea() const { return this->y > 0; }

		bool HasRightArea(const Size& dangSize) const { return this->rightX() - 1 < dangSize.x - 1; }
		bool HasDownArea(const Size& dangSize) const { return this->bottomY() - 1 < dangSize.y - 1; }
	};

	class RoomPathway : public s3d::Rect
	{
	public:
		using Rect::Rect;
		static RoomPathway FromHorizontal(const Point& start, int w) { return RoomPathway{start, {w, 1}}; };
		static RoomPathway FromVertical(const Point& start, int h) { return RoomPathway{start, {1, h}}; };

		bool IsValid() const { return this->size != Size{0, 0}; }
		bool IsHorizontal() const { return this->size.y == 1; }
		bool IsVertical() const { return this->size.x == 1; }
	};

	struct AreaRoom
	{
		SeparatedArea area;
		Rect room;
	};

	constexpr uint32 maxProgramLoopCount = 100000;
	constexpr uint32 minAreaSize = 40;
	constexpr uint32 minAreaWidthHeight = 12;
	constexpr uint32 minRoomSize = 20;
	constexpr uint32 minRoomWidthHeight = 6;

	SeparatedArea popAreaForSeparate(Array<SeparatedArea>& areas)
	{
		// 面積が大きいものを分割対象にする
		areas.sort_by([](const SeparatedArea& l, const SeparatedArea& r) { return l.area() > r.area(); });
		constexpr int index = 0; // Random(0llu, areas.size() - 1);
		const auto area = areas[index];
		if (area.area() * 2 <= minAreaSize) throw DungGenError();
		areas.remove_at(index);
		return area;
	}

	std::pair<SeparatedArea, SeparatedArea> separateArea(const SeparatedArea& area)
	{
		for (auto in : step(maxProgramLoopCount))
		{
			std::pair<SeparatedArea, SeparatedArea> result;
			const bool isVertical = Random(0, 1);
			if (isVertical)
			{
				const int sepY = Random(area.topY(), area.bottomY() - 1);
				const int sepH1 = sepY - area.y;
				const int sepH2 = area.h - sepH1;
				if (sepH1 <= minAreaWidthHeight || sepH2 <= minAreaWidthHeight) continue;
				result = std::pair{
					SeparatedArea{area.x, area.y, area.w, sepH1},
					SeparatedArea{area.x, sepY, area.w, sepH2}
				};
			}
			else
			{
				const int sepX = Random(area.leftX(), area.rightX() - 1);
				const int sepW1 = sepX - area.x;
				const int sepW2 = area.w - sepW1;
				if (sepW1 <= minAreaWidthHeight || sepW2 <= minAreaWidthHeight) continue;
				result = std::pair{
					SeparatedArea{area.x, area.y, sepW1, area.h},
					SeparatedArea{sepX, area.y, sepW2, area.h}
				};
			}

			if (result.first.area() < minAreaSize || result.second.area() < minAreaSize) continue;

			return result;
		}
		throw DungGenError();
	}

	Array<AreaRoom> distributeRoomFromArea(const Array<SeparatedArea>& areas)
	{
		Array<AreaRoom> areaRooms{Arg::reserve(areas.size())};
		for (auto&& area : areas)
		{
			bool isSucceeded = false;
			for (auto in : step(maxProgramLoopCount))
			{
				constexpr uint8 padding = 2;
				const int x = Random(area.leftX() + padding, area.rightX() - 1 - padding);
				const int w = Random(x, area.rightX() - 1 - padding) - x;
				if (w <= minRoomWidthHeight) continue;

				const int y = Random(area.topY() + padding, area.bottomY() - 1 - padding);
				const int h = Random(y, area.bottomY() - 1 - padding) - y;
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
			if (isSucceeded == false) throw DungGenError();
		}
		return areaRooms;
	}


	Array<RoomPathway> connectRoomsByPath(const Array<AreaRoom>& areaRooms)
	{
		Array<RoomPathway> paths{Arg::reserve(areaRooms.size() * 2)};
		Array<bool> connectedFlags{};
		connectedFlags.resize(areaRooms.size());
		int connectedCount = 0;

		while (connectedCount < areaRooms.size())
		{
			const int index1 = Random(0llu, areaRooms.size() - 1);
			const int index2 = Random(0llu, areaRooms.size() - 1);
			if (index1 == index2) continue;
			if (connectedFlags[index1] && connectedFlags[index2]) continue;
			const auto& target1 = areaRooms[index1];
			const auto& target2 = areaRooms[index2];
			if (target1.area.rightX() == target2.area.leftX())
			{
				// 右左で接続
				const int dividedX = target2.area.leftX();
				const int y1 = Random(target1.room.topY() + 1, target1.room.bottomY() - 2);
				const int y2 = Random(target2.room.topY() + 1, target2.room.bottomY() - 2);
				paths.push_back(RoomPathway::FromHorizontal(
					Point(target1.room.rightX(), y1), dividedX - target1.room.rightX()));
				paths.push_back(RoomPathway::FromVertical(
					Point(dividedX, std::min(y1, y2)), std::abs(y1 - y2) + 1));
				paths.push_back(RoomPathway::FromHorizontal(
					Point(dividedX, y2), target2.room.leftX() - dividedX));
				connectedCount += (connectedFlags[index1] == false) + (connectedFlags[index2] == false);
				connectedFlags[index1] = true;
				connectedFlags[index2] = true;
			}
			else if (target1.area.bottomY() == target2.area.topY())
			{
				// 下上で接続
				const int dividedY = target2.area.topY();
				const int x1 = Random(target1.room.leftX() + 1, target1.room.rightX() - 2);
				const int x2 = Random(target2.room.leftX() + 1, target2.room.rightX() - 2);
				paths.push_back(RoomPathway::FromVertical(
					Point(x1, target1.room.bottomY()), dividedY - target1.room.bottomY()));
				paths.push_back(RoomPathway::FromHorizontal(
					Point(std::min(x1, x2), dividedY), std::abs(x1 - x2) + 1));
				paths.push_back(RoomPathway::FromVertical(
					Point(x2, dividedY), target2.room.topY() - dividedY));
				connectedCount += (connectedFlags[index1] == false) + (connectedFlags[index2] == false);
				connectedFlags[index1] = true;
				connectedFlags[index2] = true;
			}
		}
		return std::move(paths);
	}

	MapGrid GenerateFreshDungeon(const DungGenProps& props)
	{
		MapGrid dung{props.size};

		Array<SeparatedArea> separatedAreas{Arg::reserve(props.areaDivision)};
		separatedAreas.push_back(SeparatedArea({{0, 0}, props.size}));

		// 全体をエリアに分割
		while (separatedAreas.size() < props.areaDivision)
		{
			// 分割対象を引っ張る
			const auto separateTarget = popAreaForSeparate(separatedAreas);
			// その対象を2分割
			const auto separated = separateArea(separateTarget);
			separatedAreas.push_back(separated.first);
			separatedAreas.push_back(separated.second);
		}

		// 分割されたエリアに部屋を作る
		auto areaRoomList = distributeRoomFromArea(separatedAreas);

		// 部屋を通路でつなぐ
		auto connectedPaths = connectRoomsByPath(areaRoomList);

		// 部屋を反映
		for (auto&& areaRoom : areaRoomList)
		{
			for (auto&& p : step(areaRoom.room.pos, areaRoom.room.size))
			{
				dung.At(p).kind = TerrainKind::Floor;
			}
		}

		// 通路を反映
		for (auto&& path : connectedPaths)
		{
			for (auto&& p : step(path.pos, path.size))
			{
				dung.At(p).kind = TerrainKind::Pathway;
			}
		}

		return dung;
	}
}
