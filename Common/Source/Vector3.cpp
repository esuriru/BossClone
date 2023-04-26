/******************************************************************************/
/*!
\file	glm::vec3.cpp
\author Wen Sheng Tang \ Lim Chian Song
\par	email: tang_wen_sheng\@nyp.edu.sg
\brief
Struct to define a 3D vector
*/
/******************************************************************************/
#include <cmath>
#include "glm::vec3.h"
#include "Vector2.h"

bool glm::vec3::IsEqual(float a, float b) const
{
	return a - b <= Math::EPSILON && b - a <= Math::EPSILON;
}

glm::vec3::glm::vec3(const float& scalar)
	: x(scalar), y(scalar), z(scalar)
{
}

/******************************************************************************/
/*!
\brief	glm::vec3 default constructor

\param	a
	x value
\param	b
	y value
\param	c
	z value
*/
/******************************************************************************/
glm::vec3::glm::vec3(float a, float b, float c) : x(a), y(b), z(c)
{
}

/******************************************************************************/
/*!
\brief	glm::vec3 copy constructor

\param	rhs
	Vector to copy from
*/
/******************************************************************************/
glm::vec3::glm::vec3(const glm::vec3 &rhs) : x(rhs.x), y(rhs.y), z(rhs.z)
{
}

glm::vec3::glm::vec3(const Vector2& rhs)
	: x(rhs.x), y(rhs.y), z(0)
{

}

glm::vec3::glm::vec3(const Vector2& rhs, float z)
	: x(rhs.x), y(rhs.y), z(z)
{
}

/******************************************************************************/
/*!
\brief	glm::vec3 destructor
*/
/******************************************************************************/
glm::vec3::~glm::vec3()
{
}

/******************************************************************************/
/*!
\brief	Set the elements of this vector

\param	a
	x value
\param	b
	y value
\param	c
	z value
*/
/******************************************************************************/
void glm::vec3::Set( float a, float b, float c )
{
	x = a;
	y = b;
	z = c;
}

/******************************************************************************/
/*!
\brief	Set all elements to zero
*/
/******************************************************************************/
void glm::vec3::SetZero( void )
{
	Set(0.0f, 0.0f, 0.0f);
}

/******************************************************************************/
/*!
\brief	Check if vector is zero, using Math::Epsilon

\return TRUE if vector is zero
*/
/******************************************************************************/
bool glm::vec3::IsZero( void ) const
{
	return IsEqual(x, 0.f) && IsEqual(y, 0.f) && IsEqual(z, 0.f);
}

/******************************************************************************/
/*!
\brief
operator+ overload for vector addition

\param rhs
	glm::vec3 to add with
\return 
	Resulting vector
*/
/******************************************************************************/
glm::vec3 glm::vec3::operator+( const glm::vec3& rhs ) const
{
	return glm::vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

/******************************************************************************/
/*!
\brief
operator+= overload for vector addition and assignment

\param rhs
	glm::vec3 to add with
\return 
	Resulting vector
*/
/******************************************************************************/
glm::vec3& glm::vec3::operator+=( const glm::vec3& rhs )
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

/******************************************************************************/
/*!
\brief
operator- overload for vector subtraction

\param rhs
	glm::vec3 to subtract with
\return Resulting vector
*/
/******************************************************************************/
glm::vec3 glm::vec3::operator-( const glm::vec3& rhs ) const
{
	return glm::vec3(x - rhs.x, y - rhs.y, z - rhs.z);
}

/******************************************************************************/
/*!
\brief
operator-= overload for vector subtraction and assignment

\param rhs
	glm::vec3 to subtract with
\return 
	Resulting vector
*/
/******************************************************************************/
glm::vec3& glm::vec3::operator-=( const glm::vec3& rhs )
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

/******************************************************************************/
/*!
\brief
operator- overload for unary negation

\return 
	Resulting vector
*/
/******************************************************************************/
glm::vec3 glm::vec3::operator-( void ) const
{
	return glm::vec3(-x, -y, -z);
}

/******************************************************************************/
/*!
\brief
operator* overload for scalar multiplication

\param scalar - float to multiply with
 
\exception None
\return Resulting vector
*/
glm::vec3 glm::vec3::operator*( float scalar ) const
{
	return glm::vec3(scalar * x,scalar * y,scalar * z);
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
glm::vec3& glm::vec3::operator*=( float scalar )
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
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
bool glm::vec3::operator==( const glm::vec3& rhs ) const
{
	return IsEqual(x, rhs.x) &&
			IsEqual(y, rhs.y) &&
			IsEqual(z, rhs.z);
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
bool glm::vec3::operator!= ( const glm::vec3& rhs ) const
{
	return !IsEqual(x, rhs.x) ||
			!IsEqual(y, rhs.y) ||
			!IsEqual(z, rhs.z);
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
glm::vec3& glm::vec3::operator=(const glm::vec3& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
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
float glm::vec3::Length( void ) const
{
	return sqrt(x * x + y * y + z * z);
}

/******************************************************************************/
/*!
\brief
Return square of length of vector

\return 
	Length square
*/
/******************************************************************************/
float glm::vec3::LengthSquared (void ) const
{
	return (x * x + y * y + z * z);
}

/******************************************************************************/
/*
\brief Distance operation using the length

\param rhs
	Vector to assign

\return
	A float of the distance between the current and rhs
*/
/******************************************************************************/
float glm::vec3::Distance(const glm::vec3& rhs) const
{
	return (*this - rhs).Length();
}

/******************************************************************************/
/*
\brief Distance operation using the length

\param rhs
	Vector to assign

\return
	A float of the distance between the current and rhs
*/
/******************************************************************************/
float glm::vec3::DistanceSquared(const glm::vec3& rhs) const
{
	return (*this - rhs).LengthSquared();
}

/******************************************************************************/
/*!
\brief
Get The Distance between 2 Vector

\param lhs
	Vector 1
\param rhs
	Vector 2
\return
	distance between the 2 vector
*/
/******************************************************************************/
float glm::vec3::Distance(const glm::vec3& lhs, const glm::vec3& rhs)
{
	return (lhs - rhs).Length();
}

/******************************************************************************/
/*!
\brief
Get The Squared Distance between 2 Vector

\param lhs
	Vector 1
\param rhs
	Vector 2
\return
	distance between the 2 vector
*/
/******************************************************************************/
float glm::vec3::DistanceSquared(const glm::vec3& lhs, const glm::vec3& rhs)
{
	return (lhs - rhs).LengthSquared();
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
float glm::vec3::Dot( const glm::vec3& rhs ) const
{
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

/******************************************************************************/
/*!
\brief
Cross product of 2 vectors

\param rhs
	Point to calculate cross product with
\return 
	Vector of cross product
*/
/******************************************************************************/
glm::vec3 glm::vec3::Cross( const glm::vec3& rhs ) const
{
	return glm::vec3(y * rhs.z - z * rhs.y,
					z * rhs.x - x * rhs.z,
					x * rhs.y - y * rhs.x);
}

/******************************************************************************/
/*!
\brief
Return a copy of this vector, normalized

\param rhs
	Point to calculate distance from
\exception Divide by zero 
	thrown if normalizing a zero vector
\return 
	Resulting normalized vector
*/
/******************************************************************************/
glm::vec3 glm::vec3::Normalized( void ) const throw( DivideByZero )
{
	float d = Length();
	if (d <= Math::EPSILON && -d <= Math::EPSILON)
		throw DivideByZero();
	return glm::vec3(x / d, y / d, z / d);
}

/******************************************************************************/
/*!
\brief
Normalize this vector and return a reference to it

\param rhs
	Point to calculate distance from
\exception Divide by zero 
	thrown if normalizing a zero vector
\return 
	Reference to this vector
*/
/******************************************************************************/
glm::vec3& glm::vec3::Normalize( void ) throw( DivideByZero )
{
	float d = Length();
	if (d <= Math::EPSILON && -d <= Math::EPSILON)
		throw DivideByZero();
	x /= d;
	y /= d;
	z /= d;
	return *this;
}

glm::vec3 glm::vec3::Reflect(const glm::vec3& rhs) const
{
	return *this - 2 * this->Dot(rhs) * rhs;
}

/******************************************************************************/
//
//UTILITY FUNCTIONS
//
/******************************************************************************/
/*!
\brief
operator<< for ostream to be use for cout

\param os
	reference to the ostream
\param rhs
	reference to the right side operand which is a glm::vec3
\return
	Resulting the updated ostream
*/
/******************************************************************************/
std::ostream& operator<< (std::ostream& os, glm::vec3& rhs)
{
	os << "[x: " << rhs.x << " , y: " << rhs.y << " , z: " << rhs.z << " ]";
	return os;
}

/******************************************************************************/
/*!
\brief
Scalar product of scalar with vector

\param scalar
	scalar value
\param vector
	vector
\return 
	Resulting vector
*/
/******************************************************************************/
glm::vec3 operator*( float scalar, const glm::vec3& rhs )
{
	return rhs * scalar;
}