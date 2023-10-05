#pragma once
#include "ActorBase.h"

namespace ExUiBasic
{
	class ActorContainer
	{
	public:
		void Update();
		void Clear();
		void Birth(const ActorBase& actor);

		template <typename T>
		const T& BirthAs(const T& actor)
		{
			static_assert(std::is_base_of<ActorBase, T>::value);
			m_actorList.push_back(actor);
			return actor;
		}

		std::vector<ActorBase>& ActorList() { return m_actorList; }
		const std::vector<ActorBase>& ActorList() const { return m_actorList; }

	private:
		std::vector<ActorBase> m_actorList{};
		void sortActorList();
	};
}
