#include "Vector2.h"
#include "glm::vec3.h"
#include <exception>

bool Vector2::IsEqual(float a, float b) const
{
    return a == b;
}

float Vector2::DistanceSquared(const Vector2& rhs) const
{
	return (*this - rhs).LengthSquared();
}

float Vector2::Distance(const Vector2& rhs) const
{
	return (*this - rhs).Length();
}

float Vector2::operator[](int index) const
{
	switch (index)
	{
	case 0:
		return this->x;
	case 1:
		return this->y;
	default:
		throw std::invalid_argument("Index out of range");
	}
}

Vector2 Vector2::PerpendicularClockwise() const
{
	return Vector2(y, -x).Normalize();
}

Vector2 Vector2::PerpendicularAntiClockwise() const
{
	return Vector2(-y, x).Normalize();
}

Vector2& Vector2::Normalize( void ) throw( DivideByZero )
{
	float d = Length();
	if (d <= Math::EPSILON && -d <= Math::EPSILON)
		throw DivideByZero();
	x /= d;
	y /= d;
	return *this;
}

Vector2::Vector2(float a, float b)
{
	x = a;
	y = b;
}

Vector2::Vector2(const Vector2& rhs)
{
    x = rhs.x;
    y = rhs.y;
}

Vector2::Vector2(const glm::vec3& rhs)
{
    x = rhs.x;
    y = rhs.y;
}

Vector2::~Vector2()
{
}

void Vector2::Set(float a, float b)
{
    x = a;
    y = b;
}

void Vector2::SetZero(void)
{
	x = 0;
	y = 0;
}

bool Vector2::IsZero(void) const
{
    return (x == 0 && y == 0);
}

Vector2 Vector2::operator+(const Vector2& rhs) const
{
    return Vector2(x + rhs.x, y + rhs.y);
}

Vector2& Vector2::operator+=(const Vector2& rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Vector2 Vector2::operator-( const Vector2& rhs ) const
{
	return Vector2(x - rhs.x, y - rhs.y);
}

/******************************************************************************/
/*!
\brief
operator-= overload for vector subtraction and assignment

\param rhs
	Vector2 to subtract with
\return 
	Resulting vector
*/
/******************************************************************************/
Vector2& Vector2::operator-=( const Vector2& rhs )
{
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

float Vector2::Cross(const Vector2& rhs) const
{
	return x * rhs.y - y - rhs.x;
}

/******************************************************************************/
/*!
\brief
operator- overload for unary negation

\return 
	Resulting vector
*/
/******************************************************************************/
Vector2 Vector2::operator-( void ) const
{
	return Vector2(-x, -y);
}

/******************************************************************************/
/*!
\brief
operator* overload for scalar multiplication

\param scalar - float to multiply with
 
\exception None
\return Resulting vector
*/
Vector2 Vector2::operator*( float scalar ) const
{
	return Vector2(scalar * x, scalar * y);
}

/******************************************************************************/
/*!
\brief
operator*= overload for scalar multiplication and assignment

\param scalar
	float to multiply with
\return 
	Resulting vector
*/
/******************************************************************************/
Vector2& Vector2::operator*=( float scalar )
{
	x *= scalar;
	y *= scalar;
	return *this;
}

/******************************************************************************/
/*!
\brief
operator== overload for equality check, using Math::EPSILON

\param rhs
	vector to compare with
 
\exception None
\return Resulting vector
*/
/******************************************************************************/
bool Vector2::operator==( const Vector2& rhs ) const
{
	return IsEqual(x, rhs.x) && IsEqual(y, rhs.y);
}

/******************************************************************************/
/*!
\brief
operator!= overload for inequality check, using Math::EPSILON

\param rhs
	vector to compare with
\return 
	Resulting vector
*/
/******************************************************************************/
bool Vector2::operator!= ( const Vector2& rhs ) const
{
	return !IsEqual(x, rhs.x) || !IsEqual(y, rhs.y);
}

/******************************************************************************/
/*!
\brief	Assignment operation (assign a vector to this vector)

\param	rhs
	Vector to assign
\return 
	A reference to this vector
*/
/******************************************************************************/
Vector2& Vector2::operator=(const Vector2& rhs)
{
	x = rhs.x;
	y = rhs.y;
	return *this;
}

/******************************************************************************/
/*!
\brief
Return length of vector

\return 
	Length
*/
/******************************************************************************/
float Vector2::Length( void ) const
{
  return sqrt(x * x + y * y);
}

/******************************************************************************/
/*!
\brief
Return square of length of vector

\return 
	Length square
*/
/******************************************************************************/
float Vector2::LengthSquared (void ) const
{
	return x * x + y * y;
}

/******************************************************************************/
/*!
\brief
Dot product of 2 vectors

\param rhs
	Point to calculate dot product with
\return 
	float value of dot product
*/
/******************************************************************************/
float Vector2::Dot( const Vector2& rhs ) const
{
	return x * rhs.x + y * rhs.y;
}


	
std::ostream& operator<<(std::ostream& os, Vector2& rhs)
{
	
	os << "[ " << rhs.x << ", " << rhs.y << " ]";
	return os;
	
}

Vector2 operator*( float scalar, const Vector2& rhs )
{
	return rhs * scalar;
}
