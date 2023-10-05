#pragma once
#include "ActorBase.h"

namespace ExUiBasic
{
	class ActorContainer
	{
	public:
		void Update();
		void Clear();
		void Birth(std::unique_ptr<IActor>&& actor);

		template <typename T>
		const T& BirthAs(const T& actor)
		{
			static_assert(std::is_base_of<ActorBase, T>::value);
			Birth(std::make_unique<T>(actor));
			return actor;
		}

		std::vector<std::unique_ptr<IActor>>& ActorList() { return m_actorList; }
		const std::vector<std::unique_ptr<IActor>>& ActorList() const { return m_actorList; }

	private:
		std::vector<std::unique_ptr<IActor>> m_actorList{};
		void sortActorList();
	};
}
