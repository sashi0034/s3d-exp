#pragma once

namespace ExEffect
{
	std::unique_ptr<IEffect> MakeFragmentTextureEffect(const Vec2& center, const Texture& texture, const Rect& region);
}
