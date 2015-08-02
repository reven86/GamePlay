#ifndef PROGRESSBAR_H_
#define PROGRESSBAR_H_

#include "Base.h"
#include "Theme.h"
#include "Control.h"
#include "Properties.h"

namespace gameplay
{

/**
 * The following properties are available for progress bar:

 @verbatim
    progressbar
    {
        style       = <styleID>                 // A Style from the Theme.
        position    = <x, y>                    // Position of the Control on-screen, measured in pixels.
        size        = <width, height>           // Size of the Control, measured in pixels.
        value       = <float>                   // The default progress position of the marker from 0 to 1.
        consumeEvents = <bool>                  // Whether the progress bar propagates input events to the Game's input event handler. Default is true.
        orientation = <HORIZONTAL or VERTICAL>  // Determines whether a progress bar is stretched along its width or its height
    }
 @endverbatim
 */
class ProgressBar : public Control
{
    friend class Container;
    friend class ControlFactory;

public:

    /**
     * Orientation types.
     */
    enum OrientationType
    {
        /**
         * Horizontal orientation.
         */
        ORIENTATION_HORIZONTAL,

        /**
         * Vertical orientation.
         */
        ORIENTATION_VERTICAL,
    };

    /**
     * Create a new progress bar control.
     *
     * @param id The control's ID.
     * @param style The control's style.
     *
     * @return The new progress bar.
     * @script{create}
     */
    static ProgressBar* create(const char* id, Theme::Style* style);

    /**
     * Set this progress bar's value.  The new value will be clamped in the range 0..1
     *
     * @param value The new value.
     */
    void setValue(float value);

    /**
     * Get this progress bar's current value.
     *
     * @return This progress bar's current value.
     */
    float getValue() const;

    /**
     * @see ScriptTarget::getTypeName
     */
    const char* getTypeName() const;

    /**
     * Get progress bar's orientation.
     *
     * @return Progress bar's orientation.
     */
    const OrientationType& getOrientation( ) const;

    /**
     * Set this progress bar's orientation.
     *
     * @param orientation The new orientation.
     */
    void setOrientation( const OrientationType& orientation );

protected:

    /**
     * Constructor.
     */
    ProgressBar();

    /**
     * Destructor.
     */
    virtual ~ProgressBar();

    /**
     * Create a progress bar with a given style and properties.
     *
     * @param style The style to apply to this slider.
     * @param properties The properties to set on this slider.
     *
     * @return The new progress bar.
     */
    static Control* create(Theme::Style* style, Properties* properties = NULL);

    void initialize(const char* typeName, Theme::Style* style, Properties* properties);

    /**
     * @see Control::drawImages
     */
    unsigned int drawImages(Form* form) const;

    /**
     * @see Control::updateState
     */
    void updateState(State state);

    /**
     * The progress bar's current value.
     */
    float _value;

    /**
     * The progress bar's orientation.
     */
    OrientationType _orientation;

    /**
     * The image for the progress bar track image.
     */
    const Theme::ThemeImage* _trackImage;
    
private:

    /**
     * Constructor.
     */
    ProgressBar(const ProgressBar& copy);
};

}

#endif
