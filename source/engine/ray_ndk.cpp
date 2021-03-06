// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
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
#if defined(_BUILD_PLATFORM_ANDROID)
#include <ray/ray.h>
#include <ray/ray_main.h>

#include <ray/game_application.h>

#include <ray/fcntl.h>
#include <ray/input.h>

#include <android_native_app_glue.h>

ray::GameApplicationPtr _gameApp;
ray::util::string _gameRootPath;
ray::util::string _gameScenePath;
bool _isQuitRequest = false;
android_app* _android_app = nullptr;

void rayInit(const char* gamedir, const char* scenename) noexcept
{
	if (gamedir)
	{
		if (ray::fcntl::access(gamedir, 0) == 0)
			_gameRootPath = ray::util::directory(gamedir);
	}

	if (scenename)
		_gameScenePath = scenename;

	_isQuitRequest = false;
}

void rayTerminate() noexcept
{
	rayCloseWindow();
}

bool rayOpenWindow(const char* title, int w, int h) noexcept
{
	_gameApp = std::make_shared<ray::GameApplication>();
	_gameApp->setFileService(true);
	_gameApp->setFileServicePath(_gameRootPath);

	if (w == 0)
		w = _android_app->contentRect.right - _android_app->contentRect.left;
	if (h == 0)
		h = _android_app->contentRect.bottom - _android_app->contentRect.top;

	if (!_gameApp->open(_android_app->window, w, h))
		return false;

	if (!_gameScenePath.empty())
	{
		if (!_gameApp->openScene(_gameScenePath))
			return false;
	}

	return true;
}

void rayCloseWindow() noexcept
{
	if (_gameApp)
	{
		_gameApp.reset();
		_gameApp = nullptr;
	}

	_isQuitRequest = true;
}

bool rayIsQuitRequest() noexcept
{
	return _isQuitRequest || _android_app->destroyRequested ? true : false;
}

void rayUpdate() noexcept
{
	if (_gameApp)
		_gameApp->update();
}

static int32_t onInputEvent(struct android_app* app, AInputEvent* event)
{
	int32_t lEventType = AInputEvent_getType(event);
	switch (lEventType)
	{
	case AINPUT_EVENT_TYPE_MOTION:
		switch (AInputEvent_getSource(event))
		{
		case AINPUT_SOURCE_TOUCHSCREEN:
			break;
		case AINPUT_SOURCE_TRACKBALL:
			break;
		}
	case AINPUT_EVENT_TYPE_KEY:
	{
		ray::InputEvent inputEvent;
		inputEvent.event = ray::InputEvent::Character;
		inputEvent.key.timestamp = ::AKeyEvent_getEventTime(event);
		inputEvent.key.state = true;

		inputEvent.key.keysym.raw = AKeyEvent_getScanCode(event);
		inputEvent.key.keysym.sym = AKeyEvent_getKeyCode(event);
		inputEvent.key.keysym.unicode = AKeyEvent_getKeyCode(event);
	}
	}

	int32_t id = AMotionEvent_getAction(event);
	switch (id)
	{
	case AMOTION_EVENT_ACTION_MOVE:
	{
		ray::InputEvent inputEvent;
		inputEvent.event = ray::InputEvent::TouchMotionMove;
		inputEvent.motion.x = ::AMotionEvent_getXOffset(event);
		inputEvent.motion.y = ::AMotionEvent_getYOffset(event);
		inputEvent.motion.xrel = inputEvent.motion.x;
		inputEvent.motion.yrel = inputEvent.motion.y;
		inputEvent.motion.state = ::AMotionEvent_getButtonState(event);
		inputEvent.motion.timestamp = ::AMotionEvent_getEventTime(event);
		inputEvent.button.button = ray::InputButton::MOUSE0;
	}
	break;
	case AMOTION_EVENT_ACTION_DOWN:
	{
		ray::InputEvent inputEvent;
		inputEvent.event = ray::InputEvent::TouchMotionDown;
		inputEvent.motion.x = ::AMotionEvent_getXOffset(event);
		inputEvent.motion.y = ::AMotionEvent_getYOffset(event);
		inputEvent.motion.xrel = inputEvent.motion.x;
		inputEvent.motion.yrel = inputEvent.motion.y;
		inputEvent.motion.state = ::AMotionEvent_getButtonState(event);
		inputEvent.motion.timestamp = ::AMotionEvent_getDownTime(event);
		inputEvent.button.button = ray::InputButton::MOUSE0;
	}
	break;
	case AMOTION_EVENT_ACTION_UP:
	{
		ray::InputEvent inputEvent;
		inputEvent.event = ray::InputEvent::TouchMotionUp;
		inputEvent.motion.x = ::AMotionEvent_getXOffset(event);
		inputEvent.motion.y = ::AMotionEvent_getYOffset(event);
		inputEvent.motion.xrel = inputEvent.motion.x;
		inputEvent.motion.yrel = inputEvent.motion.y;
		inputEvent.motion.state = ::AMotionEvent_getButtonState(event);
		inputEvent.motion.timestamp = ::AMotionEvent_getEventTime(event);
		inputEvent.button.button = ray::InputButton::MOUSE0;
	}
	break;
	case AMOTION_EVENT_ACTION_CANCEL:
	{
		ray::InputEvent inputEvent;
		inputEvent.event = ray::InputEvent::TouchMotionCancel;
		inputEvent.motion.x = ::AMotionEvent_getXOffset(event);
		inputEvent.motion.y = ::AMotionEvent_getYOffset(event);
		inputEvent.motion.xrel = inputEvent.motion.x;
		inputEvent.motion.yrel = inputEvent.motion.y;
		inputEvent.motion.state = ::AMotionEvent_getButtonState(event);
		inputEvent.motion.timestamp = ::AMotionEvent_getEventTime(event);
		inputEvent.button.button = ray::InputButton::MOUSE0;
	}
	break;
	}

	return 1;
}

void onAppCmd(struct android_app* app, int32_t cmd)
{
}

void android_main(android_app* app)
{
	_android_app = app;
	_android_app->onAppCmd = &onAppCmd;
	_android_app->onInputEvent = &onInputEvent;

	ray_main(0, 0);
}

#endif