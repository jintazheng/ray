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
#ifndef _H_MYGUI_TEXTBOX_H_
#define _H_MYGUI_TEXTBOX_H_

#include <ray/mygui_widget.h>

_NAME_BEGIN

class MyGuiTextBoxImpl final : public MyGuiWidget
{
	__DeclareSubClass(MyGuiTextBoxImpl, GuiWidget)
public:
	MyGuiTextBoxImpl() noexcept;
	MyGuiTextBoxImpl(MyGUI::Widget* parent) noexcept;
	virtual ~MyGuiTextBoxImpl() noexcept;

	bool create() except;

	Viewport getTextRegion() noexcept;
	void getTextSize(int& w, int& h) noexcept;

	void setCaption(const std::string& _value) noexcept;
	const std::string& getCaption() const noexcept;

	void setFontName(const std::string& _value) noexcept;
	const std::string& getFontName() noexcept;

	void setFontHeight(int _value) noexcept;
	int getFontHeight() noexcept;

	void setTextAlign(GuiWidgetAlign _value) noexcept;
	GuiWidgetAlign getTextAlign() noexcept;

	void setTextColour(const float4& value) noexcept;
	float4 getTextColour() noexcept;

	void setCaptionWithReplacing(const std::string& _value) noexcept;

	void setTextShadowColour(const float4& value) noexcept;
	float4 getTextShadowColour() noexcept;

	void setTextShadow(bool _value) noexcept;
	bool getTextShadow() noexcept;

private:
	std::string _caption;
	MyGUI::Widget* _parent;
	MyGUI::TextBox* _table;
};

class MyGuiTextBox final : public GuiTextBox
{
	__DeclareSubClass(MyGuiTextBox, GuiButton)
public:
	MyGuiTextBox() noexcept;
	MyGuiTextBox(MyGUI::Widget* parent) noexcept;
	virtual ~MyGuiTextBox() noexcept;

	Viewport getTextRegion() noexcept;
	void getTextSize(int& w, int& h) noexcept;

	void setCaption(const std::string& _value) noexcept;
	const std::string& getCaption() const noexcept;

	void setFontName(const std::string& _value) noexcept;
	const std::string& getFontName() noexcept;

	void setFontHeight(int _value) noexcept;
	int getFontHeight() noexcept;

	void setTextAlign(GuiWidgetAlign _value) noexcept;
	GuiWidgetAlign getTextAlign() noexcept;

	void setTextColour(const float4& value) noexcept;
	float4 getTextColour() noexcept;

	void setCaptionWithReplacing(const std::string& _value) noexcept;

	void setTextShadowColour(const float4& value) noexcept;
	float4 getTextShadowColour() noexcept;

	void setTextShadow(bool _value) noexcept;
	bool getTextShadow() noexcept;

private:

	MyGuiTextBoxImpl _impl;
};

_NAME_END

#endif