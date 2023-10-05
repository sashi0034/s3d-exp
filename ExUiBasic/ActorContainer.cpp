#include "stdafx.h"
#include "ActorContainer.h"

namespace ExUiBasic
{
	void ActorContainer::sortActorList()
	{
		std::ranges::stable_sort(
			m_actorList,
			[](const ActorBase& left, const ActorBase& right)
			{
				return left.OrderPriority() < right.OrderPriority();
			});
	}

	void ActorContainer::Update()
	{
		for (int i = m_actorList.size() - 1; i >= 0; --i)
		{
			const auto& actor = m_actorList[i];
			if (actor.IsDead()) m_actorList.erase(m_actorList.begin() + i);
		}

		// 優先度が高いほど後から更新するように並び変える
		sortActorList();

		// 更新
		for (int i = 0; i < m_actorList.size(); ++i)
		{
			auto&& actor = m_actorList[i];
			if (actor.IsActive() == false) continue;;
			actor.Update();
			// if (actor.HasChildren()) actor.AsParent().Update();
		}
	}

	void ActorContainer::Clear()
	{
		m_actorList.clear();
	}

	void ActorContainer::Birth(const ActorBase& actor)
	{
		m_actorList.push_back(actor);
	}
}
