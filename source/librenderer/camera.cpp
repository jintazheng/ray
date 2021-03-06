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
#include <ray/camera.h>
#include <ray/graphics_swapchain.h>
#include <ray/graphics_texture.h>
#include <ray/graphics_framebuffer.h>
#include <ray/render_system.h>
#include <ray/render_object_manager.h>
#include <ray/render_pipeline_framebuffer.h>

_NAME_BEGIN

__ImplementSubClass(Camera, RenderObject, "Camera")

Camera::Camera() noexcept
	: _ortho(-1.0, 1.0, -1.0, 1.0) // left, right, bottom, top
	, _aperture(45.0f)
	, _ratio(1.0f)
	, _znear(1.0f)
	, _zfar(100.0f)
	, _viewport(0.0f, 0.0f, 1.0f, 1.0f)
	, _clearColor(float4(0.0f, 0.0f, 0.0f, 1.0f))
	, _cameraType(CameraType::CameraTypePerspective)
	, _cameraOrder(CameraOrder::CameraOrder3D)
	, _cameraClearType(CameraClearFlagBits::CameraClearColorBit)
	, _cameraRenderFlags(CameraRenderFlagBits::CameraRenderScreenBit)
	, _needUpdateViewProject(true)
	, _project(float4x4::One)
	, _projectInverse(float4x4::One)
	, _viewProejct(float4x4::One)
	, _viewProjectInverse(float4x4::One)
{
	_dataManager = std::make_shared<DefaultRenderDataManager>();
}

Camera::~Camera() noexcept
{
}

void
Camera::setAperture(float aspect) noexcept
{
	_aperture = aspect;
	_needUpdateViewProject = true;
}

float
Camera::getAperture() const noexcept
{
	return _aperture;
}

void
Camera::setNear(float znear) noexcept
{
	_znear = znear;
	_needUpdateViewProject = true;
}

float
Camera::getNear() const noexcept
{
	return _znear;
}

void
Camera::setFar(float zfar) noexcept
{
	_zfar = zfar;
	_needUpdateViewProject = true;
}

float
Camera::getFar() const noexcept
{
	return _zfar;
}

void
Camera::setRatio(float ratio) noexcept
{
	_ratio = ratio;
	_needUpdateViewProject = true;
}

float
Camera::getRatio() const noexcept
{
	return _ratio;
}

void
Camera::setOrtho(const float4& ortho) noexcept
{
	_ortho = ortho;
	_needUpdateViewProject = true;
}

const float4&
Camera::getOrtho() const noexcept
{
	return _ortho;
}

const float4x4&
Camera::getView() const noexcept
{
	return this->getTransformInverse();
}

const float4x4&
Camera::getViewInverse() const noexcept
{
	return this->getTransform();
}

const float4x4&
Camera::getProject() const noexcept
{
	_updateViewProject();
	return _project;
}

const float4x4&
Camera::getProjectInverse() const noexcept
{
	_updateViewProject();
	return _projectInverse;
}

const float4x4&
Camera::getViewProject() const noexcept
{
	_updateViewProject();
	return _viewProejct;
}

const float4x4&
Camera::getViewProjectInverse() const noexcept
{
	_updateViewProject();
	return _viewProjectInverse;
}

float4
Camera::getClipConstant() const noexcept
{
	_updateViewProject();

	float4 clipConstant;
	if (_cameraType == CameraType::CameraTypeOrtho)
	{
		clipConstant.x = _znear;
		clipConstant.y = (_zfar - _znear);
		clipConstant.z = _znear;
		clipConstant.w = _zfar;
	}
	else
	{
		clipConstant.x = _znear * (_zfar / (_zfar - _znear));
		clipConstant.y = _zfar / (_zfar - _znear);
		clipConstant.z = _znear;
		clipConstant.w = _zfar;
	}

	return clipConstant;
}

float4
Camera::getProjConstant() const noexcept
{
	this->_updateViewProject();
	float2 constant(_projectInverse.a1 / _projectInverse.d3, _projectInverse.b2 / _projectInverse.d3);

	auto deviceType = RenderSystem::instance()->getRenderSetting().deviceType;
	if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGL ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES2 ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES3 ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES31 ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES32)
	{
		return float4(2.0f, 2.0f, -1.0f, -1.0f) * constant.xyxy();
	}
	else
	{
		return float4(2.0f, -2.0f, -1.0f, 1.0f) * constant.xyxy();
	}
}

float3
Camera::worldToScreen(const float3& pos) const noexcept
{
	float4 viewport = this->getPixelViewport();

	float w = viewport.z * 0.5f;
	float h = viewport.w * 0.5f;

	float4 v = this->getViewProject() * float4(pos, 1.0);
	if (v.w != 0)
		v /= v.w;

	v.x = v.x * w + w + viewport.x;
	v.y = v.y * h + h + viewport.y;

	return float3(v.x, v.y, v.z);
}

float3
Camera::worldToProject(const float3& pos) const noexcept
{
	float4 v(pos, 1.0);

	v = this->getViewProject() * v;
	if (v.w != 0)
		v /= v.w;

	return v.xyz();
}

float3
Camera::screenToWorld(const float3& pos) const noexcept
{
	float4 viewport = this->getPixelViewport();

	float4 v(pos, 1.0);

	auto deviceType = RenderSystem::instance()->getRenderSetting().deviceType;
	if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGL ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES2 ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES3 ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES31 ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES32)
	{
		v.y = viewport.w - v.y;
	}

	v.x = ((v.x - viewport.x) / viewport.z) * 2.0f - 1.0f;
	v.y = ((v.y - viewport.y) / viewport.w) * 2.0f - 1.0f;

	v = this->getViewProjectInverse() * v;
	if (v.w != 0)
		v /= v.w;

	return float3(v.x, v.y, v.z);
}

float3
Camera::screenToView(const float2& pos) const noexcept
{
	float4 viewport = this->getPixelViewport();

	float4 v(pos, 1.0, 1.0);

	auto deviceType = RenderSystem::instance()->getRenderSetting().deviceType;
	if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGL ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES2 ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES3 ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES31 ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES32)
	{
		v.y = viewport.w - v.y;
	}

	v.x = ((v.x - viewport.x) / viewport.z) * 2.0f - 1.0f;
	v.y = ((v.y - viewport.y) / viewport.w) * 2.0f - 1.0f;

	return (this->getProjectInverse() * v).xyz();
}

void
Camera::setClearColor(const float4& color) noexcept
{
	_clearColor = color;
}

const float4&
Camera::getClearColor() const noexcept
{
	return _clearColor;
}

void
Camera::setClearFlags(CameraClearFlags flags) noexcept
{
	_cameraClearType = flags;
}

CameraClearFlags
Camera::getClearFlags() const noexcept
{
	return _cameraClearType;
}

void
Camera::setViewport(const float4& viewport) noexcept
{
	_viewport = viewport;
}

const float4&
Camera::getViewport() const noexcept
{
	return _viewport;
}

float4
Camera::getPixelViewport() const noexcept
{
	std::uint32_t width, height;

	if (_pipelineFramebuffer->getFramebuffer())
	{
		width = _pipelineFramebuffer->getFramebuffer()->getGraphicsFramebufferDesc().getWidth();
		height = _pipelineFramebuffer->getFramebuffer()->getGraphicsFramebufferDesc().getHeight();
	}
	else
	{
		RenderSystem::instance()->getFramebufferSize(width, height);
	}

	float4 result;
	result.x = _viewport.x * width;
	result.y = _viewport.y * height;
	result.z = _viewport.z * width;
	result.w = _viewport.w * height;
	return result;
}

void
Camera::setCameraType(CameraType type) noexcept
{
	if (_cameraType != type)
	{
		_needUpdateViewProject = true;
		_cameraType = type;
	}
}

void
Camera::setCameraOrder(CameraOrder order) noexcept
{
	_cameraOrder = order;
}

void
Camera::setCameraRenderFlags(CameraRenderFlags flags) noexcept
{
	_cameraRenderFlags = flags;
}

CameraType
Camera::getCameraType() const noexcept
{
	return _cameraType;
}

CameraOrder
Camera::getCameraOrder() const noexcept
{
	return _cameraOrder;
}

CameraRenderFlags
Camera::getCameraRenderFlags() const noexcept
{
	return _cameraRenderFlags;
}

void
Camera::setSwapchain(GraphicsSwapchainPtr swapchin) noexcept
{
	_swapchain = swapchin;
}

const GraphicsSwapchainPtr&
Camera::getSwapchain() const noexcept
{
	return _swapchain;
}

void
Camera::setRenderPipelineFramebuffer(const RenderPipelineFramebufferPtr& framebuffer) noexcept
{
	_pipelineFramebuffer = framebuffer;
}

const RenderPipelineFramebufferPtr&
Camera::getRenderPipelineFramebuffer() const noexcept
{
	return _pipelineFramebuffer;
}

void
Camera::setRenderDataManager(const RenderDataManagerPtr& manager) noexcept
{
	assert(manager);
	_dataManager = manager;
}

const RenderDataManagerPtr&
Camera::getRenderDataManager() const noexcept
{
	return _dataManager;
}

void
Camera::_updateOrtho() const noexcept
{
	_project.makeOrtho_lh(_ortho.x, _ortho.y, _ortho.z, _ortho.w, _znear, _zfar);
	_projectInverse = math::inverse(_project);
}

void
Camera::_updatePerspective() const noexcept
{
	float ratio = 1.0;

	if (_cameraOrder != CameraOrder::CameraOrderShadow)
	{
		std::uint32_t width, height;

		if (_pipelineFramebuffer->getFramebuffer())
		{
			width = _pipelineFramebuffer->getFramebuffer()->getGraphicsFramebufferDesc().getWidth();
			height = _pipelineFramebuffer->getFramebuffer()->getGraphicsFramebufferDesc().getHeight();
		}
		else
		{
			RenderSystem::instance()->getFramebufferSize(width, height);
		}

		ratio = (float)width / height;
	}

	_project.makePerspective_fov_lh(_aperture, _ratio * ratio, _znear, _zfar);
	_projectInverse = math::inverse(_project);
}

void
Camera::_updateViewProject() const noexcept
{
	if (_needUpdateViewProject)
	{
		if (_cameraType == CameraType::CameraTypePerspective)
			this->_updatePerspective();
		else
			this->_updateOrtho();

		_viewProejct = _project * this->getView();
		_viewProjectInverse = math::inverse(_viewProejct);

		_needUpdateViewProject = false;
	}
}

void
Camera::onMoveAfter() noexcept
{
	RenderObject::onMoveAfter();

	_needUpdateViewProject = true;
}

void
Camera::onRenderBefore(const Camera& camera) noexcept
{
	RenderObject::onRenderBefore(camera);

	if (_dataManager)
	{
		_dataManager->assginVisiable(*this);
		_dataManager->noticeObjectsRenderBefore(*this);
	}
}

void
Camera::onRenderAfter(const Camera& camera) noexcept
{
	RenderObject::onRenderAfter(camera);

	if (_dataManager)
		_dataManager->noticeObjectsRenderAfter(*this);
}

_NAME_END