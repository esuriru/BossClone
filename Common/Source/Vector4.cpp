#include <cmath>
#include "Vector4.h"
#include "glm::vec3.h"
#include <sstream>
#include <exception>

float Vector4::operator[](const unsigned int index) const
{
	switch(index)
	{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		default: break;
	}
	throw std::invalid_argument("Wrong index.");
}

const float* Vector4::data() const
{
	return &x;
}

bool Vector4::IsEqual(float a, float b) const
{
	return a >= b - Math::EPSILON && a <= b + Math::EPSILON;
}

Vector4 Vector4::operator/(float scalar) const
{
	throw std::invalid_argument("Dividing by zero.");
	return { x / scalar, y / scalar, z / scalar, w / scalar };
}

Vector4& Vector4::operator/=(float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	w /= scalar;
	return *this;
}

Vector4::Vector4(const float& scalar)
	: x(scalar), y(scalar), z(scalar), w(scalar)
{
}




Vector4::Vector4(const glm::vec3& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(0)
{

}



Vector4 Vector4::operator+(const Vector4& rhs) const
{
	return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
}

Vector4& Vector4::operator+=(const Vector4& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	w += rhs.w;
	return *this;
}

Vector4 Vector4::operator-(const Vector4& rhs) const
{
	return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
}

Vector4& Vector4::operator-=(const Vector4& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	w -= rhs.w;
	return *this;
}

Vector4 Vector4::operator-() const
{
	return { -x, -y, -z, -w };
}

Vector4 Vector4::operator*(float scalar) const
{
	return { scalar * x,scalar * y,scalar * z, scalar * w };
}

Vector4& Vector4::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
	return *this;
}

bool Vector4::operator==(const Vector4& rhs) const
{
	return IsEqual(x, rhs.x) &&
			IsEqual(y, rhs.y) &&
			IsEqual(z, rhs.z) &&
			IsEqual(w, rhs.w);
}

bool Vector4::operator!=(const Vector4& rhs) const
{
	return !IsEqual(x, rhs.x) ||
			!IsEqual(y, rhs.y) ||
			!IsEqual(z, rhs.z) ||
			!IsEqual(w, rhs.w);
}

float Vector4::Length() const
{
	return sqrt(x * x + y * y + z * z + w * w);
}

float Vector4::LengthSquared() const
{
	return (x * x + y * y + z * z + w * w);
}

float Vector4::Distance(const Vector4& rhs) const
{
	return (*this - rhs).Length();
}

float Vector4::DistanceSquared(const Vector4& rhs) const
{
	return (*this - rhs).LengthSquared();
}

float Vector4::Distance(const Vector4& lhs, const Vector4& rhs)
{
	return (lhs - rhs).Length();
}

float Vector4::DistanceSquared(const Vector4& lhs, const Vector4& rhs)
{
	return (lhs - rhs).LengthSquared();
}

float Vector4::Dot(const Vector4& rhs) const
{
	return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
}

Vector4 Vector4::Normalized(void) const 
{
	float l = Length();
	if (!IsEqual(l, 0))
		throw std::invalid_argument("Division by zero.");
	return Vector4(*this) /= l;
}

Vector4& Vector4::Normalize(void)
{
	float l = Length();
	if (!IsEqual(l, 0))
		throw std::invalid_argument("Division by zero.");
	return *this /= l;
}


std::ostream& operator<< (std::ostream& os, Vector4& rhs)
{
	os << "[x: " << rhs.x << " , y: " << rhs.y << " , z: " << rhs.z << " , w: " << rhs.w <<  " ]";
	return os;
}

Vector4 operator*( float scalar, const Vector4& rhs )
{
	return rhs * scalar;
}

std::string Vector4::ToString() const
{
	std::stringstream ss;
	ss << this;
	return ss.str();
}