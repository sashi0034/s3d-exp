﻿#pragma once

namespace ExUiBasic
{
	class ActorContainer;

	struct IActor
	{
		virtual ~IActor() = default;
		virtual void Update() = 0;
		virtual float OrderPriority() const = 0;
		virtual bool IsActive() const = 0;
		virtual bool IsDead() const = 0;
	};

	class ActorBase : public IActor
	{
	public:
		ActorBase();
		~ActorBase() override = default;
		void Update() override;
		float OrderPriority() const override;

		void SetActive(bool isActive);
		bool IsActive() const override;

		void Kill();
		bool IsDead() const override;

		bool HasChildren() const;
		[[nodiscard]] ActorContainer& AsParent();

	private:
		struct Impl;
		std::shared_ptr<Impl> p_impl;
	};
}
