// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
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
#include "ogl_core_texture.h"

_NAME_BEGIN

__ImplementSubClass(OGLCoreTexture, GraphicsTexture, "OGLCoreTexture")

OGLCoreTexture::OGLCoreTexture() noexcept
	: _texture(GL_NONE)
	, _target(GL_INVALID_ENUM)
{
}

OGLCoreTexture::~OGLCoreTexture() noexcept
{
	this->close();
}

bool
OGLCoreTexture::setup(const GraphicsTextureDesc& textureDesc) noexcept
{
	assert(_texture == GL_NONE);

	GLenum target = OGLTypes::asTextureTarget(textureDesc.getTexDim(), textureDesc.isMultiSample());
	if (target == GL_INVALID_ENUM)
		return false;

	GLenum internalFormat = OGLTypes::asTextureInternalFormat(textureDesc.getTexFormat());
	if (internalFormat == GL_INVALID_ENUM)
		return false;

	glCreateTextures(target, 1, &_texture);
	if (_texture == GL_NONE)
	{
		GL_PLATFORM_LOG("glCreateTextures fail");
		return false;
	}

	GLsizei w = (GLsizei)textureDesc.getWidth();
	GLsizei h = (GLsizei)textureDesc.getHeight();
	GLsizei depth = (GLsizei)textureDesc.getDepth();

	GLsizei mipBase = textureDesc.getMipBase();
	GLsizei mipLevel = textureDesc.getMipLevel();

	if (!applySamplerWrap(textureDesc.getSamplerWrap()))
		return false;

	if (!applySamplerFilter(textureDesc.getSamplerFilter()))
		return false;

	if (!applySamplerAnis(textureDesc.getSamplerAnis()))
		return false;

	if (!applyMipmapLimit(mipBase, mipLevel))
		return false;

	if (target == GL_TEXTURE_2D)
		glTextureStorage2D(_texture, mipLevel, internalFormat, w, h);
	else if (target == GL_TEXTURE_2D_MULTISAMPLE)
		glTextureStorage2DMultisample(_texture, mipLevel, internalFormat, w, h, GL_FALSE);
	else if (target == GL_TEXTURE_2D_ARRAY)
		glTextureStorage3D(_texture, mipLevel, internalFormat, w, h, depth);
	else if (target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
		glTextureStorage3DMultisample(_texture, mipLevel, internalFormat, w, h, depth, GL_FALSE);
	else if (target == GL_TEXTURE_3D)
		glTextureStorage3D(_texture, mipLevel, internalFormat, w, h, depth);
	else if (target == GL_TEXTURE_CUBE_MAP)
		glTextureStorage2D(_texture, mipLevel, internalFormat, w, h);
	else if (target == GL_TEXTURE_CUBE_MAP_ARRAY)
		glTextureStorage3D(_texture, mipLevel, internalFormat, w, h, depth);

	auto stream = textureDesc.getStream();
	if (stream)
	{
		if (OGLTypes::isCompressedTexture(textureDesc.getTexFormat()))
		{
			GLsizei offset = 0;
			GLsizei blockSize = internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 16;

			GLint oldPackStore = 1;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldPackStore);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 8);

			for (GLint mip = mipBase; mip < mipBase + mipLevel; mip++)
			{
				GLsizei mipSize = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;

				glCompressedTextureSubImage2D(_texture, mip, 0, 0, w, h, internalFormat, mipSize, (char*)stream + offset);

				w = std::max(w >> 1, 1);
				h = std::max(h >> 1, 1);

				offset += mipSize;
			}

			glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
		}
		else
		{
			GLenum format = OGLTypes::asTextureFormat(textureDesc.getTexFormat());
			GLenum type = OGLTypes::asTextureType(textureDesc.getTexFormat());

			GLsizei offset = 0;
			GLsizei pixelSize = OGLTypes::getFormatNum(format);

			GLint oldPackStore = 1;
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldPackStore);
			glPixelStorei(GL_UNPACK_ALIGNMENT, pixelSize == 3 ? 1 : pixelSize);

			for (GLsizei mip = mipBase; mip < mipBase + mipLevel; mip++)
			{
				GLsizei mipSize = w * h * pixelSize;
				GLsizei layerBase = textureDesc.getLayerBase() + 1;
				GLsizei layerLevel = textureDesc.getLayerNums();

				for (GLsizei layer = layerBase; layer < layerBase + layerLevel; layer++)
				{
					if (target == GL_TEXTURE_2D || target == GL_TEXTURE_2D_MULTISAMPLE)
					{
						glTextureSubImage2D(_texture, mip, 0, 0, w, h, format, type, (char*)stream + offset);
						offset += mipSize;
					}
					else
					{
						if (target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_CUBE_MAP_ARRAY)
						{
							glTextureSubImage3D(_texture, mip, 0, 0, 0, w, h, 6 * layer, format, type, (char*)stream + offset);
							offset += mipSize * 6;
						}
						else
						{
							glTextureSubImage3D(_texture, mip, 0, 0, 0, w, h, depth * layer, format, type, (char*)stream + offset);
							offset += mipSize * depth;
						}
					}

					w = std::max(w >> 1, 1);
					h = std::max(h >> 1, 1);
				}
			}

			glPixelStorei(GL_UNPACK_ALIGNMENT, oldPackStore);
		}
	}

	_target = target;
	_textureDesc = textureDesc;

	return OGLCheck::checkError();
}

void
OGLCoreTexture::close() noexcept
{
	if (_texture != GL_NONE)
	{
		glDeleteTextures(1, &_texture);
		_texture = GL_NONE;
	}
}

GLenum
OGLCoreTexture::getTarget() const noexcept
{
	return _target;
}

GLuint
OGLCoreTexture::getInstanceID() const noexcept
{
	return _texture;
}

bool
OGLCoreTexture::applyMipmapLimit(std::uint32_t min, std::uint32_t count) noexcept
{
	glTextureParameteri(_texture, GL_TEXTURE_BASE_LEVEL, min);
	glTextureParameteri(_texture, GL_TEXTURE_MAX_LEVEL, min + count - 1);
	return true;
}

bool
OGLCoreTexture::applySamplerWrap(GraphicsSamplerWrap wrap) noexcept
{
	GLenum glwrap = OGLTypes::asSamplerWrap(wrap);
	if (glwrap != GL_INVALID_ENUM)
	{
		glTextureParameteri(_texture, GL_TEXTURE_WRAP_S, glwrap);
		glTextureParameteri(_texture, GL_TEXTURE_WRAP_T, glwrap);
		glTextureParameteri(_texture, GL_TEXTURE_WRAP_R, glwrap);

		return true;
	}

	return false;
}

bool
OGLCoreTexture::applySamplerFilter(GraphicsSamplerFilter filter) noexcept
{
	GLenum min = OGLTypes::asSamplerMinFilter(filter);
	GLenum mag = OGLTypes::asSamplerMagFilter(filter);
	if (min != GL_INVALID_ENUM && mag != GL_INVALID_ENUM)
	{
		glTextureParameteri(_texture, GL_TEXTURE_MIN_FILTER, min);
		glTextureParameteri(_texture, GL_TEXTURE_MAG_FILTER, mag);
		return true;
	}

	return false;
}

bool
OGLCoreTexture::applySamplerAnis(GraphicsSamplerAnis anis) noexcept
{
	if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis1)
		glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
	else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis2)
		glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2);
	else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis4)
		glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis8)
		glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
	else if (anis == GraphicsSamplerAnis::GraphicsSamplerAnis16)
		glTextureParameteri(_texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
	else
	{
		GL_PLATFORM_LOG("Can't support anisotropy format");
		return false;
	}

	return true;
}

const GraphicsTextureDesc&
OGLCoreTexture::getGraphicsTextureDesc() const noexcept
{
	return _textureDesc;
}

void
OGLCoreTexture::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
OGLCoreTexture::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END