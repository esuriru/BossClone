#pragma once

class Timestep
{
public:
	Timestep(double time = 0.0f) : time_(time) {}

	inline operator double() const { return time_; }

	inline double GetSeconds() const 
	{
		return time_;
	}

	inline double GetMilliseconds() const
	{
		return time_ * 1000.f;
	}
private:
	double time_;
};