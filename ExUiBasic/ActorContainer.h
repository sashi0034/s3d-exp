﻿#pragma once
#include "ActorBase.h"

namespace ExUiBasic
{
	class ActorContainer
	{
	public:
		void Update();
		void Clear();

		template <typename T>
		const T& Birth(const T& actor)
		{
			static_assert(std::is_base_of<ActorBase, T>::value);
			m_actorList.emplace_back(std::make_unique<T>(actor));
			return actor;
		}

		std::vector<std::unique_ptr<IActor>>& ActorList() { return m_actorList; }
		const std::vector<std::unique_ptr<IActor>>& ActorList() const { return m_actorList; }

	private:
		std::vector<std::unique_ptr<IActor>> m_actorList{};
		void sortActorList();
	};
}
