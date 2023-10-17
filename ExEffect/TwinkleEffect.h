#pragma once

namespace ExEffect
{
	std::unique_ptr<IEffect> MakeTwinkleEffect(const Effect& effect, const Vec2& pos);
}
