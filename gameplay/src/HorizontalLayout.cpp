#include "Base.h"
#include "HorizontalLayout.h"

namespace gameplay
{

HorizontalLayout::HorizontalLayout() : _rightToLeft(false)
{
}

HorizontalLayout::~HorizontalLayout()
{
}

HorizontalLayout* HorizontalLayout::create()
{
    return new HorizontalLayout();
}

void HorizontalLayout::setRightToLeft(bool rightToLeft)
{
    _rightToLeft = rightToLeft;
}

bool HorizontalLayout::getRightToLeft()
{
    return _rightToLeft;
}

Layout::Type HorizontalLayout::getType()
{
    return Layout::LAYOUT_HORIZONTAL;
}

void HorizontalLayout::update(const Container* container, const Vector2& offset)
{
    GP_ASSERT(container);

    // Need border, padding.
    Theme::Border border = container->getBorder(container->getState());
    Theme::Padding padding = container->getPadding();

    float xPosition = 0;

    const std::vector<Control*>& controls = container->getControls();

    int i, end, iter;
    if (_rightToLeft)
    {
        i = (int)controls.size() - 1;
        end = -1;
        iter = -1;
    }
    else
    {
        i = 0;
        end = (int)controls.size();
        iter = 1;
    }

    while (i != end)
    {
        Control* control = controls.at(i);
        GP_ASSERT(control);

        align(control, container);

        const Rectangle& bounds = control->getBounds();
        const Theme::Margin& margin = control->getMargin();

        xPosition += margin.left;

        control->setPosition(xPosition, margin.top);
        control->update(container, offset);

        xPosition += bounds.width + margin.right;

        i += iter;
    }
}

}