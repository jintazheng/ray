// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2017-2018.
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
#ifndef _H_UI_CONTROLLER_H_
#define _H_UI_CONTROLLER_H_

#include <ray/gui.h>
#include <ray/game_component.h>
#include <ray/camera_component.h>
#include <ray/light_component.h>
#include <ray/skybox_component.h>

#include "UIParams.h"
#include "UITexts.h"

#include "EditorEvents.h"

class GuiViewComponent final : public ray::GameComponent
{
	__DeclareSubClass(GuiViewComponent, ray::GameComponent)
public:
	GuiViewComponent() noexcept;
	~GuiViewComponent() noexcept;

	void setEditorEvents(const EditorEvents& delegate) noexcept;
	const EditorEvents& getEditorEvents() const noexcept;

	ray::GameComponentPtr clone() const noexcept;

private:
	void onActivate() except;
	void onDeactivate() noexcept;

	void onMessage(const ray::MessagePtr& message) except;

private:
	void showMainMenu() noexcept;
	void showStyleEditor() noexcept;
	void showLightMass() noexcept;
	void showAboutWindow() noexcept;

	void showMessage() noexcept;
	void showPopupMessage(const ray::util::string& title, const ray::util::string& message, std::size_t hash) noexcept;
	void showProcessMessage() noexcept;

	bool showFileOpenBrowse(ray::util::string::pointer path, std::uint32_t max_length, ray::util::string::const_pointer ext_name) noexcept;
	bool showFileOpenBrowse(std::vector<ray::util::string>& paths, ray::util::string::const_pointer ext_name) noexcept;
	bool showFileSaveBrowse(ray::util::string::pointer path, std::uint32_t max_length, ray::util::string::const_pointer ext_name) noexcept;
	bool showFolderSaveBrowse(ray::util::string::pointer path, std::uint32_t max_length) noexcept;

	void showImportModelBrowse() noexcept;
	void showImportAssetBrowse() noexcept;

	void showExportModelBrowse() noexcept;
	void showExportAssetBrowse() noexcept;
	void showExportMaterialBrowse() noexcept;

	void showProjectOpenBrowse() noexcept;
	void showProjectSaveBrowse() noexcept;
	void showProjectSaveAsBrowse() noexcept;

	void showHierarchyWindow() noexcept;
	void showAssetsWindow() noexcept;
	void showMaterialsWindow() noexcept;
	void showInspectorWindow() noexcept;
	void showCameraWindow() noexcept;
	void showSceneWindow() noexcept;
	void showDragImageWindow() noexcept;

	void showEditTransformWindow(ray::GameObject* object) noexcept;
	void showEditCameraWindow(ray::CameraComponent* component) noexcept;
	void showEditLightWindow(ray::LightComponent* light) noexcept;
	void showEditSkyboxWindow(ray::SkyboxComponent* light) noexcept;
	void showEditMaterialWindow(const EditorAssetItem& item) noexcept;

	void startUVMapper() noexcept;
	void startLightMass() noexcept;
	void startModelPicker(float x, float y) noexcept;

	void createLightProbe() noexcept;

	void saveLightMass() noexcept;

	void switchLangPackage(UILang::Lang type) noexcept;

private:
	GuiViewComponent(const GuiViewComponent&) = delete;
	GuiViewComponent& operator=(const GuiViewComponent&) = delete;

private:
	std::size_t _selectedShift;
	std::size_t _selectedSubset;
	std::size_t _selectedSubsetLast;

	ray::GameObject* _selectedObject;
	ray::GameObject* _selectedObjectLast;
	ray::GameObjectPtr _cube;

	EditorAssetItemPtr _selectedItem;
	EditorAssetItemPtr _selectedMaterial;
	EditorAssetItemPtr _selectedMaterialLast;

	std::vector<std::uint8_t> _selectedTextures;
	std::vector<std::uint8_t> _selectedMaterials;

	std::weak_ptr<ray::CameraComponent> _cameraComponent;

	ray::float2 _assetImageSize;
	ray::float2 _materialImageSize;

	ray::float4 _viewport;

	float _progress;

	bool _isShowWindowAll;
	bool _isShowMainMenu;
	bool _isShowLightMassWindow;
	bool _isShowHierarchyWindow;
	bool _isShowInspectorWindow;
	bool _isShowAssertWindow;
	bool _isShowMaterialWindow;
	bool _isShowCameraWindow;
	bool _isShowStyleEditor;
	bool _isShowAboutWindow;
	bool _isShowAboutWindowFirst;
	bool _isShowMessage;
	bool _isShowMessageFirst;
	bool _isShowProcessMessage;
	bool _isShowProcessMessageFirst;
	bool _isMouseHoveringCameraDock;

	LightMassType _lightMassType;

	std::string _pathProject;
	std::vector<std::string> _langs;

	std::size_t _messageHash;
	std::string _messageTitle;
	std::string _messageText;
	std::map<std::size_t, bool> _ignoreMessage;

	ray::GuiStyle _style;
	ray::GuiStyle _styleDefault;

	GuiParams _default;
	GuiParams _setting;

	EditorEvents _event;
};

#endif