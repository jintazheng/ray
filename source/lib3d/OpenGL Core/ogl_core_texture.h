// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#ifndef _H_OGL_CORE_TEXTURE_H_
#define _H_OGL_CORE_TEXTURE_H_

#include "ogl_types.h"

_NAME_BEGIN

class OGLCoreTexture final : public GraphicsTexture
{
	__DeclareSubClass(OGLCoreTexture, GraphicsTexture)
public:
	OGLCoreTexture() noexcept;
	~OGLCoreTexture() noexcept;

	bool setup(const GraphicsTextureDesc& textureDesc) noexcept;
	void close() noexcept;

	bool map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept;
	void unmap() noexcept;

	GLenum getTarget() const noexcept;
	GLuint getInstanceID() const noexcept;

	const GraphicsTextureDesc& getGraphicsTextureDesc() const noexcept;

private:
	bool applyMipmapLimit(std::uint32_t min, std::uint32_t count) noexcept;
	bool applySamplerWrap(GraphicsSamplerWrap wrap) noexcept;
	bool applySamplerFilter(GraphicsSamplerFilter minFilter, GraphicsSamplerFilter magFilter) noexcept;
	bool applySamplerAnis(GraphicsSamplerAnis anis) noexcept;

private:
	friend class OGLDevice;
	void setDevice(const GraphicsDevicePtr& device) noexcept;
	GraphicsDevicePtr getDevice() noexcept;

private:
	GLenum _target;
	GLuint _pbo;
	GLuint _upbo;
	GLuint _texture;
	GLuint _pboSize;
	GLuint _upboSize;
	GraphicsTextureDesc _textureDesc;
	GraphicsDeviceWeakPtr _device;
};

_NAME_END

#endif