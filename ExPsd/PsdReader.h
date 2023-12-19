#pragma once

namespace ExPsd
{
	class PsdReader
	{
	public:
		PsdReader();
		void ReadPsd();

		const Array<DynamicTexture>& Textures() const;

	private:
		struct Impl;
		std::shared_ptr<Impl> p_impl;
	};
}
