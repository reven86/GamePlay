#include "InputSample.h"
#include "SamplesGame.h"

#if defined(ADD_SAMPLE)
    ADD_SAMPLE("Input", "Basic Input", InputSample, 1);
#endif

/**
 * Returns the string representation of the given key.
 */
static const wchar_t* keyString(int key);

InputSample::InputSample()
    :  _mouseString(L"No Mouse"), _font(NULL), _inputSampleControls(NULL), _mouseWheel(0), _crosshair(NULL),
       _scene(NULL), _formNode(NULL), _formNodeParent(NULL)
{
}

void InputSample::initialize()
{
    setMultiTouch(true);

    // Load font
    _font = Font::create("res/ui/arial.gpb");
    assert(_font);

    // Reuse part of the gamepad texture as the crosshair in this sample.
    _crosshair = SpriteBatch::create("res/png/gamepad.png");
    _crosshairDstRect.set(0, 0, 256, 256);
    _crosshairSrcRect.set(256, 0, 256, 256);
    _crosshairLowerLimit.set(-_crosshairSrcRect.width / 2.0f, -_crosshairSrcRect.height / 2.0f);
    _crosshairUpperLimit.set((float)getWidth(), (float)getHeight());
    _crosshairUpperLimit += _crosshairLowerLimit;

    // Create input sample controls
    _keyboardState = false;
    _inputSampleControls = Form::create("res/common/inputs.form");
    static_cast<Button*>(_inputSampleControls->getControl("showKeyboardButton"))->addListener(this, Listener::CLICK);
    static_cast<Button*>(_inputSampleControls->getControl("captureMouseButton"))->addListener(this, Listener::CLICK);

    if (!hasMouse())
    {
        static_cast<Button*>(_inputSampleControls->getControl("captureMouseButton"))->setVisible(false);
    }
    _inputSampleControls->getControl("restoreMouseLabel")->setVisible(false);

    _mousePoint.set(-100, -100);

    // Create a 3D form that responds to raw sensor inputs.
    // For this, we will need a scene with a camera node.
    Camera* camera = Camera::createPerspective(45.0f, (float)getWidth() / (float)getHeight(), 0.25f, 100.0f);
    _scene = Scene::create();
    Node* cameraNode = _scene->addNode("Camera");
    cameraNode->setCamera(camera);
    _scene->setActiveCamera(camera);
    SAFE_RELEASE(camera);
    _formNodeParent = _scene->addNode("FormParent");
    _formNode = Node::create("Form");
    _formNodeParent->addChild(_formNode);
    Theme* theme = _inputSampleControls->getTheme();
    Form* form = Form::create("testForm", theme->getStyle("basicContainer"), Layout::LAYOUT_ABSOLUTE);
    form->setSize(225, 100);
    Label* label = Label::create("sensorLabel", theme->getStyle("iconNoBorder"));
    label->setPosition(25, 15);
    label->setSize(175, 50);
    label->setText(L"Raw sensor response (accel/gyro)");
    form->addControl(label);
    label->release();
    _formNode->setScale(0.0015f, 0.0015f, 1.0f);
    _formNodeRestPosition.set(0, 0, -1.5f);
    _formNodeParent->setTranslation(_formNodeRestPosition);
    _formNode->setTranslation(-0.2f, -0.2f, 0);
    _formNode->setDrawable(form);
    form->release();
}

void InputSample::finalize()
{
    setMouseCaptured(false);
    if (_keyboardState)
    {
        displayKeyboard(false);
    }

    SAFE_RELEASE(_scene);
    SAFE_RELEASE(_formNode);
    SAFE_RELEASE(_inputSampleControls);
    SAFE_DELETE(_crosshair);
    SAFE_RELEASE(_font);
}

void InputSample::update(float elapsedTime)
{
    if (hasAccelerometer())
    {
        Vector3 accelRaw, gyroRaw;
        getSensorValues(&accelRaw.x, &accelRaw.y, &accelRaw.z, &gyroRaw.x, &gyroRaw.y, &gyroRaw.z);

        // Adjust for landscape mode
        float temp = accelRaw.x;
        accelRaw.x = -accelRaw.y;
        accelRaw.y = temp;
        temp = gyroRaw.x;
        gyroRaw.x = -gyroRaw.y;
        gyroRaw.y = temp;

        // Respond to raw accelerometer inputs
        Vector3 position;
        _formNodeParent->getTranslation(&position);
        position.smooth(_formNodeRestPosition - accelRaw*0.04f, elapsedTime, 100);
        _formNodeParent->setTranslation(position);

        // Respond to raw gyroscope inputs
        Vector3 rotation;
        float angle = _formNodeParent->getRotation(&rotation);
        rotation *= angle;
        rotation.smooth(gyroRaw*(-0.18f), elapsedTime, 220);
        angle = rotation.length();
        rotation.normalize();
        _formNodeParent->setRotation(rotation, angle);
    }
}

void InputSample::render(float elapsedTime)
{
    // Clear the color and depth buffers
    clear(CLEAR_COLOR_DEPTH, Vector4::zero(), 1.0f, 0);

    _inputSampleControls->draw();

    // Draw text
    float fontSize = 18;
    Vector4 fontColor(1.0f, 1.0f, 1.0f, 1.0f);
    float width, height;
    wchar_t buffer[50];

    _font->start();
    
    if (isMouseCaptured())
    {
        // Draw crosshair at current offest w.r.t. center of screen
        _crosshair->start();
        _crosshair->draw(_crosshairDstRect, _crosshairSrcRect);
        _crosshair->finish();
    }
    else
    {
        for (std::list<TouchPoint>::const_iterator it = _touchPoints.begin(); it != _touchPoints.end(); ++it)
        {
            swprintf(buffer, L"T_%u(%d,%d)", it->_id, (int)it->_coord.x, (int)it->_coord.y);
            _font->measureText(buffer, fontSize, Font::LEFT_TO_RIGHT, &width, &height);
            int x = it->_coord.x - (int)(width * 0.5f);
            int y = it->_coord.y - (int)(height * 0.5f);
            _font->drawText(buffer, x, y, fontColor, fontSize);
        }

        // Mouse
        swprintf(buffer, L"M(%d,%d)", (int)_mousePoint.x, (int)_mousePoint.y);
        _font->measureText(buffer, fontSize, Font::LEFT_TO_RIGHT, &width, &height);
        int x = _mousePoint.x - (int)(width * 0.5f);
        int y = _mousePoint.y - (int)(height * 0.5f);
        _font->drawText(buffer, x, y, fontColor, fontSize);
        if (!_keyboardState && _mouseString.length() > 0)
        {
            int y = getHeight() - fontSize;
            _font->drawText(_mouseString.c_str(), 0, y, fontColor, fontSize);
        }
        if (_mouseWheel)
        {
            swprintf(buffer, L"%d", _mouseWheel);
            _font->measureText(buffer, fontSize, Font::LEFT_TO_RIGHT, &width, &height);
            int x = _mouseWheelPoint.x - (int)(width * 0.5f);
            int y = _mouseWheelPoint.y + 4;
            _font->drawText(buffer, x, y, fontColor, fontSize);
        }
    }

    // Pressed keys
    if (_keyboardString.length() > 0)
    {
        _font->drawText(_keyboardString.c_str(), 0, 0, fontColor, fontSize);
    }
    
    // Printable symbols typed
    if (_symbolsString.length() > 0)
    {
        _font->drawText(_symbolsString.c_str(), 0, 18, fontColor, fontSize);
    }

    // Held keys
    if (!_downKeys.empty())
    {
        std::wstring displayKeys;
        for (std::set<int>::const_iterator i = _downKeys.begin(); i != _downKeys.end(); ++i)
        {
            const wchar_t* str = keyString(*i);
            displayKeys.append(str);
        }
        if (!displayKeys.empty())
        {
            _font->measureText(displayKeys.c_str(), 18.0f, Font::LEFT_TO_RIGHT, &width, &height);
            int x = Game::getInstance()->getWidth() - width;
            int y = 0;
            _font->drawText(displayKeys.c_str(), x, y, fontColor, fontSize);
        }
    }

    // Draw the accelerometer values in the bottom right corner.
    static float pitch, roll;
    static float accelerometerDrawRate = 1000.0f;
    accelerometerDrawRate += elapsedTime;
    if (accelerometerDrawRate > 100.0f)
    {
        accelerometerDrawRate = 0.0f;
        getAccelerometerValues(&pitch, &roll);
    }
    if (hasAccelerometer() && !_keyboardState)
    {
        _formNode->getDrawable()->draw();

        swprintf(buffer, L"Pitch: %f   Roll: %f", pitch, roll);
        _font->measureText(buffer, 18, Font::LEFT_TO_RIGHT, &width, &height);
        _font->drawText(buffer, getWidth() - width, getHeight() - height, fontColor, fontSize);
    }
    _font->finish();
}

bool InputSample::drawScene(Node* node)
{
    Drawable* drawable = node->getDrawable(); 
    if (drawable)
        drawable->draw();
    return true;
}

void InputSample::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    TouchPoint* tp = NULL;
    // Not optimal, however we expect the list size to be very small (<10)
    for (std::list<TouchPoint>::iterator it = _touchPoints.begin(); it != _touchPoints.end(); ++it)
    {
        if (it->_id == contactIndex)
        {
            tp = &(*it); // (seems redundant, however due to STD)
            break;
        }
    }

    // Add a new touch point if not found above
    if (!tp)
    {
        tp = new TouchPoint();
        tp->_id = contactIndex;
        _touchPoints.push_back(*tp);
    }

    // Update the touch point
    tp->_coord.x = x;
    tp->_coord.y = y;
    tp->_isStale = false; // (could be overwritten below)

    switch (evt)
    {
    case Touch::TOUCH_PRESS:
        // Purge all stale touch points
        for (std::list<TouchPoint>::iterator it = _touchPoints.begin(); it != _touchPoints.end(); )
        {
            if (it->_isStale)
            {
                it = _touchPoints.erase(it);
            }
            else
            {
                ++it;
            }
        }

        if (x < 30 && y < 30)
        {
            displayKeyboard(true);
        }
        break;
    case Touch::TOUCH_RELEASE:
        // Mark the current touch point as stale
        if (tp)
        {
            tp->_isStale = true;
        }
        break;
    case Touch::TOUCH_MOVE:
        break;
    };
}

bool InputSample::mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
    _mousePoint.set(x, y);
    _mouseString.clear();

    switch (evt)
    {
    case Mouse::MOUSE_PRESS_LEFT_BUTTON:
        _mouseString.append(L"MOUSE_PRESS_LEFT_BUTTON");
        break;
    case Mouse::MOUSE_RELEASE_LEFT_BUTTON:
        _mouseString.append(L"MOUSE_RELEASE_LEFT_BUTTON");
        break;
    case Mouse::MOUSE_PRESS_MIDDLE_BUTTON:
        _mouseString.append(L"MOUSE_PRESS_MIDDLE_BUTTON");
        break;
    case Mouse::MOUSE_RELEASE_MIDDLE_BUTTON:
        _mouseString.append(L"MOUSE_RELEASE_MIDDLE_BUTTON");
        break;
    case Mouse::MOUSE_PRESS_RIGHT_BUTTON:
        _mouseString.append(L"MOUSE_PRESS_RIGHT_BUTTON");
        break;
    case Mouse::MOUSE_RELEASE_RIGHT_BUTTON:
        _mouseString.append(L"MOUSE_RELEASE_RIGHT_BUTTON");
        break;
    case Mouse::MOUSE_MOVE:
        _mouseString.append(L"MOUSE_MOVE");
        if (isMouseCaptured())
        {
            // Control crosshair from captured mouse
            _crosshairDstRect.setPosition(_crosshairDstRect.x + x, _crosshairDstRect.y + y);

            // Use screen limits to clamp the crosshair position
            Vector2 pos(_crosshairDstRect.x, _crosshairDstRect.y);
            pos.clamp(_crosshairLowerLimit, _crosshairUpperLimit);
            _crosshairDstRect.setPosition(pos.x, pos.y);
        }
        break;
    case Mouse::MOUSE_WHEEL:
        _mouseString.append(L"MOUSE_WHEEL");
        _mouseWheelPoint.x = x;
        _mouseWheelPoint.y = y;
        _mouseWheel = wheelDelta;
        break;
    }
    return false;
}

void InputSample::keyEvent(Keyboard::KeyEvent evt, int key)
{
    switch (evt)
    {
    case Keyboard::KEY_PRESS:
        _keyboardString.clear();
        _keyboardString.append(keyString(key));
        _keyboardString.append(L" pressed");
        _downKeys.insert(key);

        if (key == Keyboard::KEY_ESCAPE)
        {
            _symbolsString.clear();
        }

        if (key == Keyboard::KEY_SPACE && hasMouse())
        {
            setCaptured(false);
        }
        break;
    case Keyboard::KEY_RELEASE:
        _keyboardString.clear();
        _keyboardString.append(keyString(key));
        _keyboardString.append(L" released");
        _downKeys.erase(key);
        break;
    case Keyboard::KEY_CHAR:
        if (key == Keyboard::KEY_BACKSPACE)
        {
            if (_symbolsString.size() > 0)
                _symbolsString.resize((_symbolsString.size() - 1));
        }
        else
        {
            _symbolsString.append(1, (char)(0xFF & key));
        }
        break;
    };
}

void InputSample::controlEvent(Control* control, EventType evt)
{
    if (strcmp(control->getId(), "showKeyboardButton") == 0)
    {
        _keyboardState = !_keyboardState;
        displayKeyboard(_keyboardState);
        static_cast<Button*>(_inputSampleControls->getControl("showKeyboardButton"))->setText(_keyboardState ? L"Hide virtual keyboard" : L"Show virtual keyboard");
    }
    else if (strcmp(control->getId(), "captureMouseButton") == 0 && hasMouse())
    {
        setCaptured(true);
    }
}

void InputSample::setCaptured(bool captured)
{
    setMouseCaptured(captured);

    if (!captured || isMouseCaptured())
    {
        _inputSampleControls->getControl("showKeyboardButton")->setVisible(!captured);
        _inputSampleControls->getControl("captureMouseButton")->setVisible(!captured);
        _inputSampleControls->getControl("restoreMouseLabel")->setVisible(captured);
    }

    if (captured)
    {
        _crosshairDstRect.setPosition(
            (float)getWidth()/2.0f + _crosshairLowerLimit.x,
            (float)getHeight()/2.0f + _crosshairLowerLimit.y);
    }
}
const wchar_t* keyString(int key)
{
    // This function is helpful for finding collisions in the Keyboard::Key enum.
    switch (key)
    {
    case Keyboard::KEY_NONE:
        return L"NONE";
    case Keyboard::KEY_PAUSE:
        return L"PAUSE";
    case Keyboard::KEY_SCROLL_LOCK:
        return L"SCROLL_LOCK";
    case Keyboard::KEY_PRINT:
        return L"PRINT";
    case Keyboard::KEY_SYSREQ:
        return L"SYSREQ";
    case Keyboard::KEY_BREAK:
        return L"BREAK";
    case Keyboard::KEY_ESCAPE:
        return L"ESCAPE";
    case Keyboard::KEY_BACKSPACE:
        return L"BACKSPACE";
    case Keyboard::KEY_TAB:
        return L"TAB";
    case Keyboard::KEY_BACK_TAB:
        return L"BACK_TAB";
    case Keyboard::KEY_RETURN:
        return L"RETURN";
    case Keyboard::KEY_CAPS_LOCK:
        return L"CAPS_LOCK";
    case Keyboard::KEY_SHIFT:
        return L"SHIFT";
    case Keyboard::KEY_CTRL:
        return L"CTRL";
    case Keyboard::KEY_ALT:
        return L"ALT";
    case Keyboard::KEY_MENU:
        return L"MENU";
    case Keyboard::KEY_HYPER:
        return L"HYPER";
    case Keyboard::KEY_INSERT:
        return L"INSERT";
    case Keyboard::KEY_HOME:
        return L"HOME";
    case Keyboard::KEY_PG_UP:
        return L"PG_UP";
    case Keyboard::KEY_DELETE:
        return L"DELETE";
    case Keyboard::KEY_END:
        return L"END";
    case Keyboard::KEY_PG_DOWN:
        return L"PG_DOWN";
    case Keyboard::KEY_LEFT_ARROW:
        return L"LEFT_ARROW";
    case Keyboard::KEY_RIGHT_ARROW:
        return L"RIGHT_ARROW";
    case Keyboard::KEY_UP_ARROW:
        return L"UP_ARROW";
    case Keyboard::KEY_DOWN_ARROW:
        return L"DOWN_ARROW";
    case Keyboard::KEY_NUM_LOCK:
        return L"NUM_LOCK";
    case Keyboard::KEY_KP_PLUS:
        return L"KP_PLUS";
    case Keyboard::KEY_KP_MINUS:
        return L"KP_MINUS";
    case Keyboard::KEY_KP_MULTIPLY:
        return L"KP_MULTIPLY";
    case Keyboard::KEY_KP_DIVIDE:
        return L"KP_DIVIDE";
    case Keyboard::KEY_KP_ENTER:
        return L"KP_ENTER";
    case Keyboard::KEY_KP_HOME:
        return L"KP_HOME";
    case Keyboard::KEY_KP_UP:
        return L"KP_UP";
    case Keyboard::KEY_KP_PG_UP:
        return L"KP_PG_UP";
    case Keyboard::KEY_KP_LEFT:
        return L"KP_LEFT";
    case Keyboard::KEY_KP_FIVE:
        return L"KP_FIVE";
    case Keyboard::KEY_KP_RIGHT:
        return L"KP_RIGHT";
    case Keyboard::KEY_KP_END:
        return L"KP_END";
    case Keyboard::KEY_KP_DOWN:
        return L"KP_DOWN";
    case Keyboard::KEY_KP_PG_DOWN:
        return L"KP_PG_DOWN";
    case Keyboard::KEY_KP_INSERT:
        return L"KP_INSERT";
    case Keyboard::KEY_KP_DELETE:
        return L"KP_DELETE";
    case Keyboard::KEY_F1:
        return L"F1";
    case Keyboard::KEY_F2:
        return L"F2";
    case Keyboard::KEY_F3:
        return L"F3";
    case Keyboard::KEY_F4:
        return L"F4";
    case Keyboard::KEY_F5:
        return L"F5";
    case Keyboard::KEY_F6:
        return L"F6";
    case Keyboard::KEY_F7:
        return L"F7";
    case Keyboard::KEY_F8:
        return L"F8";
    case Keyboard::KEY_F9:
        return L"F9";
    case Keyboard::KEY_F10:
        return L"F10";
    case Keyboard::KEY_F11:
        return L"F11";
    case Keyboard::KEY_F12:
        return L"F12";
    case Keyboard::KEY_SPACE:
        return L"SPACE";
    case Keyboard::KEY_EXCLAM:
        return L"!";
    case Keyboard::KEY_QUOTE:
        return L"\"";
    case Keyboard::KEY_NUMBER:
        return L"#";
    case Keyboard::KEY_DOLLAR:
        return L"$";
    case Keyboard::KEY_PERCENT:
        return L"%";
    case Keyboard::KEY_CIRCUMFLEX:
        return L"^";
    case Keyboard::KEY_AMPERSAND:
        return L"&";
    case Keyboard::KEY_APOSTROPHE:
        return L"'";
    case Keyboard::KEY_LEFT_PARENTHESIS:
        return L"(";
    case Keyboard::KEY_RIGHT_PARENTHESIS:
        return L")";
    case Keyboard::KEY_ASTERISK:
        return L"*";
    case Keyboard::KEY_PLUS:
        return L"+";
    case Keyboard::KEY_COMMA:
        return L",";
    case Keyboard::KEY_MINUS:
        return L"-";
    case Keyboard::KEY_PERIOD:
        return L".";
    case Keyboard::KEY_SLASH:
        return L"/";
    case Keyboard::KEY_ZERO:
        return L"0";
    case Keyboard::KEY_ONE:
        return L"1";
    case Keyboard::KEY_TWO:
        return L"2";
    case Keyboard::KEY_THREE:
        return L"3";
    case Keyboard::KEY_FOUR:
        return L"4";
    case Keyboard::KEY_FIVE:
        return L"5";
    case Keyboard::KEY_SIX:
        return L"6";
    case Keyboard::KEY_SEVEN:
        return L"7";
    case Keyboard::KEY_EIGHT:
        return L"8";
    case Keyboard::KEY_NINE:
        return L"9";
    case Keyboard::KEY_COLON:
        return L":";
    case Keyboard::KEY_SEMICOLON:
        return L";";
    case Keyboard::KEY_LESS_THAN:
        return L"<";
    case Keyboard::KEY_EQUAL:
        return L"=";
    case Keyboard::KEY_GREATER_THAN:
        return L">";
    case Keyboard::KEY_QUESTION:
        return L"?";
    case Keyboard::KEY_AT:
        return L"@";
    case Keyboard::KEY_CAPITAL_A:
        return L"A";
    case Keyboard::KEY_CAPITAL_B:
        return L"B";
    case Keyboard::KEY_CAPITAL_C:
        return L"C";
    case Keyboard::KEY_CAPITAL_D:
        return L"D";
    case Keyboard::KEY_CAPITAL_E:
        return L"E";
    case Keyboard::KEY_CAPITAL_F:
        return L"F";
    case Keyboard::KEY_CAPITAL_G:
        return L"G";
    case Keyboard::KEY_CAPITAL_H:
        return L"H";
    case Keyboard::KEY_CAPITAL_I:
        return L"I";
    case Keyboard::KEY_CAPITAL_J:
        return L"J";
    case Keyboard::KEY_CAPITAL_K:
        return L"K";
    case Keyboard::KEY_CAPITAL_L:
        return L"L";
    case Keyboard::KEY_CAPITAL_M:
        return L"M";
    case Keyboard::KEY_CAPITAL_N:
        return L"N";
    case Keyboard::KEY_CAPITAL_O:
        return L"O";
    case Keyboard::KEY_CAPITAL_P:
        return L"P";
    case Keyboard::KEY_CAPITAL_Q:
        return L"Q";
    case Keyboard::KEY_CAPITAL_R:
        return L"R";
    case Keyboard::KEY_CAPITAL_S:
        return L"S";
    case Keyboard::KEY_CAPITAL_T:
        return L"T";
    case Keyboard::KEY_CAPITAL_U:
        return L"U";
    case Keyboard::KEY_CAPITAL_V:
        return L"V";
    case Keyboard::KEY_CAPITAL_W:
        return L"W";
    case Keyboard::KEY_CAPITAL_X:
        return L"X";
    case Keyboard::KEY_CAPITAL_Y:
        return L"Y";
    case Keyboard::KEY_CAPITAL_Z:
        return L"Z";
    case Keyboard::KEY_LEFT_BRACKET:
        return L"[";
    case Keyboard::KEY_BACK_SLASH:
        return L"\\";
    case Keyboard::KEY_RIGHT_BRACKET:
        return L"]";
    case Keyboard::KEY_UNDERSCORE:
        return L"_";
    case Keyboard::KEY_GRAVE:
        return L"`";
    case Keyboard::KEY_A:
        return L"a";
    case Keyboard::KEY_B:
        return L"b";
    case Keyboard::KEY_C:
        return L"c";
    case Keyboard::KEY_D:
        return L"d";
    case Keyboard::KEY_E:
        return L"e";
    case Keyboard::KEY_F:
        return L"f";
    case Keyboard::KEY_G:
        return L"g";
    case Keyboard::KEY_H:
        return L"h";
    case Keyboard::KEY_I:
        return L"i";
    case Keyboard::KEY_J:
        return L"j";
    case Keyboard::KEY_K:
        return L"k";
    case Keyboard::KEY_L:
        return L"l";
    case Keyboard::KEY_M:
        return L"m";
    case Keyboard::KEY_N:
        return L"n";
    case Keyboard::KEY_O:
        return L"o";
    case Keyboard::KEY_P:
        return L"p";
    case Keyboard::KEY_Q:
        return L"q";
    case Keyboard::KEY_R:
        return L"r";
    case Keyboard::KEY_S:
        return L"s";
    case Keyboard::KEY_T:
        return L"t";
    case Keyboard::KEY_U:
        return L"u";
    case Keyboard::KEY_V:
        return L"v";
    case Keyboard::KEY_W:
        return L"w";
    case Keyboard::KEY_X:
        return L"x";
    case Keyboard::KEY_Y:
        return L"y";
    case Keyboard::KEY_Z:
        return L"z";
    case Keyboard::KEY_LEFT_BRACE:
        return L"{";
    case Keyboard::KEY_BAR:
        return L"|";
    case Keyboard::KEY_RIGHT_BRACE:
        return L"}";
    case Keyboard::KEY_TILDE:
        return L"~";
    case Keyboard::KEY_EURO:
        return L"EURO";
    case Keyboard::KEY_POUND:
        return L"POUND";
    case Keyboard::KEY_YEN:
        return L"YEN";
    case Keyboard::KEY_MIDDLE_DOT:
        return L"MIDDLE DOT";
    case Keyboard::KEY_SEARCH:
        return L"SEARCH";
    default:
        return L"";
    };
    return L"";
}
