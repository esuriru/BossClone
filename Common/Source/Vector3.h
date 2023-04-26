/******************************************************************************/
/*!
\file	glm::vec3.h
\author Wen Sheng Tang \ Lim Chian Song
\par	email: tang_wen_sheng\@nyp.edu.sg
\brief
Struct to define a 3D vector
*/
/******************************************************************************/

#ifndef glm::vec3_H
#define glm::vec3_H

#include "MyMath.h"
#include <iostream>

#pragma warning( disable: 4290 ) //for throw(DivideByZero)

/******************************************************************************/
/*!
		Class glm::vec3:
\brief	Defines a 3D vector and its methods
*/
/******************************************************************************/
struct Vector2;
struct glm::vec3
{
	float x, y, z;
	bool IsEqual(float a, float b) const;

	explicit glm::vec3(const float& scalar);
	glm::vec3(float a = 0.0, float b = 0.0, float c = 0.0);
	glm::vec3(const glm::vec3 &rhs);
	glm::vec3(const Vector2& rhs);
	glm::vec3(const Vector2& rhs, float z);
	~glm::vec3();
	
	void Set( float a = 0, float b = 0, float c = 0 ); //Set all data
	void SetZero( void ); //Set all data to zero
	bool IsZero( void ) const; //Check if data is zero

	glm::vec3 operator+( const glm::vec3& rhs ) const; //Vector addition
	glm::vec3& operator+=( const glm::vec3& rhs ); 
	
	glm::vec3 operator-( const glm::vec3& rhs ) const; //Vector subtraction
	glm::vec3& operator-=( const glm::vec3& rhs );
	
	glm::vec3 operator-( void ) const; //Unary negation
	
	glm::vec3 operator*( float scalar ) const; //Scalar multiplication
	glm::vec3& operator*=( float scalar );

	bool operator==( const glm::vec3& rhs ) const; //Equality check
	bool operator!= ( const glm::vec3& rhs ) const; //Inequality check

	glm::vec3& operator=(const glm::vec3& rhs); //Assignment operator

	float Length( void ) const; //Get magnitude
	float LengthSquared (void ) const; //Get square of magnitude
	float Distance(const glm::vec3& rhs) const; //Get the distance 
	float DistanceSquared(const glm::vec3& rhs) const; //Get the distance squared 
	static float Distance(const glm::vec3& lhs, const glm::vec3& rhs);
	static float DistanceSquared(const glm::vec3& lhs, const glm::vec3& rhs);

	float Dot( const glm::vec3& rhs ) const; //Dot product
	glm::vec3 Cross( const glm::vec3& rhs ) const; //Cross product
	
	//Return a copy of this vector, normalized
	//Throw a divide by zero exception if normalizing a zero vector
	glm::vec3 Normalized( void ) const throw( DivideByZero );
	
	//Normalize this vector and return a reference to it
	//Throw a divide by zero exception if normalizing a zero vector
	glm::vec3& Normalize( void ) throw( DivideByZero );

	glm::vec3 Reflect(const glm::vec3& rhs) const;
	
	//Friend Functions
	friend std::ostream& operator<<( std::ostream& os, glm::vec3& rhs); //print to ostream
	friend glm::vec3 operator*( float scalar, const glm::vec3& rhs ); 

};

#endif //glm::vec3_H 