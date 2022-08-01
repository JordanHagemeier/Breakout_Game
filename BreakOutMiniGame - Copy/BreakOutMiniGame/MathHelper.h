#pragma once

class MathHelper {
public:
	static float Lerp(float a, float b, float f)
	{
		//F (1 - f) * a + f * b;
		return a + f * (b - a);
	}
	static float Clamp(float n, float lower, float upper) {
		return std::max(lower, std::min(n, upper));
	}
};
