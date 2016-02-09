#include "Base.h"
#include "CheckBox.h"
#include "Game.h"

namespace gameplay
{

CheckBox::CheckBox() : _checked(false), _image(NULL), _textSpace(5.0f), _iconScale(1.0f)
{
}

CheckBox::~CheckBox()
{

}

CheckBox* CheckBox::create(const char* id, Theme::Style* style)
{
    CheckBox* cb = new CheckBox();
    cb->_id = id ? id : "";
    cb->initialize("CheckBox", style, NULL);
    return cb;
}

Control* CheckBox::create(Theme::Style* style, Properties* properties)
{
    CheckBox* cb = new CheckBox();
    cb->initialize("CheckBox", style, properties);
    return cb;
}

void CheckBox::initialize(const char* typeName, Theme::Style* style, Properties* properties)
{
    Button::initialize(typeName, style, properties);

    if (properties)
    {
        _checked = properties->getBool("checked");
        if (properties->exists("textSpace"))
            _textSpace = properties->getFloat("textSpace");
        if (properties->exists("iconScale"))
            _iconScale = properties->getFloat("iconScale");
    }
}

const char* CheckBox::getTypeName() const
{
    return "CheckBox";
}

bool CheckBox::isChecked()
{
    return _checked;
}

void CheckBox::setChecked(bool checked)
{
    if (_checked != checked)
    {
        _checked = checked;
        setDirty(DIRTY_STATE);
        notifyListeners(Control::Listener::VALUE_CHANGED);
    }
}

void CheckBox::addListener(Control::Listener* listener, int eventFlags)
{
    if ((eventFlags & Control::Listener::TEXT_CHANGED) == Control::Listener::TEXT_CHANGED)
    {
        GP_ERROR("TEXT_CHANGED event is not applicable to CheckBox.");
        eventFlags &= ~Control::Listener::TEXT_CHANGED;
    }

    Control::addListener(listener, eventFlags);
}

bool CheckBox::keyEvent(Keyboard::KeyEvent evt, int key)
{
    if (getState() == ACTIVE && evt == Keyboard::KEY_RELEASE && key == Keyboard::KEY_RETURN)
    {
        setChecked( !_checked );
    }

    return Button::keyEvent(evt, key);
}

void CheckBox::controlEvent(Control::Listener::EventType evt)
{
    Button::controlEvent(evt);

    switch (evt)
    {
    case Control::Listener::CLICK:
        setChecked( !_checked );
        break;
    }
}

void CheckBox::updateState(State state)
{
    Label::updateState(state);

    _image = getImage(_checked ? "checked" : "unchecked", state);
}

void CheckBox::updateBounds()
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

void CheckBox::updateAbsoluteBounds(const Vector2& offset)
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

unsigned int CheckBox::drawImages(Form* form) const
{
    if (!_image)
        return 0;

    // Left, v-center.
    // TODO: Set an alignment for icons.

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

void CheckBox::setIconScale(float scale)
{
    _iconScale = scale;
}

}
