#include "GLBufferObject.h"
#include <assert.h>
#include "GLBufferObject.h"
#include "GLTexture.h"


GLBufferObject::GLBufferObject(GLenum target, GLsizei size)
{
	_size = size;
	_target = target;
	glGenBuffers(1, &_bufferID);
	
}
GLBufferObject::~GLBufferObject()
{
	glDeleteBuffers(1, &_bufferID);
}
//=============================================
GLenum  GLBufferObject::GetBufferID()
{
	return _bufferID;
}

GLenum GLBufferObject::GetTargetType()
{
	return _target;
}

void GLBufferObject::SetTargetType(GLenum target)
{
	_target = target;
}

GLsizei GLBufferObject::GetBufferSize()
{
	return _size;
}

void GLBufferObject::SetBufferSize(GLsizei size)
{
	_size = size;
}

//=============================================

void GLBufferObject::Bind()
{
	glBindBuffer(_target, _bufferID);
}

void GLBufferObject::BindEmpty()
{
	glBindBuffer(_target, 0);
}
//void glBufferDataARB(GLenum target, GLsizei size, const void* data, GLenum usage)
void GLBufferObject::BufferData(const void* data, GLenum usage)
{
	glBufferData(_target, _size, data, usage)	;
}
void GLBufferObject::BufferDataStreamDraw(const void* data, GLsizei size)
{
	if(0==size)
		glBufferData(_target, _size, data, GL_STREAM_DRAW);
	else
		glBufferData(_target, size, data, GL_STREAM_DRAW);

}

//  stream change every frame
void GLBufferObject::BufferDataStreamRead(const void* data, GLsizei size)
{
	if(0==size)
		glBufferData(_target, _size, data, GL_STREAM_READ);
	else
		glBufferData(_target, size, data, GL_STREAM_READ);

}

//draw means the data will be sent to GPU in order to draw
void GLBufferObject::BufferDataStaticDraw(const void* data, GLsizei size)
{
	if(0==size)
		glBufferData(_target, _size, data, GL_STATIC_DRAW);
	else
		glBufferData(_target, size, data, GL_STATIC_DRAW);

}

//void glBufferSubDataARB(GLenum target, GLint offset, GLsizei size, void* data)
void GLBufferObject::BufferSubData(const void* data, GLsizei size, GLint offset )
{
	glBufferSubData(_target, offset, size, data);
}

void* GLBufferObject::MapBuffer(GLenum accessFlag)
{
	void* vptr = glMapBuffer(_target, accessFlag);
	assert(vptr);
	return vptr;
}

//GLboolean is unsigned char type
bool GLBufferObject::UnMapBuffer()
{
	if(glUnmapBuffer(_target) != GL_TRUE)
		return false;
	else
		return true;
}

void GLBufferObject::CopyToTexture2D(GLTexture *texObject)
{
	Bind();
	texObject->Bind();
	texObject->LoadToGPU();
	BindEmpty();

/*	  copy pbo to texture

    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

*/

}