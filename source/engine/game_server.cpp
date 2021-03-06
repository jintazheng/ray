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
#include <ray/game_server.h>
#include <ray/game_scene.h>
#include <ray/game_features.h>
#include <ray/game_listener.h>

_NAME_BEGIN

__ImplementSingleton(GameServer)
__ImplementSubClass(GameServer, rtti::Interface, "GameServer")

GameServer::GameServer() noexcept
	: _isActive(false)
	, _isStopping(false)
	, _isQuitRequest(false)
	, _gameApp(nullptr)
{
}

GameServer::~GameServer() noexcept
{
	this->close();
}

bool
GameServer::open() noexcept
{
	_timer = std::make_shared<Timer>();
	_timer->open();
	return true;
}

void
GameServer::close() noexcept
{
	try
	{
		if (_gameListener)
			_gameListener->onMessage("GameServer : Stopping.");

		for (auto& it : _scenes)
		{
			if (_gameListener)
				_gameListener->onMessage(util::string("GameServer : Stopping : ") + it->type_name() + " : " + it->getName());

			it->setActive(false);

			if (_gameListener)
				_gameListener->onMessage(util::string("GameServer : Stopped : ") + it->type_name() + " : " + it->getName());
		}

		for (auto& it : _features)
		{
			for (auto& scene : _scenes)
			{
				if (_gameListener)
					_gameListener->onMessage(util::string("GameServer : Stopping feature : ") + it->type_name() + " with scene : " + scene->getName());

				it->onCloseScene(scene);
			}
		}

		_scenes.clear();

		for (auto& it : _features)
		{
			if (_gameListener)
				_gameListener->onMessage(util::string("GameServer : Stopping : ") + it->type_name());

			it->setActive(false);

			if (_gameListener)
				_gameListener->onMessage(util::string("GameServer : Stopped : ") + it->type_name());
		}

		_features.clear();

		if (_gameListener)
			_gameListener->onMessage("GameServer : Stopped.");

		_isActive = false;
		_isQuitRequest = true;
	}
	catch (const std::exception& e)
	{
		_isActive = false;
		_isQuitRequest = true;

		if (_gameListener)
			_gameListener->onMessage(util::string("GameServer : except with ") + e.what());
	}
}

void
GameServer::setTimer(const TimerPtr& timer) noexcept
{
	assert(timer);
	_timer = timer;
}

const TimerPtr&
GameServer::getTimer() const noexcept
{
	return _timer;
}

void
GameServer::setGameListener(const GameListenerPtr& listener) noexcept
{
	if (_gameListener != listener)
	{
		if (_gameListener)
			_gameListener->onListenerChangeBefore();

		for (auto& it : _features)
			it->setGameListener(listener);

		for (auto& it : _scenes)
			it->setGameListener(listener);

		if (_gameListener)
			_gameListener->onListenerChangeAfter();

		_gameListener = listener;
	}
}

const GameListenerPtr&
GameServer::getGameListener() const noexcept
{
	return _gameListener;
}

bool
GameServer::isActive() const noexcept
{
	return _isActive;
}

bool
GameServer::isStopping() const noexcept
{
	return _isStopping;
}

bool
GameServer::isQuitRequest() const noexcept
{
	return _isQuitRequest;
}

bool
GameServer::openScene(const util::string& filename) noexcept
{
	assert(!filename.empty());

	try
	{
		auto scene = std::make_shared<GameScene>();
		scene->setGameListener(_gameListener);

		if (!scene->load(filename))
			return false;

		return this->addScene(scene);
	}
	catch (const exception& e)
	{
		if (_gameListener)
			_gameListener->onMessage(e.what());

		return false;
	}
}

void
GameServer::closeScene(const util::string& sceneName) noexcept
{
	assert(!sceneName.empty());

	auto scene = this->findScene(sceneName);
	if (scene)
		this->closeScene(scene);
}

GameScenePtr
GameServer::findScene(const util::string& sceneName) noexcept
{
	assert(!sceneName.empty());

	for (auto& it : _scenes)
	{
		if (it->getName() == sceneName)
			return it;
	}

	return nullptr;
}

const GameScenes&
GameServer::getScenes() const noexcept
{
	return _scenes;
}

bool
GameServer::addScene(const GameScenePtr& scene) noexcept
{
	assert(scene);
	assert(std::find(_scenes.begin(), _scenes.end(), scene) == _scenes.end());

	try
	{
		if (_gameListener)
			_gameListener->onMessage(util::string("GameServer : Scene adding : ") + scene->getName());

		if (this->isActive())
		{
			for (auto& feature : _features)
			{
				if (_gameListener)
					_gameListener->onMessage(util::string("GameServer : Scene adding : ") + scene->getName() + " with feature : " + feature->type_name());

				feature->onOpenScene(scene);
			}

			scene->setActive(true);
		}

		_scenes.push_back(scene);

		if (_gameListener)
			_gameListener->onMessage(util::string("GameServer : Scene added : ") + scene->getName());

		return true;
	}
	catch (const exception& e)
	{
		if (_gameListener)
			_gameListener->onMessage(e.what());

		return false;
	}
}

void
GameServer::closeScene(const GameScenePtr& scene) noexcept
{
	assert(scene);

	auto it = std::find(_scenes.begin(), _scenes.end(), scene);
	if (it != _scenes.end())
	{
		if (_gameListener)
			_gameListener->onMessage(util::string("GameServer : Scene removeing : ") + scene->getName());

		if (this->isActive())
		{
			for (auto& feature : _features)
				feature->onCloseScene(*it);

			(*it)->setActive(false);
		}

		_scenes.erase(it);

		if (_gameListener)
			_gameListener->onMessage(util::string("GameServer : scene Removed : ") + scene->getName());
	}
}

bool
GameServer::addFeature(GameFeaturePtr& feature) noexcept
{
	assert(feature);

	try
	{
		auto it = std::find_if(_features.begin(), _features.end(), [feature](GameFeaturePtr it) { return feature->isInstanceOf(it->rtti()); });
		if (it == _features.end())
		{
			if (_gameListener)
				_gameListener->onMessage(util::string("GameServer : Feature adding : ") + feature->type_name());

			feature->_setGameServer(this);
			feature->setGameListener(_gameListener);

			if (this->isActive())
			{
				feature->onActivate();

				for (auto& scene : _scenes)
				{
					if (_gameListener)
						_gameListener->onMessage(util::string("GameServer : Feature adding : ") + feature->type_name() + " with scene : " + scene->getName());

					feature->onOpenScene(scene);
				}
			}

			_features.push_back(feature);

			if (_gameListener)
				_gameListener->onMessage(util::string("GameServer : Feature added : ") + feature->type_name());

			return true;
		}
		else
		{
			if (_gameListener)
				_gameListener->onMessage(util::string("GameServer : Feature has already added with : ") + feature->type_name());

			return false;
		}
	}
	catch (const exception& e)
	{
		if (_gameListener)
			_gameListener->onMessage(e.what());

		return false;
	}
}

void
GameServer::removeFeature(GameFeaturePtr& feature) noexcept
{
	assert(feature);
	auto it = std::find(_features.begin(), _features.end(), feature);
	if (it != _features.end())
	{
		if (_gameListener)
			_gameListener->onMessage(util::string("GameServer : Feature removeing: ") + feature->type_name());

		(*it)->onDeactivate();
		(*it)->_setGameServer(nullptr);

		_features.erase(it);

		if (_gameListener)
			_gameListener->onMessage(util::string("GameServer : Feature removed: ") + feature->type_name());
	}
}

GameFeaturePtr
GameServer::getFeature(const rtti::Rtti& rtti) const noexcept
{
	for (auto& it : _features)
	{
		if (it->isInstanceOf(rtti))
			return it;
	}

	return nullptr;
}

GameFeaturePtr
GameServer::getFeature(const rtti::Rtti* rtti) const noexcept
{
	for (auto& it : _features)
	{
		if (it->isInstanceOf(rtti))
			return it;
	}

	return nullptr;
}

const GameFeatures&
GameServer::getGameFeatures() const noexcept
{
	return _features;
}

void
GameServer::_setGameApp(GameApplication* app) noexcept
{
	_gameApp = app;
}

GameApplication*
GameServer::getGameApp() noexcept
{
	return _gameApp;
}

bool
GameServer::sendMessage(const MessagePtr& message) noexcept
{
	try
	{
		if (this->isQuitRequest())
			return false;

		for (auto& it : _features)
			it->onMessage(message);

		for (auto& it : _scenes)
			it->sendMessage(message);

		_dispatcher.sendMessage(message);

		return true;
	}
	catch (const exception& e)
	{
		if (_gameListener)
			_gameListener->onMessage(e.what());

		return false;
	}
}

bool
GameServer::postMessage(const MessagePtr& event) noexcept
{
	_dispatcher.postMessage(event);
	return true;
}

bool
GameServer::start() noexcept
{
	if (this->isQuitRequest())
		return false;

	if (_isActive)
	{
		if (_gameListener)
			_gameListener->onMessage("GameServer : has already started.");

		return true;
	}

	if (_gameListener)
		_gameListener->onMessage("GameServer : Starting.");

	try
	{
		for (auto& it : _features)
		{
			if (_gameListener)
				_gameListener->onMessage(util::string("GameServer : Starting : ") + it->type_name());

			it->setActive(true);

			if (_gameListener)
				_gameListener->onMessage(util::string("GameServer : Started : ") + it->type_name());
		}

		for (auto& it : _features)
		{
			for (auto& scene : _scenes)
			{
				if (_gameListener)
					_gameListener->onMessage(util::string("GameServer : Starting feature : ") + it->type_name() + " with scene : " + scene->getName());

				it->onOpenScene(scene);
			}
		}

		for (auto& it : _scenes)
		{
			if (_gameListener)
				_gameListener->onMessage(util::string("GameServer : Starting scene : ") + it->getName());

			it->setActive(true);
		}

		_timer->reset();

		_isActive = true;

		if (_gameListener)
			_gameListener->onMessage("GameServer : Started.");

		return true;
	}
	catch (const exception& e)
	{
		_isQuitRequest = true;

		if (_gameListener)
			_gameListener->onMessage(util::string("GameServer : except with ") + e.what());

		return false;
	}
}

void
GameServer::stop() noexcept
{
	if (!this->isStopping())
	{
		if (_gameListener)
			_gameListener->onMessage("GameServer : stopped.");

		_isStopping = false;
	}
	else
	{
		if (_gameListener)
			_gameListener->onMessage("GameServer : has already stopped.");
	}
}

void
GameServer::update() noexcept
{
	if (this->isQuitRequest() || !this->isActive() || this->isStopping())
		return;

	try
	{
		_timer->update();

		MessagePtr event;
		while (_dispatcher.pollMessages(event))
		{
			if (!this->sendMessage(event))
				_isQuitRequest = true;
		}

		if (!_isQuitRequest)
		{
			for (auto& it : _features)
				it->onFrameBegin();

			for (auto& it : _features)
				it->onFrame();

			for (auto& it : _features)
				it->onFrameEnd();
		}
	}
	catch (const exception& e)
	{
		if (_gameListener)
			_gameListener->onMessage(e.what());

		_isQuitRequest = true;
	}
}

_NAME_END