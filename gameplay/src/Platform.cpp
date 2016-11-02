/*
    Copyright 2016 Andrew Karpushin (reven86@gmail.com).

    This source file is a Derivative Work from orignal Gameplay3D source files.
    The Gameplay3D project is distributed under the terms of Apache 2.0 License.
    Original Gameplay3D source files can be found at https://github.com/gameplay3d/GamePlay

    Changes to orginal document were done in lines: 13-43 
*/

// Implementation of base platform-agnostic platform functionality.
#include "Base.h"
#include "Platform.h"
#include "Game.h"
#include "ScriptController.h"
#include "Form.h"

namespace gameplay
{

void Platform::touchEventInternal(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex, bool actuallyMouse)
{
    bool eventNotProcessed = actuallyMouse || !Form::touchEventInternal(evt, x, y, contactIndex);
    Game::getInstance()->touchEventInternal(evt, x, y, contactIndex, !eventNotProcessed);
}

void Platform::keyEventInternal(Keyboard::KeyEvent evt, int key)
{
    bool processed = Form::keyEventInternal(evt, key);
    Game::getInstance()->keyEventInternal(evt, key, processed);
}

bool Platform::mouseEventInternal(Mouse::MouseEvent evt, int x, int y, float wheelDelta)
{
    bool eventConsumed = Form::mouseEventInternal(evt, x, y, wheelDelta);

    if (Game::getInstance()->mouseEventInternal(evt, x, y, wheelDelta, eventConsumed))
        return true;

    return eventConsumed;
}

void Platform::gestureSwipeEventInternal(int x, int y, int direction)
{
    Game::getInstance()->gestureSwipeEventInternal(x, y, direction);
}

void Platform::gesturePinchEventInternal(int x, int y, float scale, int numberOfTouches)
{
    Game::getInstance()->gesturePinchEventInternal(x, y, scale, numberOfTouches);
}

void Platform::gestureRotationEventInternal(int x, int y, float rotation, int numberOfTouches)
{
    Game::getInstance()->gestureRotationEventInternal(x, y, rotation, numberOfTouches);
}

void Platform::gesturePanEventInternal(int x, int y, int numberOfTouches)
{
    Game::getInstance()->gesturePanEventInternal(x, y, numberOfTouches);
}

void Platform::gestureTapEventInternal(int x, int y)
{
    Game::getInstance()->gestureTapEventInternal(x, y);
}

void Platform::gestureLongTapEventInternal(int x, int y, float duration)
{
    Game::getInstance()->gestureLongTapEventInternal(x, y, duration);
}

void Platform::gestureDragEventInternal(int x, int y)
{
    Game::getInstance()->gestureDragEventInternal(x, y);
}

void Platform::gestureDropEventInternal(int x, int y)
{
    Game::getInstance()->gestureDropEventInternal(x, y);
}

void Platform::resizeEventInternal(unsigned int width, unsigned int height)
{
    Game::getInstance()->resizeEventInternal(width, height);
    Form::resizeEventInternal(width, height);
}

void Platform::gamepadEventConnectedInternal(GamepadHandle handle,  unsigned int buttonCount, unsigned int joystickCount, unsigned int triggerCount, const char* name)
{
    Gamepad::add(handle, buttonCount, joystickCount, triggerCount, name);
}

void Platform::gamepadEventDisconnectedInternal(GamepadHandle handle)
{
    Gamepad::remove(handle);
}

void Platform::gamepadButtonPressedEventInternal(GamepadHandle handle, Gamepad::ButtonMapping mapping)
{
    Gamepad* gamepad = Gamepad::getGamepad(handle);
    if (gamepad)
    {
        unsigned int newButtons = gamepad->_buttons | (1 << mapping);
        gamepad->setButtons(newButtons);
        Form::gamepadButtonEventInternal(gamepad);
    }
}

void Platform::gamepadButtonReleasedEventInternal(GamepadHandle handle, Gamepad::ButtonMapping mapping)
{
    Gamepad* gamepad = Gamepad::getGamepad(handle);
    if (gamepad)
    {
        unsigned int newButtons = gamepad->_buttons & ~(1 << mapping);
        gamepad->setButtons(newButtons);
        Form::gamepadButtonEventInternal(gamepad);
    }
}

void Platform::gamepadTriggerChangedEventInternal(GamepadHandle handle, unsigned int index, float value)
{
    Gamepad* gamepad = Gamepad::getGamepad(handle);
    if (gamepad)
    {
        gamepad->setTriggerValue(index, value);
        Form::gamepadTriggerEventInternal(gamepad, index);
    }
}

void Platform::gamepadJoystickChangedEventInternal(GamepadHandle handle, unsigned int index, float x, float y)
{
    Gamepad* gamepad = Gamepad::getGamepad(handle);
    if (gamepad)
    {
        gamepad->setJoystickValue(index, x, y);
        Form::gamepadJoystickEventInternal(gamepad, index);
    }
}

}
