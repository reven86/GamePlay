#ifndef HORIZONTALLAYOUT_H_
#define HORIZONTALLAYOUT_H_

#include "Layout.h"
#include "Container.h"

namespace gameplay
{

/**
 * Defines a horizontal layout.
 *
 * Controls are placed next to one another horizontally until
 * the right-most edge of the container is reached.
 */
class HorizontalLayout : public Layout
{
    friend class Form;
    friend class Container;

public:

    /**
     * Set whether this layout will start laying out controls from the right of the container.
     * This setting defaults to 'false', meaning controls will start at the left.
     *
     * @param rightToLeft Whether to start laying out controls from the right of the container.
     */
    void setRightToLeft(bool rightToLeft);

    /**
     * Get whether this layout will start laying out controls from the right of the container.
     *
     * @return Whether to start laying out controls from the right of the container.
     */
    bool getRightToLeft();

    /**
     * Get the type of this Layout.
     *
     * @return Layout::LAYOUT_HORIZONTAL
     */
    Layout::Type getType();

    /**
     * Returns the horizontal spacing between controls in the layout.
     *
     * @return The horizontal spacing between controls.
     */
    int getSpacing() const;

    /**
     * Sets the horizontal spacing to add between controls in the layout.
     *
     * @param spacing The horizontal spacing between controls.
     */
    void setSpacing(int spacing);

protected:

    /**
     * Constructor.
     */
    HorizontalLayout();

    /**
     * Destructor.
     */
    virtual ~HorizontalLayout();

    /**
     * Update the controls contained by the specified container.
     *
     * Controls are placed next to one another horizontally until
     * the right-most edge of the container is reached.
     *
     * @param container The container to update.
     */
    void update(const Container* container);

    /**
     * Flag determining whether this layout will start laying out controls from the right of the container.
     * The default is 'false' meaning controls will start at the left.
     */
    bool _rightToLeft;

    /**
     * Spacing between controls in the layout.
     */
    int _spacing;

private:

    /**
     * Constructor.
     */
    HorizontalLayout(const HorizontalLayout& copy);

    /**
     * Create a HorizontalLayout.
     *
     * @return a HorizontalLayout object.
     */
    static HorizontalLayout* create();

};

}

#endif