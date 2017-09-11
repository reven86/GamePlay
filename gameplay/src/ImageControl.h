#ifndef IMAGECONTROL_H_
#define IMAGECONTROL_H_

#include "Control.h"
#include "Theme.h"
#include "Image.h"
#include "SpriteBatch.h"
#include "Rectangle.h"

namespace gameplay
{

/**
 * Defines an image control.
 *
 * This allows forms to display seperate images from arbitrary files not specified in the theme.
 *
 * @see http://gameplay3d.github.io/GamePlay/docs/file-formats.html#wiki-UI_Forms
 */
class ImageControl : public Control
{
    friend class Container;
    friend class ControlFactory;

public:

    /**
     * Creates a new ImageControl.
     *
     * @param id The image control ID.
     * @param style The image control style (optional).
     *
     * @return The new image control.
     * @script{create}
     */
    static ImageControl* create(const char* id, Theme::Style* style = NULL);

    /**
     * Extends ScriptTarget::getTypeName() to return the type name of this class.
     *
     * Child controls should override this function to return the correct type name.
     *
     * @return The type name of this class: "ImageControl"
     * @see ScriptTarget::getTypeName()
     */
    const char* getTypeName() const;

    /**
     * Set the path of the image for this ImageControl to display.
     *
     * @param path The path to the image.
     */
    virtual void setImage(const char* path);

    /**
     * Set image for this ImageControl to display.
     *
     * @param image The image to display.
     */
    virtual void setImage(Image * image);

    /**
     * Set the source region of this ImageControl.  This is the region of the file,
     * in pixels, to use when drawing.
     *
     * @param x The x coordinate of the source region.
     * @param y The y coordinate of the source region.
     * @param width The width of the source region.
     * @param height The height of the source region.
     */
    void setRegionSrc(float x, float y, float width, float height);

    /**
     * Set the source region of this ImageControl.  This is the region of the file,
     * in pixels, to use when drawing.
     *
     * @param region The new source region.
     */
    void setRegionSrc(const Rectangle& region);

    /**
     * Get the source region of this ImageControl.
     *
     * @return The source region of this ImageControl.
     */
    const Rectangle& getRegionSrc() const;

    /**
     * Sets the destination region of this ImageControl.  This is the region
     * within the control's viewport to draw the image.
     *
     * @param x The x coordinate of the destination region.
     * @param y The y coordinate of the destination region.
     * @param width The width of the destination region.
     * @param height The height of the destination region.
     */
    void setRegionDst(float x, float y, float width, float height);

    /**
     * Sets the destination region of this ImageControl.  This is the region
     * within the control's viewport to draw the image.
     *
     * @param region The new destination region.
     */
    void setRegionDst(const Rectangle& region);

    /**
     * Get the destination region of this ImageControl.
     *
     * @return The destination region of this ImageControl.
     */
    const Rectangle& getRegionDst() const;

    /**
     * Get the SpriteBatch for this ImageControl.
     *
     * @return SpriteBatch instance.
     */
    const SpriteBatch * getSpriteBatch() const;

    /**
     * Get image color.
     *
     * @return Color which is used when image's sprite batch is rendered,
     *         before applying Control's opacity.
     */
    const Vector4& getColor() const;

    /**
     * Set image color.
     *
     * @param color Color which is used when image's sprite batch is rendered,
     *              before applying Control's opacity.
     */
    void setColor(const Vector4& color);

protected:

    ImageControl();

    virtual ~ImageControl();

    /**
     * Creates a new ImageControl.
     * 
     * @param style The control's custom style.
     * @param properties A properties object containing a definition of the ImageControl (optional).
     *
     * @return The new ImageControl.
     * @script{create}
      * 
      */
    static Control* create(Theme::Style* style, Properties* properties = NULL);

    void initialize(const char* typeName, Theme::Style* style, Properties* properties);

    /**
     * @see Control::drawImages
     */
    unsigned int drawImages(Form* form) const;

    /**
     * @see Control::updateBounds
     */
    void updateBounds();

private:

    ImageControl(const ImageControl& copy);

protected:

    // Source region.
    Rectangle _srcRegion;
    // Destination region.
    Rectangle _dstRegion;
    SpriteBatch* _batch;
    Vector4 _color;

    // One over texture width and height, for use when calculating UVs from a new source region.
    float _tw;
    float _th;
    
    // Calculated UVs.
    Theme::UVs _uvs;
};

}

#endif