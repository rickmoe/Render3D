#pragma once

class VertexBuffer
{
private:
	unsigned int m_rendererID;
public:
	VertexBuffer() : m_rendererID(0) {}
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void bind() const;
	void unbind() const;
};