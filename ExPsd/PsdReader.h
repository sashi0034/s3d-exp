#pragma once
#include "PSDObject.h"

namespace ExPsd
{
	enum class StoreTarget
	{
		Image,
		Texture,
		ImageAndTexture,
	};

	class PsdReader
	{
	public:
		struct Config
		{
			StoreTarget storeTarget = StoreTarget::Texture;
		};

		PsdReader();
		explicit PsdReader(const Config& config = {});

		/// @brief ファイル読み込み時などで発生したエラー (レイヤー単体にはエラーが含まれている可能性があります)
		[[nodiscard]]
		Optional<PSDError> getCriticalError() const;

		[[nodiscard]]
		const PSDObject& getObject() const;

	private:
		struct Impl;
		std::shared_ptr<Impl> p_impl;
	};
}
