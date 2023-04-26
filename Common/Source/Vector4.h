#pragma once
#include <string>


struct Vector2;
struct glm::vec3;

struct Vector4
{
	// Use a union because it allows access to the variable through a different name
	union { float x, r; };
	union { float y, g; };
	union { float z, b; };
	union { float w, a; };


#pragma region CONSTRUCTORS
	explicit Vector4(const float& scalar);

    constexpr Vector4()
        : x(0), y(0), z(0), w(0) {};
	constexpr Vector4(float x, float y, float z, float w) noexcept
        : x(x), y(y), z(z), w(w) {}
    constexpr Vector4(const Vector4& rhs) = default;
    constexpr Vector4& operator= (const Vector4& rhs) = default;
	Vector4(const glm::vec3& rhs);
	~Vector4() = default;

#pragma endregion CONSTRUCTORS

#pragma region UTILITY

	std::string ToString() const;
	float operator[](const unsigned int index) const;
	const float* data() const;

	bool IsEqual(float a, float b) const;

#pragma endregion UTILITY
	
#pragma region OPERATORS
	Vector4 operator+(const Vector4& rhs) const; 
	Vector4& operator+=(const Vector4& rhs); 
	
	Vector4 operator-(const Vector4& rhs) const; 
	Vector4& operator-=(const Vector4& rhs);
	
	Vector4 operator-() const; 
	
	Vector4 operator*(float scalar) const; 
	Vector4& operator*=(float scalar);

	Vector4 operator/(float scalar) const; 
	Vector4& operator/=(float scalar);

	bool operator==(const Vector4& rhs) const;
	bool operator!= (const Vector4& rhs) const;

	float Length() const; 
	float LengthSquared () const; 
	float Distance(const Vector4& rhs) const; 
	float DistanceSquared(const Vector4& rhs) const; 

	static float Distance(const Vector4& lhs, const Vector4& rhs);
	static float DistanceSquared(const Vector4& lhs, const Vector4& rhs);

	float Dot(const Vector4& rhs) const; 
	
	Vector4 Normalized() const;
	
	Vector4& Normalize();

	friend Vector4 operator*( float scalar, const Vector4& rhs ); 

	friend std::ostream& operator<<(std::ostream& os, Vector4& rhs); 
#pragma endregion OPERATORS 
	

};
