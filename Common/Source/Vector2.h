#ifndef Vector2_H
#define Vector2_H

#include "MyMath.h"
#include <iostream>

#pragma warning( disable: 4290 ) //for throw(DivideByZero)

struct glm::vec3;
struct Vector2
{
	float x, y;
	bool IsEqual(float a, float b) const;
	
	Vector2(float a = 0.0, float b = 0.0);
	Vector2(const Vector2& rhs);
	Vector2(const glm::vec3& rhs);
	~Vector2();

	void Set(float a = 0, float b = 0); //Set all data
	void SetZero(void); //Set all data to zero
	bool IsZero(void) const; //Check if data is zero
	float operator[](int index) const;

	Vector2 PerpendicularClockwise() const;
	Vector2 PerpendicularAntiClockwise() const;

	Vector2 operator+(const Vector2& rhs) const; //Vector addition
	Vector2& operator+=(const Vector2& rhs);

	Vector2 operator-(const Vector2& rhs) const; //Vector subtraction
	Vector2& operator-=(const Vector2& rhs);

	float Cross(const Vector2& rhs) const;
	Vector2& Normalize();

	Vector2 operator-(void) const; //Unary negation

	Vector2 operator*(float scalar) const; //Scalar multiplication
	Vector2& operator*=(float scalar);

	bool operator==(const Vector2& rhs) const; //Equality check
	bool operator!= (const Vector2& rhs) const; //Inequality check

	Vector2& operator=(const Vector2& rhs); //Assignment operator

	float Length(void) const; //Get magnitude
	float LengthSquared(void) const; //Get square of magnitude

	float DistanceSquared(const Vector2& rhs) const; //Get the distance squared 
	float Distance(const Vector2& rhs) const;
	float Dot(const Vector2& rhs) const; //Dot product


	friend std::ostream& operator<<(std::ostream& os, Vector2& rhs); //print to ostream

	friend Vector2 operator*(float scalar, const Vector2& rhs); //what is this for?
};

#endif //Vector2_H