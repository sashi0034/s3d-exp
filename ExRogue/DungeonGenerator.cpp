﻿#include "stdafx.h"
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

	void addPathsBetweenRoomsHorizontal(
		Array<RoomPathway>& paths, const AreaRoom& leftTarget, const AreaRoom& rightTarget)
	{
		const int dividedX = rightTarget.area.leftX();
		const int y1 = Random(leftTarget.room.topY() + 1, leftTarget.room.bottomY() - 2);
		const int y2 = Random(rightTarget.room.topY() + 1, rightTarget.room.bottomY() - 2);
		paths.push_back(RoomPathway::FromHorizontal(
			Point(leftTarget.room.rightX(), y1), dividedX - leftTarget.room.rightX()));
		paths.push_back(RoomPathway::FromVertical(
			Point(dividedX, std::min(y1, y2)), std::abs(y1 - y2) + 1));
		paths.push_back(RoomPathway::FromHorizontal(
			Point(dividedX, y2), rightTarget.room.leftX() - dividedX));
	}

	void addPathsBetweenRoomsVertical(
		Array<RoomPathway>& paths, const AreaRoom& topTarget, const AreaRoom& bottomTarget)
	{
		const int dividedY = bottomTarget.area.topY();
		const int x1 = Random(topTarget.room.leftX() + 1, topTarget.room.rightX() - 2);
		const int x2 = Random(bottomTarget.room.leftX() + 1, bottomTarget.room.rightX() - 2);
		paths.push_back(RoomPathway::FromVertical(
			Point(x1, topTarget.room.bottomY()), dividedY - topTarget.room.bottomY()));
		paths.push_back(RoomPathway::FromHorizontal(
			Point(std::min(x1, x2), dividedY), std::abs(x1 - x2) + 1));
		paths.push_back(RoomPathway::FromVertical(
			Point(x2, dividedY), bottomTarget.room.topY() - dividedY));
	}

	Array<RoomPathway> connectRoomsByPaths(const Array<AreaRoom>& areaRooms)
	{
		Array<RoomPathway> paths{Arg::reserve(areaRooms.size() * 2)};
		Array<int> connectedIndexes{Arg::reserve(areaRooms.size())};
		Array<int> nextConnectIndexQueue{};
		nextConnectIndexQueue.push_back(0);

		while (connectedIndexes.size() < areaRooms.size())
		{
			if (nextConnectIndexQueue.size() == 0)
				throw DungGenError();

			// キューから次の対象を取り出す
			const int index1 = nextConnectIndexQueue[0];
			nextConnectIndexQueue.pop_front();

			// インデックそれぞれに対して調べていく
			for (int index2 = 0; index2 < areaRooms.size(); ++index2)
			{
				// 自分自身を無視
				if (index2 == index1) continue;

				// 既に接続済みなら無視
				if (connectedIndexes.contains(index2)) continue;

				const auto& target1 = areaRooms[index1];
				const auto& target2 = areaRooms[index2];

				const bool connectRightLeft = target1.area.rightX() == target2.area.leftX();
				const bool connectLeftRight = target1.area.leftX() == target2.area.rightX();
				const bool connectBottomTop = target1.area.bottomY() == target2.area.topY();
				const bool connectTopBottom = target1.area.topY() == target2.area.bottomY();

				if (connectRightLeft)
					addPathsBetweenRoomsHorizontal(paths, target1, target2);
				else if (connectLeftRight)
					addPathsBetweenRoomsHorizontal(paths, target2, target1);
				else if (connectBottomTop)
					addPathsBetweenRoomsVertical(paths, target1, target2);
				else if (connectTopBottom)
					addPathsBetweenRoomsVertical(paths, target2, target1);

				if (connectRightLeft || connectLeftRight || connectBottomTop || connectTopBottom)
				{
					// 今回の接続したインデックス情報を確定し、次回はそのインデックスから派生させる
					connectedIndexes.push_back(index2);
					nextConnectIndexQueue.push_back(index2);
				}
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
		auto connectedPaths = connectRoomsByPaths(areaRoomList);

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
