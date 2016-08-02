/*
    Copyright 2016 Andrew Karpushin (reven86@gmail.com).

    This source file is a Derivative Work from orignal Gameplay3D source files.
    The Gameplay3D project is distributed under the terms of Apache 2.0 License.
    Original Gameplay3D source files can be found at https://github.com/gameplay3d/GamePlay

    Changes to orginal document were done in lines: 72, 141.
*/

#ifndef CHECKBOX_H_
#define CHECKBOX_H_

#include "Theme.h"
#include "Properties.h"
#include "Touch.h"
#include "Button.h"

namespace gameplay
{

/**
 * Defines a checkbox control.  
 *
 * This is a button that can be enabled or disabled.
 *
 * @see http://gameplay3d.github.io/GamePlay/docs/file-formats.html#wiki-UI_Forms
 */
class CheckBox : public Button
{
    friend class Container;
    friend class ControlFactory;

public:

    /**
     * Creates a new CheckBox.
     *
     * @param id The checkbox ID.
     * @param style The checkbox style (optional).
     *
     * @return The new checkbox.
     * @script{create}
     */
    static CheckBox* create(const char* id, Theme::Style* style = NULL);

    /**
     * Extends ScriptTarget::getTypeName() to return the type name of this class.
     *
     * Child controls should override this function to return the correct type name.
     *
     * @return The type name of this class: "CheckBox"
     * @see ScriptTarget::getTypeName()
     */
    const char* getTypeName() const;

    /**
     * Gets whether this checkbox is checked.
     *
     * @return Whether this checkbox is checked.
     */
    bool isChecked();

    /**
     * Sets whether the checkbox is checked.
     *
     * @param checked TRUE if the checkbox is checked; FALSE if the checkbox is not checked.
     */
    void setChecked(bool checked);

    /**
     * Add a listener to be notified of specific events affecting
     * this control.  Event types can be OR'ed together.
     * E.g. To listen to touch-press and touch-release events,
     * pass <code>Control::Listener::TOUCH | Control::Listener::RELEASE</code>
     * as the second parameter.
     *
     * @param listener The listener to add.
     * @param eventFlags The events to listen for.
     */
    virtual void addListener(Control::Listener* listener, int eventFlags);

    /**
     * Get scale factor of icon.
     */
    float getIconScale() const { return _iconScale; };

    /**
     * Set scale factor for icon.
     *
     * @param scale Scale factor for icon.
     */
    void setIconScale(float scale);

protected:

    /**
     * Constructor.
     */
    CheckBox();

    /**
     * Destructor.
     */
    ~CheckBox();

    /**
     * Create a checkbox with a given style and properties.
     *
     * @param style The style to apply to this checkbox.
     * @param properties A properties object containing a definition of the checkbox (optional).
     *
     * @return The new checkbox.
     */
    static Control* create(Theme::Style* style, Properties* properties = NULL);

    /**
    * @see Control::initialize
    */
    void initialize(const char* typeName, Theme::Style* style, Properties* properties);

    /**
     * Keyboard callback on key events.
     *
     * @see Keyboard::KeyEvent
     * @see Keyboard::Key
     */
    bool keyEvent(Keyboard::KeyEvent evt, int key);

    /**
     * @see Control#controlEvent
     */
    void controlEvent(Control::Listener::EventType evt);

    /**
     * @see Control::updateState
     */
    void updateState(State state);

    /**
     * @see Control::updateBounds
     */
    void updateBounds();

    /**
     * @see Control::updateAbsoluteBounds
     */
    void updateAbsoluteBounds(const Vector2& offset);

    /**
     * @see Control::drawImages
     */
    unsigned int drawImages(Form* form) const;

    /**
     * Whether this checkbox is currently checked.
     */
    bool _checked;

    /**
     * The Theme::ThemeImage to display for the checkbox.
     */
    const Theme::ThemeImage* _image;

    /**
     * The space between icon and text. Default is 5.
     */
    float _textSpace;

    /**
     * The scale of icon. 1 means icon height of unchecked image is 
     * equal to text height.
     */
    float _iconScale;

private:

    /*
     * Constructor.
     */
    CheckBox(const CheckBox& copy);
};

}

#endif
