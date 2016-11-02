#include "GestureSample.h"
#include "SamplesGame.h"

#include <sstream>

// Linux and windows don't support gestures right now
#if defined(__ANDROID__) || defined(__APPLE__) 
#if defined(ADD_SAMPLE)
    ADD_SAMPLE("Input", "Gestures", GestureSample, 2);
#endif
#endif

GestureSample::GestureSample()
    : _font(NULL)
{
}

void GestureSample::initialize()
{
    setMultiTouch(true);

    // Load font
    _font = Font::create("res/ui/arial.gpb");
    assert(_font);

    bool anySupported = false;
    if (isGestureSupported(Gesture::GESTURE_TAP))
    {
        anySupported = true;
        registerGesture(Gesture::GESTURE_TAP);
        GP_ASSERT(isGestureRegistered(Gesture::GESTURE_TAP));
    }
    if (isGestureSupported(Gesture::GESTURE_SWIPE))
    {
        anySupported = true;
        registerGesture(Gesture::GESTURE_SWIPE);
        GP_ASSERT(isGestureRegistered(Gesture::GESTURE_SWIPE));
    }
    if (isGestureSupported(Gesture::GESTURE_PINCH))
    {
        anySupported = true;
        registerGesture(Gesture::GESTURE_PINCH);
        GP_ASSERT(isGestureRegistered(Gesture::GESTURE_PINCH));
    }
	if (isGestureSupported(Gesture::GESTURE_LONG_TAP))
	{
        anySupported = true;
        registerGesture(Gesture::GESTURE_LONG_TAP);
        GP_ASSERT(isGestureRegistered(Gesture::GESTURE_LONG_TAP));
	}
	if (isGestureSupported(Gesture::GESTURE_DRAG))
	{
        anySupported = true;
        registerGesture(Gesture::GESTURE_DRAG);
        GP_ASSERT(isGestureRegistered(Gesture::GESTURE_DRAG));
	}
	if (isGestureSupported(Gesture::GESTURE_DROP))
	{
        anySupported = true;
        registerGesture(Gesture::GESTURE_DROP);
        GP_ASSERT(isGestureRegistered(Gesture::GESTURE_DROP));
	}
    //GP_ASSERT(anySupported == isGestureSupported(Gesture::GESTURE_ANY_SUPPORTED));
}

void GestureSample::finalize()
{
    SAFE_RELEASE(_font);

    unregisterGesture(Gesture::GESTURE_TAP);
    unregisterGesture(Gesture::GESTURE_SWIPE);
    unregisterGesture(Gesture::GESTURE_PINCH);
	unregisterGesture(Gesture::GESTURE_LONG_TAP);
	unregisterGesture(Gesture::GESTURE_DRAG);
	unregisterGesture(Gesture::GESTURE_DROP);
}

void GestureSample::update(float elapsedTime)
{
}

void GestureSample::render(float elapsedTime)
{
    // Clear the color and depth buffers
    clear(CLEAR_COLOR_DEPTH, Vector4::zero(), 1.0f, 0);

    // Draw text
    Vector4 fontColor(1.0f, 1.0f, 1.0f, 1.0f);
    float fontSize = 18;
    
    _font->start();
    float y = 0;
    size_t count = 0;
    for (std::list<std::wstring>::const_iterator it = _eventLog.begin(); it != _eventLog.end(); ++it)
    {
        ++count;
        _font->drawText(it->c_str(), 0, y, fontColor, fontSize);
        y += fontSize;

        if (y > (int)getHeight())
        {
            _eventLog.resize(count);
            break;
        }
    }
    
    float x = getWidth() - 200;
    y = getHeight() - fontSize * 6;

    if (isGestureSupported(Gesture::GESTURE_TAP))
    {
        _font->drawText(L"Tap supported", x, y, fontColor, fontSize);
        y += fontSize;
    }
    if (isGestureSupported(Gesture::GESTURE_SWIPE))
    {
        _font->drawText(L"Swipe supported", x, y, fontColor, fontSize);
        y += fontSize;
    }
    if (isGestureSupported(Gesture::GESTURE_PINCH))
    {
        _font->drawText(L"Pinch supported", x, y, fontColor, fontSize);
        y += fontSize;
    }
    if (isGestureSupported(Gesture::GESTURE_LONG_TAP))
    {
        _font->drawText(L"Long tap supported", x, y, fontColor, fontSize);
        y += fontSize;
    }
    if (isGestureSupported(Gesture::GESTURE_DRAG))
    {
        _font->drawText(L"Drag supported", x, y, fontColor, fontSize);
        y += fontSize;
    }
    if (isGestureSupported(Gesture::GESTURE_DROP))
    {
        _font->drawText(L"Drop supported", x, y, fontColor, fontSize);
        y += fontSize;
    }

    _font->finish();
}

void GestureSample::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    switch (evt)
    {
    case Touch::TOUCH_PRESS:
        if (x < 30 && y < 30)
        {
            displayKeyboard(true);
        }
        break;
    case Touch::TOUCH_RELEASE:
        break;
    case Touch::TOUCH_MOVE:
        break;
    };
}

void GestureSample::gestureSwipeEvent(int x, int y, int direction)
{
    std::wstringstream convert;
    convert << "Swipe (";

    if (direction & Gesture::SWIPE_DIRECTION_UP)
    {
        convert << "U";
    }
    if (direction & Gesture::SWIPE_DIRECTION_DOWN)
    {
        convert << "D";
    }
    if (direction & Gesture::SWIPE_DIRECTION_LEFT)
    {
        convert << "L";
    }
    if (direction & Gesture::SWIPE_DIRECTION_RIGHT)
    {
        convert << "R";
    }
    convert << ") " << x << ", " << y;
    _eventLog.push_front(convert.str());
}
   
void GestureSample::gesturePinchEvent(int x, int y, float scale, int numberOfTouches)
{
    std::wstringstream convert;
    convert << "Pinch " << x << ", " << y << " scale(" << scale << ")";
    _eventLog.push_front(convert.str());
}

void GestureSample::gestureTapEvent(int x, int y)
{
    std::wstringstream convert;
    convert << "Tap " << x << ", " << y;
    _eventLog.push_front(convert.str());
}

void GestureSample::gestureLongTapEvent(int x, int y, float duration)
{
    std::wstringstream convert;
    convert << "Long tap " << x << ", " << y << " (" << duration << "ms)";
    _eventLog.push_front(convert.str());
}

void GestureSample::gestureDragEvent(int x, int y)
{
    std::wstringstream convert;
    convert << "Drag " << x << ", " << y;
    _eventLog.push_front(convert.str());
}

void GestureSample::gestureDropEvent(int x, int y)
{
    std::wstringstream convert;
    convert << "Drop " << x << ", " << y;
    _eventLog.push_front(convert.str());
}

