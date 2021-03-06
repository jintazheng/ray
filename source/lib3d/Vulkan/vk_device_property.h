// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#ifndef _H_VK_DEVICE_PROPERTY_H_
#define _H_VK_DEVICE_PROPERTY_H_

#include "vk_types.h"

_NAME_BEGIN

class VulkanDeviceProperty final : public GraphicsDeviceProperty
{
public:
	VulkanDeviceProperty() noexcept;
	~VulkanDeviceProperty() noexcept;

	bool setup(VkPhysicalDevice physicalDevice) noexcept;
	void close() noexcept;

	VkPhysicalDevice getPhysicalDevice() const noexcept;

	const GraphicsDeviceProperties& getGraphicsDeviceProperties() const noexcept;

private:
	bool checkPhysicalDeviceLayer(VkPhysicalDevice physicalDevice, std::size_t deviceEnabledLayerCount, const char* deviceValidationLayerNames[]) noexcept;
	bool checkPhysicalDeviceExtension(VkPhysicalDevice physicalDevice, std::size_t deviceEnabledExtensitionCount, const char* deviceEnabledExtensitionNames[]) noexcept;

private:
	bool initTextureSupports() noexcept;
	bool initTextureDimSupports() noexcept;
	bool initVertexSupports() noexcept;
	bool initShaderSupports() noexcept;

private:
	VulkanDeviceProperty(const VulkanDeviceProperty&) = delete;
	VulkanDeviceProperty& operator=(const VulkanDeviceProperty&) = delete;

private:
	VkPhysicalDevice _physicalDevice;
	GraphicsDeviceProperties _deviceProperties;
};

_NAME_END

#endif