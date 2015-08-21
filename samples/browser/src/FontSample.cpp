#include "FontSample.h"
#include "SamplesGame.h"

#if defined(ADD_SAMPLE)
    ADD_SAMPLE("Graphics", "Font", FontSample, 7);
#endif

#define FONT_COUNT 5

std::string _fontNames[] =
{
    "arial",
    "arial-dist.field",
    "badaboom",
    "fishfingers",
    "neuropol"
};

std::string _fontFiles[] =
{
    "res/ui/arial.gpb",
    "res/common/fonts/arial-distance.gpb",
    "res/common/fonts/badaboom.gpb",
    "res/common/fonts/fishfingers.gpb",
    "res/common/fonts/neuropol.gpb"
};

FontSample::FontSample()
    : _form(NULL), _stateBlock(NULL), _size(18), _wrap(true), _ignoreClip(false), _useViewport(true), _textFlags(Font::LEFT_TO_RIGHT), _simple(false), _alignment(Font::ALIGN_LEFT),
      _fontsCount(FONT_COUNT), _fontIndex(0), _font(NULL), _viewport(250, 100, 512, 200)
{
}

void FontSample::finalize()
{
    SAFE_RELEASE(_stateBlock);

    for (unsigned int i = 0; i < _fonts.size(); i++)
    {
        SAFE_RELEASE(_fonts[i]);
    }

    SAFE_RELEASE(_form);
}

void FontSample::initialize()
{
    // Create our render state block that will be reused across all materials
    _stateBlock = RenderState::StateBlock::create();
    _stateBlock->setCullFace(true);
    _stateBlock->setDepthTest(true);
    _stateBlock->setBlend(true);
    _stateBlock->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
    _stateBlock->setBlendDst(RenderState::BLEND_ONE_MINUS_SRC_ALPHA);

    for (unsigned int i = 0; i < _fontsCount; i++)
    {
        std::string s = _fontFiles[i].c_str();
        Font* f = Font::create(s.c_str());
        _fonts.push_back(f);
    }
    _font = _fonts[0];
    
    _sampleString = std::wstring( L"Lorem ipsum dolor sit amet, \n" \
                                L"consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n" \
                                L"Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\n" \
                                L"Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.\n" \
                                L"Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");

    // Create and listen to form.
    _form = Form::create("res/common/text.form");
    static_cast<Button*>(_form->getControl("fontButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("wrapButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("clipRectButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("reverseButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("switchClipRegionButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("simpleAdvancedButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("smallerButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("biggerButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("topLeftButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("topCenterButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("topRightButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("centerLeftButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("centerButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("centerRightButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("bottomLeftButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("bottomCenterButton"))->addListener(this, Control::Listener::CLICK);
    static_cast<Button*>(_form->getControl("bottomRightButton"))->addListener(this, Control::Listener::CLICK);
}

void FontSample::update(float elapsedTime)
{
}

void FontSample::render(float elapsedTime)
{
    // Clear the screen.
    clear(CLEAR_COLOR_DEPTH, Vector4(0, 0, 0, 1), 1.0f, 0);

    // Draw the frame rate.
    wchar_t fps[5];
    swprintf(fps, 5, L"%u", getFrameRate());

    _fonts[0]->start();

    _fonts[0]->drawText(fps, 245, 5, Vector4(0, 0.5f, 1, 1), _size);
    
    _form->draw();

    float size = (float)_font->getSize() * _size;
    if (_font != _fonts[1])
        _font->start();

    if (_simple)
    {
        // Sample simple versions of measureText, drawText.
        float w, h;
        _font->measureText(_sampleString.c_str(), _size, _textFlags, &w, &h);
        _font->drawText(_sampleString.c_str(), _viewport.x, _viewport.y, Vector4::fromColor(0xff0000ff), _size, _textFlags);
        _font->drawText(L"'", _viewport.x, _viewport.y, Vector4::fromColor(0x00ff00ff), _size);
        _font->drawText(L".", _viewport.x, _viewport.y + h, Vector4::fromColor(0x00ff00ff), _size);
        _font->drawText(L"'", _viewport.x + w, _viewport.y, Vector4::fromColor(0x00ff00ff), _size);
        _font->drawText(L".", _viewport.x + w, _viewport.y + h, Vector4::fromColor(0x00ff00ff), _size);
    }
    else
    {
        // Sample viewport versions.
        gameplay::Rectangle area;
        _font->measureText(_sampleString.c_str(), _viewport, _size, _textFlags, &area, _alignment, _wrap, _ignoreClip);

        _font->drawText(_sampleString.c_str(), _useViewport ? _viewport : area, Vector4::fromColor(0xffffffff), _size, _alignment, _wrap, _textFlags);
    
        _font->drawText(L"'", _viewport.x, _viewport.y, Vector4::fromColor(0x00ff00ff), _size);
        _font->drawText(L".", _viewport.x, _viewport.y + _viewport.height, Vector4::fromColor(0x00ff00ff), _size);
        _font->drawText(L"'", _viewport.x + _viewport.width, _viewport.y, Vector4::fromColor(0x00ff00ff), _size);
        _font->drawText(L".", _viewport.x + _viewport.width, _viewport.y + _viewport.height, Vector4::fromColor(0x00ff00ff), _size);

        _font->drawText(L"'", area.x, area.y, Vector4::fromColor(0x0000ffff), _size);
        _font->drawText(L".", area.x, area.y + area.height, Vector4::fromColor(0x0000ffff), _size);
        _font->drawText(L"'", area.x + area.width, area.y, Vector4::fromColor(0x0000ffff), _size);
        _font->drawText(L".", area.x + area.width, area.y + area.height, Vector4::fromColor(0x0000ffff), _size);
    }

    if (_font != _fonts[0])
    {
        _font->finish();
    }
    _fonts[0]->finish();

    _form->draw();
}

void FontSample::touchEvent(Touch::TouchEvent event, int x, int y, unsigned int contactIndex)
{
    _viewport.width = x - _viewport.x;
    _viewport.height = y - _viewport.y;
}

void FontSample::controlEvent(Control* control, EventType evt)
{
    const char* id = control->getId();

    if (strcmp(id, "fontButton") == 0)
    {
        _fontIndex++;
        if (_fontIndex >= _fontsCount)
        {
            _fontIndex = 0;
        }
        _font = _fonts[_fontIndex];
        std::wstring s = L"Font (" + std::wstring(_fontNames[_fontIndex].begin(),_fontNames[_fontIndex].end()) + L")";
        static_cast<Button*>(control)->setText(s.c_str());
    }
    else if (strcmp(id, "wrapButton") == 0)
    {
        _wrap = !_wrap;
        Button* wrapButton = static_cast<Button*>(control);
        if (_wrap)
            wrapButton->setText(L"Word Wrap (On)");
        else
            wrapButton->setText(L"Word Wrap (Off)");
    }
    else if (strcmp(id, "clipRectButton") == 0)
    {
        _ignoreClip = !_ignoreClip;
        Button* clipRectButton = static_cast<Button*>(control);
        if (_ignoreClip)
            clipRectButton->setText(L"Clipping (Off)");
        else
            clipRectButton->setText(L"Clipping (On)");
    }
    else if (strcmp(id, "reverseButton") == 0)
    {
        Button* reverseButton = static_cast<Button*>(control);
        switch (_textFlags)
        {
        case Font::LEFT_TO_RIGHT:
            _textFlags = Font::RIGHT_TO_LEFT;
            reverseButton->setText(L"Reverse Text (On)");
            break;
        case Font::RIGHT_TO_LEFT:
            _textFlags = Font::DRAW_VERTICAL_CCW;
            reverseButton->setText(L"Draw vertical CCW");
            break;
        case Font::DRAW_VERTICAL_CCW:
            _textFlags = Font::DRAW_VERTICAL_CW;
            reverseButton->setText(L"Draw vertical CW");
            break;
        case Font::DRAW_VERTICAL_CW:
            _textFlags = Font::LEFT_TO_RIGHT;
            reverseButton->setText(L"Reverse Text (Off)");
            break;
        default:
            break;
        }
    }
    else if (strcmp(id, "switchClipRegionButton") == 0)
    {
        _useViewport = !_useViewport;
        Button* switchClipButton = static_cast<Button*>(control);
        if (_useViewport)
            switchClipButton->setText(L"Clip Regions (Viewport)");
        else
            switchClipButton->setText(L"Clip Regions (Text Area)");
    }
    else if (strcmp(id, "simpleAdvancedButton") == 0)
    {
        _simple = !_simple;
        Button* simpleAdvancedButton = static_cast<Button*>(control);
        if (_simple)
            simpleAdvancedButton->setText(L"Font API (Simple)");
        else
            simpleAdvancedButton->setText(L"Font API (Advanced)");
    }
    else if (strcmp(id, "smallerButton") == 0)
    {
        if (_size > 8)
        {
            _size -= 2;
            Label* sizeLabel = static_cast<Label*>(_form->getControl("sizeLabel"));
            wchar_t s[20];
            swprintf(s, 20, L"Size (%u)", _size);
            sizeLabel->setText(s);
        }
    }
    else if (strcmp(id, "biggerButton") == 0)
    {
        _size += 2;
        Label* sizeLabel = static_cast<Label*>(_form->getControl("sizeLabel"));
        wchar_t s[20];
        swprintf(s, 20, L"Size (%u)", _size);
        sizeLabel->setText(s);
    }
    else if (strcmp(id, "topLeftButton") == 0)
    {
        _alignment = Font::ALIGN_TOP_LEFT;
        Label* alignmentLabel = static_cast<Label*>(_form->getControl("alignmentLabel"));
        alignmentLabel->setText(L"Align (Top-Left)");
    }
    else if (strcmp(id, "topCenterButton") == 0)
    {
        _alignment = Font::ALIGN_TOP_HCENTER;
        Label* alignmentLabel = static_cast<Label*>(_form->getControl("alignmentLabel"));
        alignmentLabel->setText(L"Align (Top-Center)");
    }
    else if (strcmp(id, "topRightButton") == 0)
    {
        _alignment = Font::ALIGN_TOP_RIGHT;
        Label* alignmentLabel = static_cast<Label*>(_form->getControl("alignmentLabel"));
        alignmentLabel->setText(L"Align (Top-Right)");
    }
    else if (strcmp(id, "centerLeftButton") == 0)
    {
        _alignment = Font::ALIGN_VCENTER_LEFT;
        Label* alignmentLabel = static_cast<Label*>(_form->getControl("alignmentLabel"));
        alignmentLabel->setText(L"Align (Center-Left)");
    }
    else if (strcmp(id, "centerButton") == 0)
    {
        _alignment = Font::ALIGN_VCENTER_HCENTER;
        Label* alignmentLabel = static_cast<Label*>(_form->getControl("alignmentLabel"));
        alignmentLabel->setText(L"Align (Center)");
    }
    else if (strcmp(id, "centerRightButton") == 0)
    {
        _alignment = Font::ALIGN_VCENTER_RIGHT;
        Label* alignmentLabel = static_cast<Label*>(_form->getControl("alignmentLabel"));
        alignmentLabel->setText(L"Align (Center-Right)");
    }
    else if (strcmp(id, "bottomLeftButton") == 0)
    {
        _alignment = Font::ALIGN_BOTTOM_LEFT;
        Label* alignmentLabel = static_cast<Label*>(_form->getControl("alignmentLabel"));
        alignmentLabel->setText(L"Align (Bottom-Left)");
    }
    else if (strcmp(id, "bottomCenterButton") == 0)
    {
        _alignment = Font::ALIGN_BOTTOM_HCENTER;
        Label* alignmentLabel = static_cast<Label*>(_form->getControl("alignmentLabel"));
        alignmentLabel->setText(L"Align (Bottom-Center)");
    }
    else if (strcmp(id, "bottomRightButton") == 0)
    {
        _alignment = Font::ALIGN_BOTTOM_RIGHT;
        Label* alignmentLabel = static_cast<Label*>(_form->getControl("alignmentLabel"));
        alignmentLabel->setText(L"Align (Bottom-Right)");
    }
}
