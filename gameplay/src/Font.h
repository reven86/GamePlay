/*
    Copyright 2016 Andrew Karpushin (reven86@gmail.com).

    This source file is a Derivative Work from orignal Gameplay3D source files.
    The Gameplay3D project is distributed under the terms of Apache 2.0 License.
    Original Gameplay3D source files can be found at https://github.com/gameplay3d/GamePlay

    Changes to orginal document were done in lines: 28-350.
*/

#ifndef FONT_H_
#define FONT_H_

#include "SpriteBatch.h"

namespace gameplay
{

/**
 * Defines a font for text rendering.
 */
class Font : public Ref
{
    friend class Bundle;
    friend class Text;
    friend class TextBox;

public:

    /**
     * Defines the set of allowable font styles.
     */
    enum Style
    {
        PLAIN = 0,
        BOLD = 1,
        ITALIC = 2,
        BOLD_ITALIC = 4,
        TEXTURED = 5,
    };

    /**
     * Defines the set of allowable alignments when drawing text.
     */
    enum Justify
    {
        ALIGN_LEFT = 0x01,
        ALIGN_HCENTER = 0x02,
        ALIGN_RIGHT = 0x04,
        ALIGN_TOP = 0x10,
        ALIGN_VCENTER = 0x20,
        ALIGN_BOTTOM = 0x40,
        ALIGN_TOP_LEFT = ALIGN_TOP | ALIGN_LEFT,
        ALIGN_VCENTER_LEFT = ALIGN_VCENTER | ALIGN_LEFT,
        ALIGN_BOTTOM_LEFT = ALIGN_BOTTOM | ALIGN_LEFT,
        ALIGN_TOP_HCENTER = ALIGN_TOP | ALIGN_HCENTER,
        ALIGN_VCENTER_HCENTER = ALIGN_VCENTER | ALIGN_HCENTER,
        ALIGN_BOTTOM_HCENTER = ALIGN_BOTTOM | ALIGN_HCENTER,
        ALIGN_TOP_RIGHT = ALIGN_TOP | ALIGN_RIGHT,
        ALIGN_VCENTER_RIGHT = ALIGN_VCENTER | ALIGN_RIGHT,
        ALIGN_BOTTOM_RIGHT = ALIGN_BOTTOM | ALIGN_RIGHT
    };

    /**
     * Defines flags to render text.
     */
    enum DrawFlags
    {
        LEFT_TO_RIGHT = 0x00,       //@< Default drawing behavior, text is rendered left to right
        RIGHT_TO_LEFT = 0x01,       //@< Whether to draw text from right to left.
        DRAW_VERTICAL_CCW = 0x02,   //@< Text is rotated 90 degrees counter clockwise.
        DRAW_VERTICAL_CW = 0x04     //@< Text is rotated 90 degrees clockwise.
    };

    /**
     * Defines the format of the font.
     */
    enum Format
    {
        BITMAP = 0,
        DISTANCE_FIELD = 1
    };

    /**
     * Creates a font from the given bundle.
     *
     * If the 'id' parameter is NULL, it is assumed that the Bundle at 'path'
     * contains exactly one Font resource. If the Bundle does not meet this criteria,
     * NULL is returned.
     *
     * If a font for the given path has already been loaded, the existing font will be
     * returned with its reference count increased.
     *
     * @param path The path to a bundle file containing a font resource.
     * @param id An optional ID of the font resource within the bundle (NULL for the first/only resource).
     *
     * @return The specified Font or NULL if there was an error.
     * @script{create}
     */
    static Font* create(const char* path, const char* id = NULL);

    /**
     * Gets the font size (max height of glyphs) in pixels, at the specified index.
     *
     * The Font class can store multiple sizes of glyphs for a font. The number of font
     * sizes stored can be retrieved via getSizeCount.
     *
     * @param index Index of the size to returned (default is 0).
     * @see getSizeCount
     */
    unsigned int getSize(unsigned int index = 0) const;

    /**
     * Returns the number of font sizes supported by this Font.
     */
    unsigned int getSizeCount() const;

    /**
     * Gets the font format. BITMAP or DISTANCEMAP.
     */
    Format getFormat() const;

    /**
     * Determines if this font supports the specified character code.
     *
     * @param character The character code to check.
     * @return True if this Font supports (can draw) the specified character, false otherwise.
     */
    bool isCharacterSupported(int character) const;

    /**
     * Starts text drawing for this font.
     */
    void start() const;

    /**
     * Draws the specified text in a solid color, with a scaling factor.
     *
     * @param text The text to draw.
     * @param x The viewport x position to draw text at.
     * @param y The viewport y position to draw text at.
     * @param color The color of text.
     * @param size The size to draw text (0 for default size).
     * @param flags Drawing flags.
     * @param characterSpacing Additional spacing between characters, in pixels.
     * @param lineSpacing Additional spacing between lines, in pixels.
     * @script{ignore}
     */
    void drawText(const wchar_t* text, float x, float y, const Vector4& color, float size = 0, DrawFlags flags = LEFT_TO_RIGHT, float characterSpacing = 0.0f, float lineSpacing = 0.0f) const;

    /**
     * Draws the specified text in a solid color, with a scaling factor.
     *
     * @param text The text to draw.
     * @param x The viewport x position to draw text at.
     * @param y The viewport y position to draw text at.
     * @param red The red channel of the text color.
     * @param green The green channel of the text color.
     * @param blue The blue channel of the text color.
     * @param alpha The alpha channel of the text color.
     * @param size The size to draw text (0 for default size).
     * @param flags Drawing flags.
     * @param characterSpacing Additional spacing between characters, in pixels.
     * @param lineSpacing Additional spacing between lines, in pixels.
     * @script{ignore}
     */
    void drawText(const wchar_t* text, float x, float y, float red, float green, float blue, float alpha, float size = 0, 
        DrawFlags flags = LEFT_TO_RIGHT, float characterSpacing = 0.0f, float lineSpacing = 0.0f) const;

    /**
     * Draws the specified text within a rectangular area, with a specified alignment and scale.
     * Clips text outside the viewport. Optionally wraps text to fit within the width of the viewport.
     *
     * @param text The text to draw.
     * @param area The viewport area to draw within.  Text will be clipped outside this rectangle.
     * @param color The color of text.
     * @param size The size to draw text (0 for default size).
     * @param justify Justification of text within the viewport.
     * @param wrap Wraps text to fit within the width of the viewport if true.
     * @param flags Drawing flags.
     * @param clip A region to clip text within after applying justification to the viewport area.
     * @param characterSpacing Additional spacing between characters, in pixels.
     * @param lineSpacing Additional spacing between lines, in pixels.
     * @script{ignore}
     */
    void drawText(const wchar_t* text, const Rectangle& area, const Vector4& color, float size = 0, 
        Justify justify = ALIGN_TOP_LEFT, bool wrap = true, DrawFlags flags = LEFT_TO_RIGHT, const Rectangle& clip = Rectangle(0, 0, 0, 0),
        float characterSpacing = 0.0f, float lineSpacing = 0.0f) const;

    /**
     * Finishes text batching for this font and renders all drawn text.
     */
    void finish() const;

    /**
     * Measures a string's width and height without alignment, wrapping or clipping.
     *
     * @param text The text to measure.
     * @param size The font height to scale to.
     * @param flags Text's flags.
     * @param widthOut Destination for the text's width.
     * @param heightOut Destination for the text's height.
     * @param characterSpacing Additional spacing between characters, in pixels.
     * @param lineSpacing Additional spacing between lines, in pixels.
     * @script{ignore}
     */
    void measureText(const wchar_t* text, float size, DrawFlags flags, float* widthOut, float* heightOut,
        float characterSpacing = 0.0f, float lineSpacing = 0.0f) const;

    /**
     * Measures a string's bounding box after alignment, wrapping and clipping within a viewport.
     *
     * @param text The text to measure.
     * @param clip The clip rectangle.
     * @param size The font height to scale to.
     * @param flags Text's flags.
     * @param out Destination rectangle to store the bounds in.
     * @param justify Justification of text within the viewport.
     * @param wrap Whether to measure text with wrapping applied.
     * @param ignoreClip Whether to clip 'out' to the viewport.  Set false for the bounds of what would actually be drawn
     *                within the given viewport; true for bounds that are guaranteed to fit the entire string of text.
     * @param characterSpacing Additional spacing between characters, in pixels.
     * @param lineSpacing Additional spacing between lines, in pixels.
     * @script{ignore}
     */
    void measureText(const wchar_t* text, const Rectangle& clip, float size, DrawFlags flags, Rectangle* out,
                     Justify justify = ALIGN_TOP_LEFT, bool wrap = true, bool ignoreClip = false,
                     float characterSpacing = 0.0f, float lineSpacing = 0.0f) const;

    /**
     * Get an character index into a string corresponding to the character nearest the given location within the clip region.
     * @script{ignore}
     */
    int getIndexAtLocation(const wchar_t* text, const Rectangle& clip, float size, const Vector2& inLocation, Vector2* outLocation,
        Justify justify = ALIGN_TOP_LEFT, bool wrap = true, DrawFlags flags = LEFT_TO_RIGHT,
        float characterSpacing = 0.0f, float lineSpacing = 0.0f) const;

    /**
     * Get the location of the character at the given index.
     * @script{ignore}
     */
    void getLocationAtIndex(const wchar_t* text, const Rectangle& clip, float size, Vector2* outLocation, const unsigned int destIndex,
        Justify justify = ALIGN_TOP_LEFT, bool wrap = true, DrawFlags flags = LEFT_TO_RIGHT,
        float characterSpacing = 0.0f, float lineSpacing = 0.0f) const;

    /**
     * Gets the sprite batch used to draw this Font.
     *
     * @param size The font size to be drawn.
     *
     * @return The SpriteBatch that most closely matches the requested font size.
     */
    SpriteBatch* getSpriteBatch(unsigned int size) const;

    /**
     * Gets the Justify value from the given string.
     * Returns ALIGN_TOP_LEFT if the string is unrecognized.
     *
     * @param justify The string such as "ALIGN_HCENTER" or "ALIGN_VCENTER_RIGHT".
     *
     * @return The Justify value.
     */
    static Justify getJustify(const char* justify);

    /**
     * Gets the DrawFlags value from the given string.
     * Returns LEFT_TO_RIGHT if the string is unrecognized.
     *
     * @param flags The string such as "RIGHT_TO_LEFT" or "DRAW_VERTICAL_CW".
     *
     * @return The DrawFlags value.
     */
    static DrawFlags getDrawFlags(const char* flags);

public:

    /**
     * Defines a font glyph within the texture map for a font.
     */
    class Glyph
    {
    public:
        /**
         * Glyph character code (decimal value).
         */
        unsigned int code;

        /**
         * Glyph width (in pixels).
         */
        unsigned int width;

        /**
         * Glyph left side bearing (in pixels).
         */
        int bearingX;

        /**
         * Glyph horizontal advance (in pixels).
         */
        unsigned int advance;

        /**
         * Glyph texture coordinates.
         */
        float uvs[4];
    };

    /**
     * Gets glyph by character code or NULL.
     */
    const Glyph * getGlyphByCode( int characterCode ) const;

private:
    /**
     * Constructor.
     */
    Font();

    /**
     * Constructor.
     */
    Font(const Font& copy);

    /**
     * Destructor.
     */
    ~Font();

    /**
     * Hidden copy assignment operator.
     */
    Font& operator=(const Font&);

    /**
     * Creates a font with the given characteristics from the specified glyph array and texture map.
     *
     * This method will create a new Font object regardless of whether another Font is already
     * created with the same attributes.
     *
     * @param family The font family name.
     * @param style The font style.
     * @param size The font size.
     * @param glyphs An array of font glyphs, defining each character in the font within the texture map.
     * @param glyphCount The number of items in the glyph array.
     * @param texture A texture map containing rendered glyphs.
     * @param format The format of the font (bitmap or distance fields)
     *
     * @return The new Font or NULL if there was an error.
     */
    static Font* create(const char* family, Style style, unsigned int size, Glyph* glyphs, int glyphCount, Texture* texture, Font::Format format);

    void getMeasurementInfo(const wchar_t* text, const Rectangle& area, float size, Justify justify, bool wrap, DrawFlags flags,
                            std::vector<float>* xPositions, float* yPosition, std::vector<unsigned int>* lineLengths,
                            float characterSpacing, float lineSpacing) const;

    int getIndexOrLocation(const wchar_t* text, const Rectangle& clip, float size, const Vector2& inLocation, Vector2* outLocation,
        const int destIndex = -1, Justify justify = ALIGN_TOP_LEFT, bool wrap = true, DrawFlags flags = LEFT_TO_RIGHT,
        float characterSpacing = 0.0f, float lineSpacing = 0.0f) const;

    float getTokenWidth(const wchar_t* token, unsigned int length, float size, float scale, float characterSpacing) const;

    // get token length, space and newlines act as delimiters. for asian languages each char is considered as one token
    unsigned int getTokenLength(const wchar_t* tokens) const;
    unsigned int getReversedTokenLength(const wchar_t* token, const wchar_t* bufStart) const;

    int handleDelimiters(const wchar_t** token, const float size, float scale, const int iteration, const float areaX, float* xPos, float* yPos, unsigned int* lineLength,
                         std::vector<float>::const_iterator* xPositionsIt, std::vector<float>::const_iterator xPositionsEnd, unsigned int* charIndex = NULL,
                         const Vector2* stopAtPosition = NULL, const int currentIndex = -1, const int destIndex = -1, 
                         float characterSpacing = 0.0f, float lineSpacing = 0.0f) const;

    void addLineInfo(const Rectangle& area, float lineWidth, int lineLength, Justify hAlign,
        std::vector<float>* xPositions, std::vector<unsigned int>* lineLengths, DrawFlags flags) const;

    //! Returns glyph index by character code or -1.
    int getGlyphIndexByCode( int characterCode ) const;

    const Font* findClosestSize(int size) const;

    void lazyStart() const;

    Format _format;
    std::string _path;
    std::string _id;
    std::string _family;
    Style _style;
    unsigned int _size;
    std::vector<Font*> _sizes; // stores additional font sizes of the same family
    Glyph* _glyphs;
    unsigned int _glyphCount;
    Texture* _texture;
    SpriteBatch* _batch;
    Rectangle _viewport;
    mutable MaterialParameter* _cutoffParam;    // cached value, updated on draw.
};

}

#endif
