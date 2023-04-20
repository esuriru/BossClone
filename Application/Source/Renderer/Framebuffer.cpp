#include "Framebuffer.h"
#include <iostream>
#include "Core/Application.h"
#include "GL/glew.h"

Framebuffer::Framebuffer(uint32_t skybox_id)
{
	glGenFramebuffers(1, &framebufferID_);
	Bind();
	
	glGenTextures(1, &colorbufferID_);
	glBindTexture(GL_TEXTURE_2D, colorbufferID_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, colorbufferID_, 0);

	glGenRenderbuffers(1, &depthbufferID_);
	glBindRenderbuffer(GL_RENDERBUFFER, depthbufferID_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbufferID_);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is not complete.\n";
	}

	Unbind();
}

Framebuffer::~Framebuffer()
{
	glDeleteBuffers(1, &framebufferID_);
	glDeleteBuffers(1, &colorbufferID_);
	glDeleteBuffers(1, &depthbufferID_);
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID_);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
