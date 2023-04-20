#pragma once
#include <cstdint>

class Framebuffer
{
public:
	Framebuffer(uint32_t id);
	~Framebuffer();

	void Bind();
	void Unbind();

private:
	uint32_t framebufferID_;
	uint32_t colorbufferID_;
	uint32_t depthbufferID_;

};

