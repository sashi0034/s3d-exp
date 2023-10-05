#pragma once

namespace ExUiBasic
{
	class ActorContainer;

	class ActorBase
	{
	public:
		ActorBase();
		virtual ~ActorBase() = default;
		virtual void Update();
		virtual float OrderPriority() const;

		void SetActive(bool isActive);
		bool IsActive() const;

		void Kill();
		bool IsDead() const;

		bool HasChildren() const;
		[[nodiscard]] ActorContainer& AsParent();

	private:
		struct Impl;
		std::shared_ptr<Impl> p_impl;
	};
}
