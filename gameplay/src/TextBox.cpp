#include "TextBox.h"
#include "Game.h"

namespace gameplay
{

TextBox::TextBox() : _caretLocation(0), _lastKeypress(0), _fontSize(0), _caretImage(NULL), _passwordChar('*'), _inputMode(TEXT), _ctrlPressed(false), _shiftPressed(false)
    , _prevCaretLocation( 0.0f, 0.0f ), _limitTextToBounds(false)
{
    _canFocus = true;
}

TextBox::~TextBox()
{
}

TextBox* TextBox::create(const char* id, Theme::Style* style)
{
    TextBox* textBox = new TextBox();
    textBox->_id = id ? id : "";
    textBox->initialize("TextBox", style, NULL);
    return textBox;
}

Control* TextBox::create(Theme::Style* style, Properties* properties)
{
    TextBox* textBox = new TextBox();
    textBox->initialize("TextBox", style, properties);
    return textBox;
}

void TextBox::initialize(const char* typeName, Theme::Style* style, Properties* properties)
{
    Label::initialize(typeName, style, properties);

	if (properties)
	{
		_inputMode = getInputMode(properties->getString("inputMode"));
        _limitTextToBounds = properties->getBool("limitText", false);
	}
}

const char* TextBox::getTypeName() const
{
    return "TextBox";
}

void TextBox::addListener(Control::Listener* listener, int eventFlags)
{
    if ((eventFlags & Control::Listener::VALUE_CHANGED) == Control::Listener::VALUE_CHANGED)
    {
        GP_ERROR("VALUE_CHANGED event is not applicable to this control.");
    }

    Control::addListener(listener, eventFlags);
}

int TextBox::getLastKeypress()
{
    return _lastKeypress;
}

unsigned int TextBox::getCaretLocation() const
{
    return _caretLocation;
}

void TextBox::setCaretLocation(unsigned int index)
{
    _caretLocation = index;
    if (_caretLocation > _text.length())
        _caretLocation = (unsigned int)_text.length();
}

bool TextBox::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    if (getState() == ACTIVE) {
        switch (evt)
        {
        case Touch::TOUCH_PRESS:
            setCaretLocation(x, y);
            break;
        case Touch::TOUCH_MOVE:
            setCaretLocation(x, y);
            break;
        default:
            break;
        }
    }

    return Label::touchEvent(evt, x, y, contactIndex);
}

static bool isWhitespace(wchar_t c)
{
    switch (c)
    {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
        return true;

    default:
        return false;
    }
}

static unsigned int findNextWord(const std::wstring& text, unsigned int from, bool backwards)
{
    int pos = (int)from;
    if (backwards)
    {
        if (pos > 0)
        {
            // Moving backwards: skip all consecutive whitespace characters
            while (pos > 0 && isWhitespace(text.at(pos-1)))
                --pos;
            // Now search back to the first whitespace character
            while (pos > 0 && !isWhitespace(text.at(pos-1)))
                --pos;
        }
    }
    else
    {
        const int len = (const int)text.length();
        if (pos < len)
        {
            // Moving forward: skip all consecutive non-whitespace characters
            ++pos;
            while (pos < len && !isWhitespace(text.at(pos)))
                ++pos;
            // Now search for the first non-whitespace character
            while (pos < len && isWhitespace(text.at(pos)))
                ++pos;
        }
    }

    return (unsigned int)pos;
}

bool TextBox::keyEvent(Keyboard::KeyEvent evt, int key)
{
    switch (evt)
    {
        case Keyboard::KEY_PRESS:
        {
            switch (key)
            {
            	case Keyboard::KEY_SHIFT:
            	{
                    _shiftPressed = true;
                    break;
            	}
                case Keyboard::KEY_CTRL:
                {
                    _ctrlPressed = true;
                    break;
                }
                case Keyboard::KEY_HOME:
                {
                    _caretLocation = 0;
                    break;
                }
                case Keyboard::KEY_END:
                {
                    _caretLocation = _text.length();
                    break;
                }
                case Keyboard::KEY_DELETE:
                {
                    if (_caretLocation < _text.length())
                    {
                        int newCaretLocation;
                        if (_ctrlPressed)
                        {
                            newCaretLocation = findNextWord(getDisplayedText(), _caretLocation, false);
                        }
                        else
                        {
                            newCaretLocation = _caretLocation + 1;
                        }
                        _text.erase(_caretLocation, newCaretLocation - _caretLocation);
                        notifyListeners(Control::Listener::TEXT_CHANGED);
                    }
                    break;
                }
                case Keyboard::KEY_TAB:
                {
                    // Allow tab to move the focus forward.
                    return false;
                }
                case Keyboard::KEY_LEFT_ARROW:
                {
                    if (_caretLocation > 0)
                    {
                        if (_ctrlPressed)
                        {
                            _caretLocation = findNextWord(getDisplayedText(), _caretLocation, true);
                        }
                        else
                        {
                            --_caretLocation;
                        }
                    }
                    break;
                }
                case Keyboard::KEY_RIGHT_ARROW:
                {
                    if (_caretLocation < _text.length())
                    {
                        if (_ctrlPressed)
                        {
                            _caretLocation = findNextWord(getDisplayedText(), _caretLocation, false);
                        }
                        else
                        {
                            ++_caretLocation;
                        }
                    }
                    break;
                }
                case Keyboard::KEY_UP_ARROW:
                {
                    // TODO: Support multiline
                    break;
                }
                case Keyboard::KEY_DOWN_ARROW:
                {
                    // TODO: Support multiline
                    break;
                }
                case Keyboard::KEY_BACKSPACE:
                {
                    if (_caretLocation > 0)
                    {
                        int newCaretLocation;
                        if (_ctrlPressed)
                        {
                            newCaretLocation = findNextWord(getDisplayedText(), _caretLocation, true);
                        }
                        else
                        {
                            newCaretLocation = _caretLocation - 1;
                        }
                        _text.erase(newCaretLocation, _caretLocation - newCaretLocation);
                        _caretLocation = newCaretLocation;
                        notifyListeners(Control::Listener::TEXT_CHANGED);
                    }
                    break;
                }
            }
            break;
        }

        case Keyboard::KEY_CHAR:
        {
            switch (key)
            {
                case Keyboard::KEY_RETURN:
                    // TODO: Support multi-line
                    notifyListeners(Control::Listener::ACTIVATED);
                    break;
                case Keyboard::KEY_ESCAPE:
                    break;
                case Keyboard::KEY_BACKSPACE:
                    break;
                case Keyboard::KEY_TAB:
                    // Allow tab to move the focus forward.
                    return false;
                default:
                // allow entering only ASCII chars.
                if( isascii( key ) )
                {
                    // Insert character into string, only if our font supports this character
                    if (_shiftPressed && islower(key))
                    {
                        key = toupper(key);
                    }
                    // Insert character into string, only if our font supports this character
                    if (_font && _font->isCharacterSupported(key))
                    {
                        if (_caretLocation <= _text.length())
                        {
                            _text.insert(_caretLocation, 1, (char)key);
                            _caretLocation++;

                            if (_limitTextToBounds)
                            {
                                Control::State state = getState();
                                Font* font = getFont(state);
                                unsigned int fontSize = getFontSize(state);
                                Font::Justify textAlignment = getTextAlignment(state);

                                Rectangle textBounds;
                                font->measureText(getDisplayedText().c_str(), _bounds, fontSize, getTextDrawingFlags(state), &textBounds, textAlignment, true, true,
                                    getCharacterSpacing(state), getLineSpacing(state));

                                if (textBounds.width > _bounds.width || textBounds.height > _bounds.height)
                                {
                                    _caretLocation--;
                                    _text.erase (_caretLocation, 1);
                                }
                            }
                        }

                        notifyListeners(Control::Listener::TEXT_CHANGED);
                    }
                    break;
                }
            
                break;
            }
            break;
        }
        case Keyboard::KEY_RELEASE:
            switch (key)
            {
            	case Keyboard::KEY_SHIFT:
            	{
                    _shiftPressed = false;
                    break;
             	 }
                case Keyboard::KEY_CTRL:
                {
                    _ctrlPressed = false;
                    break;
                }
            }
    }

    _lastKeypress = key;

    return Label::keyEvent(evt, key);
}

void TextBox::controlEvent(Control::Listener::EventType evt)
{
    Label::controlEvent(evt);

    switch (evt)
    {
    case Control::Listener::FOCUS_GAINED:
        Game::getInstance()->displayKeyboard(true);
        break;

    case Control::Listener::FOCUS_LOST:
        Game::getInstance()->displayKeyboard(false);
        break;
    default:
        break;
    }
}

void TextBox::updateState(State state)
{
    Label::updateState(state);

    _fontSize = getFontSize(state);
    _caretImage = getImage("textCaret", state);
}

unsigned int TextBox::drawImages(Form* form) const
{
    Control::State state = getState();

    if (_caretImage && (state == ACTIVE || hasFocus()))
    {
        // Draw the cursor at its current location.
        const Rectangle& region = _caretImage->getRegion();
        if (!region.isEmpty())
        {
            const Theme::UVs& uvs = _caretImage->getUVs();
            Vector4 color = _caretImage->getColor();
            color.w *= _opacity;

            float caretWidth = region.width * _fontSize / region.height;

            Font* font = getFont(state);
            unsigned int fontSize = getFontSize(state);
            Vector2 point;
            font->getLocationAtIndex(getDisplayedText().c_str(), _textBounds, fontSize, &point, _caretLocation, 
                getTextAlignment(state), true, getTextDrawingFlags(state));

            SpriteBatch* batch = _style->getTheme()->getSpriteBatch();
            startBatch(form, batch);
            batch->draw(point.x - caretWidth * 0.5f, point.y, caretWidth, fontSize, uvs.u1, uvs.v1, uvs.u2, uvs.v2, color, _viewportClipBounds);
            finishBatch(form, batch);

            return 1;
        }
    }

    return 0;
}

unsigned int TextBox::drawText(Form* form) const
{
    if (_text.size() <= 0 || _viewportClipBounds.width <= 0 || _viewportClipBounds.height <= 0)
        return 0;

    // Draw the text.
    if (_font)
    {
        Control::State state = getState();
        const std::wstring displayedText = getDisplayedText();
        unsigned int fontSize = getFontSize(state);

        SpriteBatch* batch = _font->getSpriteBatch(fontSize);
        startBatch(form, batch);
        _font->drawText(displayedText.c_str(), _textBounds, _textColor, fontSize, getTextAlignment(state), true, getTextDrawingFlags(state), _viewportClipBounds,
            getCharacterSpacing(state), getLineSpacing(state));
        finishBatch(form, batch);

        return 1;
    }

    return 0;
}

void TextBox::setText(wchar_t const *text)
{
    Label::setText(text);
    if (_caretLocation > _text.length())
    {
        _caretLocation = _text.length();
    }
    notifyListeners(Control::Listener::TEXT_CHANGED);
}

void TextBox::setCaretLocation(int x, int y)
{
    Control::State state = getState();

    Vector2 point(x + _absoluteBounds.x, y + _absoluteBounds.y);

    // Get index into string and cursor location from the latest touch location.
    Font* font = getFont(state);
    unsigned int fontSize = getFontSize(state);
    Font::Justify textAlignment = getTextAlignment(state);
    Font::DrawFlags flags = getTextDrawingFlags(state);
    const std::wstring displayedText = getDisplayedText();

    int index = font->getIndexAtLocation(displayedText.c_str(), _textBounds, fontSize, point, &point,
        textAlignment, true, flags);

    if (index == -1)
    {
        // Attempt to find the nearest valid caret location.
        Rectangle textBounds;
        font->measureText(displayedText.c_str(), _textBounds, fontSize, flags, &textBounds, textAlignment, true, true);

        if (point.x > textBounds.x + textBounds.width &&
            point.y > textBounds.y + textBounds.height)
        {
            font->getLocationAtIndex(displayedText.c_str(), _textBounds, fontSize, &point, (unsigned int)_text.length(),
                textAlignment, true, flags);
            return;
        }

        if (point.x < textBounds.x)
        {
            point.x = floorf(textBounds.x);
        }
        else if (point.x > textBounds.x + textBounds.width)
        {
            point.x = ceilf(textBounds.x + textBounds.width);
        }

        if (point.y < textBounds.y)
        {
            point.y = floorf(textBounds.y);
        }
        else if (point.y > textBounds.y + textBounds.height)
        {
            Font* font = getFont(state);
            GP_ASSERT(font);
            float fontSize = getFontSize(_state);
            point.y = ceilf(textBounds.y + textBounds.height - fontSize);
        }

        index = font->getIndexAtLocation(displayedText.c_str(), _textBounds, fontSize, point, &point,
            textAlignment, true, flags);
    }

    if (index != -1)
    {
        _caretLocation = index;
    }
    else
    {
        _caretLocation = _text.length();
    }
}

void TextBox::getCaretLocation(Vector2* p)
{
    GP_ASSERT(p);

    State state = getState();
    getFont(state)->getLocationAtIndex(getDisplayedText().c_str(), _textBounds, getFontSize(state), p, _caretLocation, getTextAlignment(state), true, getTextDrawingFlags(state));
}

void TextBox::setPasswordChar(wchar_t character)
{
    _passwordChar = character;
}

wchar_t TextBox::getPasswordChar() const
{
    return _passwordChar;
}

void TextBox::setInputMode(InputMode inputMode)
{
    _inputMode = inputMode;
}

TextBox::InputMode TextBox::getInputMode() const
{
    return _inputMode;
}

TextBox::InputMode TextBox::getInputMode(const char* inputMode)
{
    if (!inputMode)
    {
        return TextBox::TEXT;
    }

    if (strcmp(inputMode, "TEXT") == 0)
    {
        return TextBox::TEXT;
    }
    else if (strcmp(inputMode, "PASSWORD") == 0)
    {
        return TextBox::PASSWORD;
    }
    else
    {
        GP_ERROR("Failed to get corresponding textbox inputmode for unsupported value '%s'.", inputMode);
    }

    // Default.
    return TextBox::TEXT;
}

std::wstring TextBox::getDisplayedText() const
{
    std::wstring displayedText;
    switch (_inputMode) {
        case PASSWORD:
            displayedText.insert((size_t)0, _text.length(), _passwordChar);
            break;

        case TEXT:
        default:
            displayedText = _text;
            break;
    }

    return displayedText;
}

void TextBox::updateBounds()
{
    if (_text.empty() && _autoSize != AUTO_SIZE_NONE && _font)
    {
        Control::updateBounds();

        if (_autoSize & AUTO_SIZE_WIDTH)
            setWidthInternal(ceilf(getBorder(NORMAL).left + getBorder(NORMAL).right + getPadding().left + getPadding().right));
        if (_autoSize & AUTO_SIZE_HEIGHT)
            setHeightInternal(ceilf(getFontSize(NORMAL) + getBorder(NORMAL).top + getBorder(NORMAL).bottom + getPadding().top + getPadding().bottom));
    }
    else
    {
        Label::updateBounds();
    }
}

}
