#include <cassert>
#include <cmath>

#include "Vector3.h"
#include "Vector4.h"

#include <stdexcept>

#include "MathHelpers.h"

namespace dae
{
	Vector4::Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	Vector4::Vector4(const Vector3& v, float _w) : x(v.x), y(v.y), z(v.z), w(_w) {}

	float Vector4::Magnitude() const
	{
		return std::sqrt(x * x + y * y + z * z + w * w);
	}

	float Vector4::SqrMagnitude() const
	{
		return x * x + y * y + z * z + w * w;
	}

	float Vector4::Normalize()
	{
		const float m = Magnitude();
		x /= m;
		y /= m;
		z /= m;
		w /= m;

		return m;
	}

	Vector4 Vector4::Normalized() const
	{
		const float m = Magnitude();
		return { x / m, y / m, z / m, w / m };
	}

	float Vector4::Dot(const Vector4& v1, const Vector4& v2)
	{
		//todo W1
		throw std::runtime_error("Not Implemented Yet");
		return {};
	}

#pragma region Operator Overloads
	Vector4 Vector4::operator*(float scale) const
	{
		return { x * scale, y * scale, z * scale, w * scale };
	}

	Vector4 Vector4::operator+(const Vector4& v) const
	{
		return { x + v.x, y + v.y, z + v.z, w + v.w };
	}

	Vector4 Vector4::operator-(const Vector4& v) const
	{
		return { x - v.x, y - v.y, z - v.z, w - v.w };
	}

	Vector4& Vector4::operator+=(const Vector4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	float& Vector4::operator[](int index)
	{
		assert(index <= 3 && index >= 0);

		if (index == 0)return x;
		if (index == 1)return y;
		if (index == 2)return z;
		return w;
	}

	float Vector4::operator[](int index) const
	{
		assert(index <= 3 && index >= 0);

		if (index == 0)return x;
		if (index == 1)return y;
		if (index == 2)return z;
		return w;
	}

	bool Vector4::operator==(const Vector4& v) const
	{
		return AreEqual(x, v.x, .000001f) && AreEqual(y, v.y, .000001f) && AreEqual(z, v.z, .000001f) && AreEqual(w, v.w, .000001f);
	}
#pragma endregion
}
