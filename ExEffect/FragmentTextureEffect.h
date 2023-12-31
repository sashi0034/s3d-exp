﻿#pragma once

namespace ExEffect
{
	std::unique_ptr<IEffect> EmitFragmentTextureEffect(
		const Vec2& center, const TextureRegion& texture, const Color& trailColor, int diffusion = 64);
}
