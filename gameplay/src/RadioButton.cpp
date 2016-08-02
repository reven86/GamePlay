/*
    Copyright 2016 Andrew Karpushin (reven86@gmail.com).

    This source file is a Derivative Work from orignal Gameplay3D source files.
    The Gameplay3D project is distributed under the terms of Apache 2.0 License.
    Original Gameplay3D source files can be found at https://github.com/gameplay3d/GamePlay

    Changes to orginal document were done in lines: 8-216.
*/

#include "Base.h"
#include "RadioButton.h"

namespace gameplay
{
static std::vector<RadioButton*> __radioButtons;

RadioButton::RadioButton() : _selected(false), _image(NULL), _iconScale(1.0f)
{
}

RadioButton::~RadioButton()
{
    // Remove this RadioButton from the global list.
    std::vector<RadioButton*>::iterator it = std::find(__radioButtons.begin(), __radioButtons.end(), this);
    if (it != __radioButtons.end())
    {
        __radioButtons.erase(it);
    }
}

RadioButton* RadioButton::create(const char* id, Theme::Style* style)
{
    RadioButton* rb = new RadioButton();
    rb->_id = id ? id : "";
    rb->initialize("RadioButton", style, NULL);

    __radioButtons.push_back(rb);

    return rb;
}

Control* RadioButton::create(Theme::Style* style, Properties* properties)
{
    RadioButton* rb = new RadioButton();
    rb->initialize("RadioButton", style, properties);

    __radioButtons.push_back(rb);

    return rb;
}

void RadioButton::initialize(const char* typeName, Theme::Style* style, Properties* properties)
{
    Button::initialize(typeName, style, properties);

    if (properties)
    {
        if (properties->getBool("selected"))
        {
            RadioButton::clearSelected(_groupId);
            _selected = true;
        }

        const char* groupId = properties->getString("group");
        if (groupId)
        {
            _groupId = groupId;
        }

        if (properties->exists("iconScale"))
            _iconScale = properties->getFloat("iconScale");
    }
}

const char* RadioButton::getTypeName() const
{
    return "RadioButton";
}

bool RadioButton::isSelected() const
{
    return _selected;
}

void RadioButton::setSelected(bool selected)
{
    if (selected)
        RadioButton::clearSelected(_groupId);

    if (selected != _selected)
    {
        _selected = selected;
        setDirty(DIRTY_STATE);
        notifyListeners(Control::Listener::VALUE_CHANGED);
    }
}

void RadioButton::addListener(Control::Listener* listener, int eventFlags)
{
    if ((eventFlags & Control::Listener::TEXT_CHANGED) == Control::Listener::TEXT_CHANGED)
    {
        GP_ERROR("TEXT_CHANGED event is not applicable to RadioButton.");
    }

    Control::addListener(listener, eventFlags);
}

void RadioButton::clearSelected(const std::string& groupId)
{
    std::vector<RadioButton*>::const_iterator it;
    for (it = __radioButtons.begin(); it < __radioButtons.end(); ++it)
    {
        RadioButton* radioButton = *it;
        GP_ASSERT(radioButton);
        if (groupId == radioButton->_groupId)
        {
            radioButton->setSelected(false);
        }
    }
}

bool RadioButton::keyEvent(Keyboard::KeyEvent evt, int key)
{
    if (getState() == ACTIVE && evt == Keyboard::KEY_RELEASE && key == Keyboard::KEY_RETURN && !_selected)
    {
        RadioButton::clearSelected(_groupId);
        _selected = true;
        notifyListeners(Control::Listener::VALUE_CHANGED);
    }

    return Button::keyEvent(evt, key);
}

void RadioButton::controlEvent(Control::Listener::EventType evt)
{
    Button::controlEvent(evt);

    switch (evt)
    {
    case Control::Listener::CLICK:
        if (!_selected)
        {
            RadioButton::clearSelected(_groupId);
            _selected = true;
            notifyListeners(Control::Listener::VALUE_CHANGED);
        }
        break;
    }
}

void RadioButton::updateState(State state)
{
    Label::updateState(state);

    _image = getImage(_selected ? "selected" : "unselected", state);
}

void RadioButton::updateBounds()
{
    Label::updateBounds();

    const Rectangle& region = _image ? _image->getRegion() : Rectangle(0, 0);
    Vector2 size(region.width, region.height);

    size *= _iconScale;

    if (_autoSize & AUTO_SIZE_HEIGHT)
    {
        // Text-only width was already measured in Label::update - append image
        const Theme::Border& border = getBorder(NORMAL);
        const Theme::Border& padding = getPadding();
        setHeightInternal(std::max(size.y, _bounds.height) + border.top + border.bottom + padding.top + padding.bottom);
    }

    if ((_autoSize & AUTO_SIZE_WIDTH) != 0 && _font)
    {
        // Text-only width was already measured in Label::update - append image
        setWidthInternal(size.x + 5 + _bounds.width);
    }
}

void RadioButton::updateAbsoluteBounds(const Vector2& offset)
{
    Control::updateAbsoluteBounds(offset);

    float oldTextWidth = _textBounds.width;
    _textBounds.set(_viewportBounds.x, _viewportBounds.y, _viewportBounds.width, _viewportBounds.height);

    if (_image)
    {
        float delta = _image->getRegion().width * _iconScale + 5;
        _textBounds.x += delta;
        _textBounds.width -= delta;

        if ((_autoSize & AUTO_SIZE_HEIGHT) != 0 && (_autoSize & AUTO_SIZE_WIDTH) == 0 && oldTextWidth != _textBounds.width)
        {
            // text bounds have been changed and word wrapping is applied, need to recalculate control's height
            setDirty(DIRTY_BOUNDS);
        }
    }
}

unsigned int RadioButton::drawImages(Form* form) const
{
    if (!_image)
        return 0;

    // Left, v-center.
    // TODO: Set an alignment for radio button images.   
    const Rectangle& region = _image->getRegion();
    const Theme::UVs& uvs = _image->getUVs();
    Vector4 color = _image->getColor();
    color.w *= _opacity;

    Vector2 pos(_viewportBounds.x, _viewportBounds.y);

    SpriteBatch* batch = _style->getTheme()->getSpriteBatch();
    startBatch(form, batch);
    batch->draw(pos.x, pos.y, region.width * _iconScale, region.height * _iconScale, uvs.u1, uvs.v1, uvs.u2, uvs.v2, color, _viewportClipBounds);
    finishBatch(form, batch);

    return 1;
}

void RadioButton::setGroupId(const char* groupId)
{
    _groupId = groupId;
}

const char* RadioButton::getGroupId() const
{
    return _groupId.c_str();
}

void RadioButton::setIconScale(float scale)
{
    _iconScale = scale;
}

}
