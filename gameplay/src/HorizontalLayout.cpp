#include "Base.h"
#include "HorizontalLayout.h"

namespace gameplay
{

HorizontalLayout::HorizontalLayout() : _rightToLeft(false), _spacing(0)
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

int HorizontalLayout::getSpacing() const
{
    return _spacing;
}

void HorizontalLayout::setSpacing(int spacing)
{
    _spacing = spacing;
}

void HorizontalLayout::update(const Container* container)
{
    GP_ASSERT(container);

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

        if (control->isVisible())
        {
            const Rectangle& bounds = control->getBounds();
            const Theme::Margin& margin = control->getMargin();

            xPosition += margin.left;

            control->setPosition(xPosition, control->getY());

            xPosition += bounds.width + margin.right + _spacing;
        }

        i += iter;
    }
}

}