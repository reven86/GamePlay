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
    
    if (!eventNotProcessed && evt == Touch::TOUCH_PRESS)
        evt = Touch::TOUCH_MOVE;    // little hack to make sure app updates touch cursor on first press event
    
    if (eventNotProcessed)// || evt == Touch::TOUCH_MOVE)
    {
        Game::getInstance()->touchEventInternal(evt, x, y, contactIndex);
    }
}

void Platform::keyEventInternal(Keyboard::KeyEvent evt, int key)
{
    if (!Form::keyEventInternal(evt, key))
    {
        Game::getInstance()->keyEventInternal(evt, key);
    }
}

bool Platform::mouseEventInternal(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
    bool eventConsumed = Form::mouseEventInternal(evt, x, y, wheelDelta);

    // send mouse move event even if it is consumed by form
    if( !eventConsumed || evt == Mouse::MOUSE_MOVE )
        return Game::getInstance()->mouseEventInternal(evt, x, y, wheelDelta);


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
    unsigned int newButtons = gamepad->_buttons | (1 << mapping);
    gamepad->setButtons(newButtons);
    Form::gamepadButtonEventInternal(gamepad);
}

void Platform::gamepadButtonReleasedEventInternal(GamepadHandle handle, Gamepad::ButtonMapping mapping)
{
    Gamepad* gamepad = Gamepad::getGamepad(handle);
    unsigned int newButtons = gamepad->_buttons & ~(1 << mapping);
    gamepad->setButtons(newButtons);
    Form::gamepadButtonEventInternal(gamepad);
}

void Platform::gamepadTriggerChangedEventInternal(GamepadHandle handle, unsigned int index, float value)
{
    Gamepad* gamepad = Gamepad::getGamepad(handle);
    gamepad->setTriggerValue(index, value);
    Form::gamepadTriggerEventInternal(gamepad, index);
}

void Platform::gamepadJoystickChangedEventInternal(GamepadHandle handle, unsigned int index, float x, float y)
{
    Gamepad* gamepad = Gamepad::getGamepad(handle);
    gamepad->setJoystickValue(index, x, y);
    Form::gamepadJoystickEventInternal(gamepad, index);
}

}
