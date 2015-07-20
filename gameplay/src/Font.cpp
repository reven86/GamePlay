#include "Base.h"
#include "Font.h"
#include "Text.h"
#include "Game.h"
#include "FileSystem.h"
#include "Bundle.h"
#include "Material.h"

// Default font shaders
#define FONT_VSH "res/shaders/font.vert"
#define FONT_FSH "res/shaders/font.frag"
#define FONT_FSH_ALPHA "res/shaders/font_alpha.frag"

namespace gameplay
{

static std::vector<Font*> __fontCache;

static Effect* __fontEffect = NULL;
static Effect* __fontEffectAlpha = NULL;

Font::Font() :
    _format(BITMAP), _style(PLAIN), _size(0), _spacing(0.0f), _glyphs(NULL), _glyphCount(0), _texture(NULL), _batch(NULL), _cutoffParam(NULL)
{
}

Font::~Font()
{
    // Remove this Font from the font cache.
    std::vector<Font*>::iterator itr = std::find(__fontCache.begin(), __fontCache.end(), this);
    if (itr != __fontCache.end())
    {
        __fontCache.erase(itr);
    }

    SAFE_DELETE(_batch);
    SAFE_DELETE_ARRAY(_glyphs);
    SAFE_RELEASE(_texture);

    // Free child fonts
    for (size_t i = 0, count = _sizes.size(); i < count; ++i)
    {
        SAFE_RELEASE(_sizes[i]);
    }
}

Font* Font::create(const char* path, const char* id)
{
    GP_ASSERT(path);

    // Search the font cache for a font with the given path and ID.
    for (size_t i = 0, count = __fontCache.size(); i < count; ++i)
    {
        Font* f = __fontCache[i];
        GP_ASSERT(f);
        if (f->_path == path && (id == NULL || f->_id == id))
        {
            // Found a match.
            f->addRef();
            return f;
        }
    }

    // Load the bundle.
    Bundle* bundle = Bundle::create(path);
    if (bundle == NULL)
    {
        GP_WARN("Failed to load font bundle '%s'.", path);
        return NULL;
    }

    Font* font = NULL;
    if (id == NULL)
    {
        // Get the ID of the first object in the bundle (assume it's a Font).
        const char* id;
        if ((id = bundle->getObjectId(0)) == NULL)
        {
            GP_WARN("Failed to load font without explicit id; the first object in the font bundle has a null id.");
            return NULL;
        }

        // Load the font using the ID of the first object in the bundle.
        font = bundle->loadFont(bundle->getObjectId(0));
    }
    else
    {
        // Load the font with the given ID.
        font = bundle->loadFont(id);
    }

    if (font)
    {
        // Add this font to the cache.
        __fontCache.push_back(font);
    }

    SAFE_RELEASE(bundle);

    return font;
}

Font* Font::create(const char* family, Style style, unsigned int size, Glyph* glyphs, int glyphCount, Texture* texture, Font::Format format)
{
    GP_ASSERT(family);
    GP_ASSERT(glyphs);
    GP_ASSERT(texture);

    // Create the effect for the font's sprite batch.
    Effect ** fontEffect = style == Font::TEXTURED ? &__fontEffect : &__fontEffectAlpha;
    if (*fontEffect == NULL)
    {
        const char* defines = NULL;
        if (format == DISTANCE_FIELD)
            defines = "DISTANCE_FIELD";
        *fontEffect = Effect::createFromFile(FONT_VSH, style == Font::TEXTURED ? FONT_FSH : FONT_FSH_ALPHA, defines);
        if (*fontEffect == NULL)
        {
            GP_WARN("Failed to create effect for font.");
            SAFE_RELEASE(texture);
            return NULL;
        }
    }
    else
    {
        ( *fontEffect )->addRef();
    }

    // Create batch for the font.
    SpriteBatch* batch = SpriteBatch::create(texture, *fontEffect, 128);

    // Release __fontEffect since the SpriteBatch keeps a reference to it
    SAFE_RELEASE(*fontEffect);

    if (batch == NULL)
    {
        GP_WARN("Failed to create batch for font.");
        return NULL;
    }

    // Add linear filtering for better font quality.
    Texture::Sampler* sampler = batch->getSampler();
    sampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);
    sampler->setWrapMode(Texture::CLAMP, Texture::CLAMP);

    // Increase the ref count of the texture to retain it.
    texture->addRef();

    Font* font = new Font();
    font->_format = format;
    font->_family = family;
    font->_style = style;
    font->_size = size;
    font->_texture = texture;
    font->_batch = batch;

    // Copy the glyphs array.
    font->_glyphs = new Glyph[glyphCount];
    memcpy(font->_glyphs, glyphs, sizeof(Glyph) * glyphCount);
    font->_glyphCount = glyphCount;

    return font;
}

unsigned int Font::getSize(unsigned int index) const
{
    GP_ASSERT(index <= _sizes.size());

    // index zero == this font
    return index == 0 ? _size : _sizes[index - 1]->_size;
}

unsigned int Font::getSizeCount() const
{
    return _sizes.size() + 1; // +1 for "this" font
}

Font::Format Font::getFormat() const
{
    return _format;
}

bool Font::isCharacterSupported(int character) const
{
    // TODO: Update this once we support unicode fonts
    int glyphIndex = character - 32; // HACK for ASCII
    return (glyphIndex >= 0 && glyphIndex < (int)_glyphCount);
}

void Font::start() const
{
    // no-op : fonts now are lazily started on the first draw call
}

void Font::lazyStart() const
{
    if (_batch->isStarted())
        return; // already started

    // Update the projection matrix for our batch to match the current viewport
    const Rectangle& vp = Game::getInstance()->getViewport();
    if (!vp.isEmpty())
    {
        Game* game = Game::getInstance();
        Matrix projectionMatrix;
        Matrix::createOrthographicOffCenter(0, vp.width, vp.height, 0, 0, 1, &projectionMatrix);
        _batch->setProjectionMatrix(projectionMatrix);
    }

    _batch->start();
}

void Font::finish() const
{
    // Finish any font batches that have been started
    if (_batch->isStarted())
        _batch->finish();

    for (size_t i = 0, count = _sizes.size(); i < count; ++i)
    {
        SpriteBatch* batch = _sizes[i]->_batch;
        if (batch->isStarted())
            batch->finish();
    }
}

const Font* Font::findClosestSize(int size) const
{
    if (size == (int)_size)
        return this;

    int diff = abs(size - (int)_size);
    const Font* closest = this;
    for (size_t i = 0, count = _sizes.size(); i < count; ++i)
    {
        Font* f = _sizes[i];
        int d = abs(size - (int)f->_size);
        if (d < diff || (d == diff && f->_size > closest->_size)) // prefer scaling down instead of up
        {
            diff = d;
            closest = f;
        }
    }

    return closest;
}

void Font::drawText(const wchar_t* text, float x, float y, const Vector4& color, float size, bool rightToLeft) const
{
    GP_ASSERT(_size);
    GP_ASSERT(text);

    if (size == 0)
    {
        size = _size;
    }
    else
    {
        // Delegate to closest sized font
        const Font* f = findClosestSize(size);
        if (f != this)
        {
            f->drawText(text, x, y, color, size, rightToLeft);
            return;
        }
    }

    lazyStart();

    float scale = (float)size / _size;
    float spacing = size * _spacing;
    const wchar_t* cursor = NULL;

    if (rightToLeft)
    {
        cursor = text;
    }

    float xPos = x, yPos = y;
    bool done = false;

    int spaceIndex = getGlyphIndexByCode(' ');
    float spaceAdvance = spaceIndex >= 0 && spaceIndex < (int)_glyphCount ? _glyphs[spaceIndex].advance * scale : size * 0.5f;

    while (!done)
    {
        size_t length;
        size_t startIndex;
        int iteration;
        if (rightToLeft)
        {
            wchar_t delimiter = cursor[0];
            while (!done &&
                   (delimiter == L' ' ||
                   delimiter == L'\t' ||
                   delimiter == L'\r' ||
                   delimiter == L'\n' ||
                   delimiter == 0))
            {
                switch (delimiter)
                {
                case L' ':
                    xPos += spaceAdvance;
                    break;
                case L'\r':
                case L'\n':
                    yPos += size;
                    xPos = x;
                    break;
                case L'\t':
                    xPos += spaceAdvance * 4;
                    break;
                case 0:
                    done = true;
                    break;
                }

                if (!done)
                {
                    ++cursor;
                    delimiter = cursor[0];
                }
            }

            length = wcscspn (cursor, L"\r\n");
            startIndex = length - 1;
            iteration = -1;
        }
        else
        {
            length = wcslen(text);
            startIndex = 0;
            iteration = 1;
        }

        GP_ASSERT(_glyphs);
        GP_ASSERT(_batch);
        for (size_t i = startIndex; i < length; i += (size_t)iteration)
        {
            wchar_t c = 0;
            if (rightToLeft)
            {
                c = cursor[i];
            }
            else
            {
                c = text[i];
            }

            // Draw this character.
            switch (c)
            {
            case L' ':
                xPos += spaceAdvance;
                break;
            case L'\r':
            case L'\n':
                yPos += size;
                xPos = x;
                break;
            case L'\t':
                xPos += spaceAdvance * 4;
                break;
            default:
                int index = getGlyphIndexByCode( c );
                if (index >= 0 && index < (int)_glyphCount)
                {
                    Glyph& g = _glyphs[index];

                    if (getFormat() == DISTANCE_FIELD )
                    {
                        if (_cutoffParam == NULL)
                            _cutoffParam = _batch->getMaterial()->getParameter("u_cutoff");
                        // TODO: Fix me so that smaller font are much smoother
                        _cutoffParam->setVector2(Vector2(1.0, 1.0));
                    }
                    _batch->draw(xPos + g.bearingX * scale, yPos, g.width * scale, size, g.uvs[0], g.uvs[1], g.uvs[2], g.uvs[3], color);
                    xPos += g.advance * scale + spacing;
                    break;
                }
                break;
            }
        }

        if (rightToLeft)
        {
            cursor += length;
        }
        else
        {
            done = true;
        }
    }
}

void Font::drawText(const wchar_t* text, float x, float y, float red, float green, float blue, float alpha, float size, bool rightToLeft) const
{
    drawText(text, x, y, Vector4(red, green, blue, alpha), size, rightToLeft);
}

void Font::drawText(const wchar_t* text, const Rectangle& area, const Vector4& color, float size, Justify justify, bool wrap, bool rightToLeft, const Rectangle& clip) const
{
    GP_ASSERT(text);
    GP_ASSERT(_size);

    if (size == 0)
    {
        size = _size;
    }
    else
    {
        // Delegate to closest sized font
        const Font* f = findClosestSize(size);
        if (f != this)
        {
            f->drawText(text, area, color, size, justify, wrap, rightToLeft, clip);
            return;
        }
    }

    lazyStart();

    float scale = (float)size / _size;
    float spacing = size * _spacing;
    float yPos = area.y;
    const float areaHeight = area.height - size;
    std::vector<float> xPositions;
    std::vector<unsigned int> lineLengths;

    getMeasurementInfo(text, area, size, justify, wrap, rightToLeft, &xPositions, &yPos, &lineLengths);

    // Now we have the info we need in order to render.
    float xPos = area.x;
    std::vector<float>::const_iterator xPositionsIt = xPositions.begin();
    if (xPositionsIt != xPositions.end())
    {
        xPos = *xPositionsIt++;
    }

    const wchar_t* token = text;
    int iteration = 1;
    unsigned int lineLength;
    unsigned int currentLineLength = 0;
    const wchar_t* lineStart;
    std::vector<unsigned int>::const_iterator lineLengthsIt;
    if (rightToLeft)
    {
        lineStart = token;
        lineLengthsIt = lineLengths.begin();
        lineLength = *lineLengthsIt++;
        token += lineLength - 1;
        iteration = -1;
    }

    while (token[0] != 0)
    {
        // Handle delimiters until next token.
        if (!handleDelimiters(&token, size, scale, iteration, area.x, &xPos, &yPos, &currentLineLength, &xPositionsIt, xPositions.end()))
        {
            break;
        }

        bool truncated = false;
        unsigned int tokenLength;
        float tokenWidth;
        unsigned int startIndex;
        if (rightToLeft)
        {
            tokenLength = getReversedTokenLength(token, text);
            currentLineLength += tokenLength;
            token -= (tokenLength - 1);
            tokenWidth = getTokenWidth(token, tokenLength, size, scale);
            iteration = -1;
            startIndex = tokenLength - 1;
        }
        else
        {
            tokenLength = (unsigned int)wcscspn (token, L" \r\n\t");
            tokenWidth = getTokenWidth(token, tokenLength, size, scale);
            iteration = 1;
            startIndex = 0;
        }

        // Wrap if necessary.
        if (wrap && (floorf(xPos + tokenWidth) > area.x + area.width || (rightToLeft && currentLineLength > lineLength)))
        {
            currentLineLength = tokenLength;

            if (xPositionsIt != xPositions.end())
            {
                xPos = *xPositionsIt++;
                yPos += size;
            }
            else
            {
                xPos = area.x;
            }
        }

        bool draw = true;
        if (ceilf(yPos) < area.y - size)
        {
            // Skip drawing until line break or wrap.
            draw = false;
        }
        else if (floorf(yPos) > area.y + areaHeight)
        {
            // Truncate below area's vertical limit.
            break;
        }

        GP_ASSERT(_glyphs);
        GP_ASSERT(_batch);
        for (int i = startIndex; i < (int)tokenLength && i >= 0; i += iteration)
        {
            wchar_t c = token[i];
            int glyphIndex = getGlyphIndexByCode( c );

            if (glyphIndex >= 0 && glyphIndex < (int)_glyphCount)
            {
                Glyph& g = _glyphs[glyphIndex];

                if (floorf(xPos + g.advance*scale) > area.x + area.width)
                {
                    // Truncate this line and go on to the next one.
                    truncated = true;
                    break;
                }
                else if (ceilf(xPos) >= area.x)
                {
                    // Draw this character.
                    if (draw)
                    {
                        if (getFormat() == DISTANCE_FIELD)
                        {
                            if (_cutoffParam == NULL)
                                _cutoffParam = _batch->getMaterial()->getParameter("u_cutoff");
                            // TODO: Fix me so that smaller font are much smoother
                            _cutoffParam->setVector2(Vector2(1.0, 1.0));
                        }
                        if (clip != Rectangle(0, 0, 0, 0))
                        {
                            _batch->draw(xPos + g.bearingX * scale, yPos, g.width * scale, size, g.uvs[0], g.uvs[1], g.uvs[2], g.uvs[3], color, clip);
                        }
                        else
                        {
                            _batch->draw(xPos + g.bearingX * scale, yPos, g.width * scale, size, g.uvs[0], g.uvs[1], g.uvs[2], g.uvs[3], color);
                        }
                    }
                }
                xPos += g.advance*scale + spacing;
            }
        }

        if (!truncated)
        {
            if (rightToLeft)
            {
                if (token == lineStart)
                {
                    token += lineLength;

                    // Now handle delimiters going forwards.
                    if (!handleDelimiters(&token, size, scale, 1, area.x, &xPos, &yPos, &currentLineLength, &xPositionsIt, xPositions.end()))
                    {
                        break;
                    }

                    if (lineLengthsIt != lineLengths.end())
                    {
                        lineLength = *lineLengthsIt++;
                    }
                    lineStart = token;
                    token += lineLength-1;
                }
                else
                {
                    token--;
                }
            }
            else
            {
                token += tokenLength;
            }
        }
        else
        {
            if (rightToLeft)
            {
                token = lineStart + lineLength;

                if (!handleDelimiters(&token, size, scale, 1, area.x, &xPos, &yPos, &currentLineLength, &xPositionsIt, xPositions.end()))
                {
                    break;
                }

                if (lineLengthsIt != lineLengths.end())
                {
                    lineLength = *lineLengthsIt++;
                }
                lineStart = token;
                token += lineLength-1;
            }
            else
            {
                // Skip the rest of this line.
                size_t tokenLength = wcscspn (token, L"\n");

                if (tokenLength > 0)
                {
                    // Get first token of next line.
                    token += tokenLength;
                }
            }
        }
    }
}

void Font::measureText(const wchar_t* text, float size, float* width, float* height) const
{
    GP_ASSERT(_size);
    GP_ASSERT(text);
    GP_ASSERT(width);
    GP_ASSERT(height);

    if (size == 0)
    {
        size = _size;
    }
    else
    {
        // Delegate to closest sized font
        const Font* f = findClosestSize(size);
        if (f != this)
        {
            f->measureText(text, size, width, height);
            return;
        }
    }

    const size_t length = wcslen(text);
    if (length == 0)
    {
        *width = 0;
        *height = 0;
        return;
    }

    float scale = (float)size / _size;
    const wchar_t* token = text;

    *width = 0;
    *height = size;

    // Measure a line at a time.
    while (token[0] != 0)
    {
        while (token[0] == L'\n')
        {
            *height += size;
            ++token;
        }

        unsigned int tokenLength = (unsigned int)wcscspn(token, L"\n");
        float tokenWidth = getTokenWidth(token, tokenLength, size, scale);
        if (tokenWidth > *width)
        {
            *width = tokenWidth;
        }

        token += tokenLength;
    }
}

void Font::measureText(const wchar_t* text, const Rectangle& clip, float size, Rectangle* out, Justify justify, bool wrap, bool ignoreClip) const
{
    GP_ASSERT(_size);
    GP_ASSERT(text);
    GP_ASSERT(out);

    if (size == 0)
    {
        size = _size;
    }
    else
    {
        // Delegate to closest sized font
        const Font* f = findClosestSize(size);
        if (f != this)
        {
            f->measureText(text, clip, size, out, justify, wrap, ignoreClip);
            return;
        }
    }

    if (wcslen(text) == 0)
    {
        out->set(0, 0, 0, 0);
        return;
    }

    float scale = (float)size / _size;
    Justify vAlign = static_cast<Justify>(justify & 0xF0);
    if (vAlign == 0)
    {
        vAlign = ALIGN_TOP;
    }

    Justify hAlign = static_cast<Justify>(justify & 0x0F);
    if (hAlign == 0)
    {
        hAlign = ALIGN_LEFT;
    }

    const wchar_t* token = text;
    std::vector<bool> emptyLines;
    std::vector<Vector2> lines;

    float lineWidth = 0;
    float yPos = clip.y + size;
    const float viewportHeight = clip.height;

    int spaceIndex = getGlyphIndexByCode(' ');
    float spaceAdvance = spaceIndex >= 0 && spaceIndex < (int)_glyphCount ? _glyphs[spaceIndex].advance * scale : size * 0.5f;

    if (wrap)
    {
        float delimWidth = 0;
        bool reachedEOF = false;
        while (token[0] != 0)
        {
            // Handle delimiters until next token.
            wchar_t delimiter = token[0];
            while (delimiter == L' ' ||
                    delimiter == L'\t' ||
                    delimiter == L'\r' ||
                    delimiter == L'\n' ||
                    delimiter == 0)
            {
                switch (delimiter)
                {
                    case L' ':
                        delimWidth += spaceAdvance;
                        break;
                    case L'\r':
                    case L'\n':
                        // Add line-height to vertical cursor.
                        yPos += size;

                        if (lineWidth > 0)
                        {
                            // Determine horizontal position and width.
                            float hWhitespace = clip.width - lineWidth;
                            float xPos = clip.x;
                            if (hAlign == ALIGN_HCENTER)
                            {
                                xPos += hWhitespace * 0.5f;
                            }
                            else if (hAlign == ALIGN_RIGHT)
                            {
                                xPos += hWhitespace;
                            }

                            // Record this line's size.
                            emptyLines.push_back(false);
                            lines.push_back(Vector2(xPos, lineWidth));
                        }
                        else
                        {
                            // Record the existence of an empty line.
                            emptyLines.push_back(true);
                            lines.push_back(Vector2(FLT_MAX, 0));
                        }

                        lineWidth = 0;
                        delimWidth = 0;
                        break;
                    case L'\t':
                        delimWidth += spaceAdvance * 4;
                        break;
                    case 0:
                        reachedEOF = true;
                        break;
                }

                if (reachedEOF)
                {
                    break;
                }

                token++;
                delimiter = token[0];
            }

            if (reachedEOF)
            {
                break;
            }

            // Measure the next token.
            unsigned int tokenLength = (unsigned int)wcscspn(token, L" \r\n\t");
            float tokenWidth = getTokenWidth(token, tokenLength, size, scale);

            // Wrap if necessary.
            if (int(lineWidth + tokenWidth + delimWidth) > int(clip.width))
            {
                // Add line-height to vertical cursor.
                yPos += size;

                // Determine horizontal position and width.
                float hWhitespace = clip.width - lineWidth;
                float xPos = clip.x;
                if (hAlign == ALIGN_HCENTER)
                {
                    xPos += hWhitespace * 0.5f;
                }
                else if (hAlign == ALIGN_RIGHT)
                {
                    xPos += hWhitespace;
                }

                // Record this line's size.
                emptyLines.push_back(false);
                lines.push_back(Vector2(xPos, lineWidth));
                lineWidth = 0;
            }
            else
            {
                lineWidth += delimWidth;
            }

            delimWidth = 0;
            lineWidth += tokenWidth;
            token += tokenLength;
        }
    }
    else
    {
        // Measure a whole line at a time.
        int emptyLinesCount = 0;
        while (token[0] != 0)
        {
            // Handle any number of consecutive newlines.
            bool nextLine = true;
            while (token[0] == L'\n')
            {
                if (nextLine)
                {
                    // Add line-height to vertical cursor.
                    yPos += size * (emptyLinesCount+1);
                    nextLine = false;
                    emptyLinesCount = 0;
                    emptyLines.push_back(false);
                }
                else
                {
                    // Record the existence of an empty line.
                    ++emptyLinesCount;
                    emptyLines.push_back(true);
                    lines.push_back(Vector2(FLT_MAX, 0));
                }

                token++;
            }

            // Measure the next line.
            unsigned int tokenLength = (unsigned int)wcscspn(token, L"\n");
            lineWidth = getTokenWidth(token, tokenLength, size, scale);

            // Determine horizontal position and width.
            float xPos = clip.x;
            float hWhitespace = clip.width - lineWidth;
            if (hAlign == ALIGN_HCENTER)
            {
                xPos += hWhitespace * 0.5f;
            }
            else if (hAlign == ALIGN_RIGHT)
            {
                xPos += hWhitespace;
            }

            // Record this line's size.
            lines.push_back(Vector2(xPos, lineWidth));

            token += tokenLength;
        }

        yPos += size;
    }

    if (wrap)
    {
        // Record the size of the last line.
        float hWhitespace = clip.width - lineWidth;
        float xPos = clip.x;
        if (hAlign == ALIGN_HCENTER)
        {
            xPos += hWhitespace * 0.5f;
        }
        else if (hAlign == ALIGN_RIGHT)
        {
            xPos += hWhitespace;
        }

        lines.push_back(Vector2(xPos, lineWidth));
    }

    float x = FLT_MAX;
    float y = clip.y;
    float width = 0;
    float height = yPos - clip.y;

    // Calculate top of text without clipping.
    float vWhitespace = viewportHeight - height;
    if (vAlign == ALIGN_VCENTER)
    {
        y += vWhitespace * 0.5f;
    }
    else if (vAlign == ALIGN_BOTTOM)
    {
        y += vWhitespace;
    }

    int clippedTop = 0;
    int clippedBottom = 0;
    if (!ignoreClip)
    {
        // Trim rect to fit text that would actually be drawn within the given clip.
        if (y >= clip.y)
        {
            // Text goes off the bottom of the clip.
            clippedBottom = static_cast< int >( (height - viewportHeight) / size ) + 1;
            if (clippedBottom > 0)
            {
                // Also need to crop empty lines above non-empty lines that have been clipped.
                size_t emptyIndex = emptyLines.size() - clippedBottom;
                while (emptyIndex < emptyLines.size() && emptyLines[emptyIndex] == true)
                {
                    height -= size;
                    emptyIndex++;
                }

                height -= size * clippedBottom;
            }
            else
            {
                clippedBottom = 0;
            }
        }
        else
        {
            // Text goes above the top of the clip.
            clippedTop = static_cast< int >( (clip.y - y) / size ) + 1;
            if (clippedTop < 0)
            {
                clippedTop = 0;
            }

            // Also need to crop empty lines below non-empty lines that have been clipped.
            size_t emptyIndex = clippedTop;
            while (emptyIndex < emptyLines.size() && emptyLines[emptyIndex] == true)
            {
                y += size;
                height -= size;
                emptyIndex++;
            }

            if (vAlign == ALIGN_VCENTER)
            {
                // In this case lines may be clipped off the bottom as well.
                clippedBottom = static_cast< int >( (height - viewportHeight + vWhitespace/2 + 0.01) / size ) + 1;
                if (clippedBottom > 0)
                {
                    emptyIndex = emptyLines.size() - clippedBottom;
                    while (emptyIndex < emptyLines.size() && emptyLines[emptyIndex] == true)
                    {
                        height -= size;
                        emptyIndex++;
                    }

                    height -= size * clippedBottom;
                }
                else
                {
                    clippedBottom = 0;
                }
            }

            y = y + size * clippedTop;
            height = height - size * clippedTop;
        }
    }

    // Determine left-most x coordinate and largest width out of lines that have not been clipped.
    for (int i = clippedTop; i < (int)lines.size() - clippedBottom; ++i)
    {
        if (lines[i].x < x)
        {
            x = lines[i].x;
        }
        if (lines[i].y > width)
        {
            width = lines[i].y;
        }
    }

    if (!ignoreClip)
    {
        // Guarantee that the output rect will fit within the clip.
        out->x = (x >= clip.x)? x : clip.x;
        out->y = (y >= clip.y)? y : clip.y;
        out->width = (width <= clip.width)? width : clip.width;
        out->height = (height <= viewportHeight)? height : viewportHeight;
    }
    else
    {
        out->x = x;
        out->y = y;
        out->width = width;
        out->height = height;
    }
}

void Font::getMeasurementInfo(const wchar_t* text, const Rectangle& area, float size, Justify justify, bool wrap, bool rightToLeft,
        std::vector<float>* xPositions, float* yPosition, std::vector<unsigned int>* lineLengths) const 
{
    GP_ASSERT(_size);
    GP_ASSERT(text);
    GP_ASSERT(yPosition);

    if (size == 0)
        size = _size;

    float scale = (float)size / _size;

    Justify vAlign = static_cast<Justify>(justify & 0xF0);
    if (vAlign == 0)
    {
        vAlign = ALIGN_TOP;
    }

    Justify hAlign = static_cast<Justify>(justify & 0x0F);
    if (hAlign == 0)
    {
        hAlign = ALIGN_LEFT;
    }

    const wchar_t* token = text;
    const float areaHeight = area.height - size;

    int spaceIndex = getGlyphIndexByCode(' ');
    float spaceAdvance = spaceIndex >= 0 && spaceIndex < (int)_glyphCount ? _glyphs[spaceIndex].advance * scale : size * 0.5f;

    // For alignments other than top-left, need to calculate the y position to begin drawing from
    // and the starting x position of each line.  For right-to-left text, need to determine
    // the number of characters on each line.
    if (vAlign != ALIGN_TOP || hAlign != ALIGN_LEFT || rightToLeft)
    {
        float lineWidth = 0;
        float delimWidth = 0;

        if (wrap)
        {
            // Go a word at a time.
            bool reachedEOF = false;
            unsigned int lineLength = 0;
            while (token[0] != 0)
            {
                float tokenWidth = 0;

                // Handle delimiters until next token.
                wchar_t delimiter = token[0];
                while (delimiter == L' ' ||
                       delimiter == L'\t' ||
                       delimiter == L'\r' ||
                       delimiter == L'\n' ||
                       delimiter == 0)
                {
                    switch (delimiter)
                    {
                        case L' ':
                            delimWidth += spaceAdvance;
                            lineLength++;
                            break;
                        case L'\r':
                        case L'\n':
                            *yPosition += size;

                            if (lineWidth > 0)
                            {
                                addLineInfo(area, lineWidth, lineLength, hAlign, xPositions, lineLengths, rightToLeft);
                            }

                            lineWidth = 0;
                            lineLength = 0;
                            delimWidth = 0;
                            break;
                        case L'\t':
                            delimWidth += spaceAdvance * 4;
                            lineLength++;
                            break;
                        case 0:
                            reachedEOF = true;
                            break;
                    }

                    if (reachedEOF)
                    {
                        break;
                    }

                    token++;
                    delimiter = token[0];
                }

                if (reachedEOF || token == NULL)
                {
                    break;
                }

                unsigned int tokenLength = (unsigned int)wcscspn(token, L" \r\n\t");
                tokenWidth += getTokenWidth(token, tokenLength, size, scale);

                // Wrap if necessary.
                if (int(lineWidth + tokenWidth + delimWidth) > int(area.width))
                {
                    *yPosition += size;

                    // Push position of current line.
                    if (lineLength)
                    {
                        addLineInfo(area, lineWidth, lineLength-1, hAlign, xPositions, lineLengths, rightToLeft);
                    }
                    else
                    {
                        addLineInfo(area, lineWidth, tokenLength, hAlign, xPositions, lineLengths, rightToLeft);
                    }

                    // Move token to the next line.
                    lineWidth = 0;
                    lineLength = 0;
                    delimWidth = 0;
                }
                else
                {
                    lineWidth += delimWidth;
                    delimWidth = 0;
                }

                lineWidth += tokenWidth;
                lineLength += tokenLength;
                token += tokenLength;
            }

            // Final calculation of vertical position.
            float textHeight = *yPosition - area.y;
            float vWhiteSpace = areaHeight - textHeight;
            if (vAlign == ALIGN_VCENTER)
            {
                *yPosition = area.y + vWhiteSpace * 0.5f;
            }
            else if (vAlign == ALIGN_BOTTOM)
            {
                *yPosition = area.y + vWhiteSpace;
            }

            // Calculation of final horizontal position.
            addLineInfo(area, lineWidth, lineLength, hAlign, xPositions, lineLengths, rightToLeft);
        }
        else
        {
            // Go a line at a time.
            while (token[0] != 0)
            {
                wchar_t delimiter = token[0];
                while (delimiter == L'\n')
                {
                    *yPosition += size;
                    ++token;
                    delimiter = token[0];
                }

                unsigned int tokenLength = (unsigned int)wcscspn(token, L"\n");
                if (tokenLength == 0)
                {
                    tokenLength = (unsigned int)wcslen(token);
                }

                float lineWidth = getTokenWidth(token, tokenLength, size, scale);
                addLineInfo(area, lineWidth, tokenLength, hAlign, xPositions, lineLengths, rightToLeft);

                token += tokenLength;
            }

            float textHeight = *yPosition - area.y;
            float vWhiteSpace = areaHeight - textHeight;
            if (vAlign == ALIGN_VCENTER)
            {
                *yPosition = area.y + vWhiteSpace * 0.5f;
            }
            else if (vAlign == ALIGN_BOTTOM)
            {
                *yPosition = area.y + vWhiteSpace;
            }
        }

        if (vAlign == ALIGN_TOP)
        {
            *yPosition = area.y;
        }
    }
}

float Font::getCharacterSpacing() const
{
    return _spacing;
}

void Font::setCharacterSpacing(float spacing)
{
    _spacing = spacing;
}

int Font::getIndexAtLocation(const wchar_t* text, const Rectangle& area, float size, const Vector2& inLocation, Vector2* outLocation,
                                      Justify justify, bool wrap, bool rightToLeft) const
{
    return getIndexOrLocation(text, area, size, inLocation, outLocation, -1, justify, wrap, rightToLeft);
}

void Font::getLocationAtIndex(const wchar_t* text, const Rectangle& clip, float size, Vector2* outLocation, const unsigned int destIndex,
                              Justify justify, bool wrap, bool rightToLeft) const
{
    getIndexOrLocation(text, clip, size, *outLocation, outLocation, (const int)destIndex, justify, wrap, rightToLeft);
}

int Font::getIndexOrLocation(const wchar_t* text, const Rectangle& area, float size, const Vector2& inLocation, Vector2* outLocation,
                                      const int destIndex, Justify justify, bool wrap, bool rightToLeft) const
{
    GP_ASSERT(_size);
    GP_ASSERT(text);
    GP_ASSERT(outLocation);

    if (size == 0)
    {
        size = _size;
    }
    else
    {
        // Delegate to closest sized font
        const Font* f = findClosestSize(size);
        if (f != this)
        {
            return f->getIndexOrLocation(text, area, size, inLocation, outLocation, destIndex, justify, wrap, rightToLeft);
        }
    }

    unsigned int charIndex = 0;

    // Essentially need to measure text until we reach inLocation.
    float scale = (float)size / _size;
    float spacing = size * _spacing;
    float yPos = area.y;
    const float areaHeight = area.height - size;
    std::vector<float> xPositions;
    std::vector<unsigned int> lineLengths;

    getMeasurementInfo(text, area, size, justify, wrap, rightToLeft, &xPositions, &yPos, &lineLengths);

    float xPos = area.x;
    std::vector<float>::const_iterator xPositionsIt = xPositions.begin();
    if (xPositionsIt != xPositions.end())
    {
        xPos = *xPositionsIt++;
    }

    const wchar_t* token = text;

    int iteration = 1;
    unsigned int lineLength;
    unsigned int currentLineLength = 0;
    const wchar_t* lineStart;
    std::vector<unsigned int>::const_iterator lineLengthsIt;
    if (rightToLeft)
    {
        lineStart = token;
        lineLengthsIt = lineLengths.begin();
        lineLength = *lineLengthsIt++;
        token += lineLength - 1;
        iteration = -1;
    }

    while (token[0] != 0)
    {
        // Handle delimiters until next token.
        unsigned int delimLength = 0;
        int result;
        if (destIndex == -1)
        {
            result = handleDelimiters(&token, size, scale, iteration, area.x, &xPos, &yPos, &delimLength, &xPositionsIt, xPositions.end(), &charIndex, &inLocation);
        }
        else
        {
            result = handleDelimiters(&token, size, scale, iteration, area.x, &xPos, &yPos, &delimLength, &xPositionsIt, xPositions.end(), &charIndex, NULL, charIndex, destIndex);
        }

        currentLineLength += delimLength;
        if (result == 0 || result == 2)
        {
            outLocation->x = xPos;
            outLocation->y = yPos;
            return charIndex;
        }

        if (destIndex == (int)charIndex ||
            (destIndex == -1 &&
             inLocation.x >= xPos && inLocation.x < xPos + spacing &&
             inLocation.y >= yPos && inLocation.y < yPos + size))
        {
            outLocation->x = xPos;
            outLocation->y = yPos;
            return charIndex;
        }

        bool truncated = false;
        unsigned int tokenLength;
        float tokenWidth;
        unsigned int startIndex;
        if (rightToLeft)
        {
            tokenLength = getReversedTokenLength(token, text);
            currentLineLength += tokenLength;
            charIndex += tokenLength;
            token -= (tokenLength - 1);
            tokenWidth = getTokenWidth(token, tokenLength, size, scale);
            iteration = -1;
            startIndex = tokenLength - 1;
        }
        else
        {
            tokenLength = (unsigned int)wcscspn(token, L" \r\n\t");
            tokenWidth = getTokenWidth(token, tokenLength, size, scale);
            iteration = 1;
            startIndex = 0;
        }

        // Wrap if necessary.
        if (wrap && (floorf(xPos + tokenWidth) > area.x + area.width || (rightToLeft && currentLineLength > lineLength)))
        {
            currentLineLength = tokenLength;

            if (xPositionsIt != xPositions.end())
            {
                xPos = *xPositionsIt++;
                yPos += size;
            }
            else
            {
                xPos = area.x;
            }
        }

        if (floorf(yPos) > area.y + areaHeight)
        {
            // Truncate below area's vertical limit.
            break;
        }

        GP_ASSERT(_glyphs);
        for (int i = startIndex; i < (int)tokenLength && i >= 0; i += iteration)
        {
            wchar_t c = token[i];
            int glyphIndex = getGlyphIndexByCode( c );

            if (glyphIndex >= 0 && glyphIndex < (int)_glyphCount)
            {
                Glyph& g = _glyphs[glyphIndex];

                if (floorf(xPos + g.advance*scale) > area.x + area.width)
                {
                    // Truncate this line and go on to the next one.
                    truncated = true;
                    break;
                }

                // Check against inLocation.
                //  Note: g.width is smaller than g.advance, so if I only check against g.width, I will 
                //  miss locations towards the right of the character.
                if (destIndex == (int)charIndex ||
                    (destIndex == -1 &&
                    inLocation.x >= xPos && inLocation.x < xPos + g.advance*scale + spacing &&
                    inLocation.y >= yPos && inLocation.y < yPos + size))
                {
                    outLocation->x = xPos;
                    outLocation->y = yPos;
                    return charIndex;
                }

                xPos += g.advance*scale + spacing;
                charIndex++;
            }
        }

        if (!truncated)
        {
            if (rightToLeft)
            {
                if (token == lineStart)
                {
                    token += lineLength;

                    // Now handle delimiters going forwards.
                    if (!handleDelimiters(&token, size, scale, 1, area.x, &xPos, &yPos, &currentLineLength, &xPositionsIt, xPositions.end()))
                    {
                        break;
                    }
                    charIndex += currentLineLength;

                    if (lineLengthsIt != lineLengths.end())
                    {
                        lineLength = *lineLengthsIt++;
                    }
                    lineStart = token;
                    token += lineLength-1;
                    charIndex += tokenLength;
                }
                else
                {
                    token--;
                }
            }
            else
            {
                token += tokenLength;
            }
        }
        else
        {
            if (rightToLeft)
            {
                token = lineStart + lineLength;

                if (!handleDelimiters(&token, size, scale, 1, area.x, &xPos, &yPos, &currentLineLength, &xPositionsIt, xPositions.end()))
                {
                    break;
                }

                if (lineLengthsIt != lineLengths.end())
                {
                    lineLength = *lineLengthsIt++;
                }
                lineStart = token;
                token += lineLength-1;
            }
            else
            {
                // Skip the rest of this line.
                unsigned int tokenLength = (unsigned int)wcscspn(token, L"\n");

                if (tokenLength > 0)
                {
                    // Get first token of next line.
                    token += tokenLength;
                    charIndex += tokenLength;
                }
            }
        }
    }


    if (destIndex == (int)charIndex ||
        (destIndex == -1 &&
         inLocation.x >= xPos && inLocation.x <= ceilf(xPos + spacing) &&
         inLocation.y >= yPos && inLocation.y <= ceilf(yPos + size)))
    {
        outLocation->x = xPos;
        outLocation->y = yPos;
        return charIndex;
    }

    return -1;
}

float Font::getTokenWidth(const wchar_t* token, unsigned int length, float size, float scale) const
{
    GP_ASSERT(token);
    GP_ASSERT(_glyphs);

    if (size == 0)
        size = _size;
    float spacing = size * _spacing;

    int spaceIndex = getGlyphIndexByCode(' ');
    float spaceAdvance = spaceIndex >= 0 && spaceIndex < (int)_glyphCount ? _glyphs[spaceIndex].advance * scale : size * 0.5f;

    // Calculate width of word or line.
    float tokenWidth = 0;
    for (unsigned int i = 0; i < length; ++i)
    {
        wchar_t c = token[i];
        switch (c)
        {
        case L' ':
            tokenWidth += spaceAdvance;
            break;
        case L'\t':
            tokenWidth += spaceAdvance * 4;
            break;
        default:
            int glyphIndex = getGlyphIndexByCode( c );
            if (glyphIndex >= 0 && glyphIndex < (int)_glyphCount)
            {
                Glyph& g = _glyphs[glyphIndex];
                tokenWidth += g.advance * scale + spacing;
            }
            break;
        }
    }

    return tokenWidth;
}

unsigned int Font::getReversedTokenLength(const wchar_t* token, const wchar_t* bufStart) const
{
    GP_ASSERT(token);
    GP_ASSERT(bufStart);

    const wchar_t* cursor = token;
    wchar_t c = cursor[0];
    unsigned int length = 0;

    while (cursor != bufStart && c != L' ' && c != L'\r' && c != L'\n' && c != L'\t')
    {
        length++;
        cursor--;
        c = cursor[0];
    }

    if (cursor == bufStart)
    {
        length++;
    }

    return length;
}

int Font::handleDelimiters(const wchar_t** token, const float size, float scale, const int iteration, const float areaX, float* xPos, float* yPos, unsigned int* lineLength,
                          std::vector<float>::const_iterator* xPositionsIt, std::vector<float>::const_iterator xPositionsEnd, unsigned int* charIndex,
                          const Vector2* stopAtPosition, const int currentIndex, const int destIndex) const
{
    GP_ASSERT(token);
    GP_ASSERT(*token);
    GP_ASSERT(xPos);
    GP_ASSERT(yPos);
    GP_ASSERT(lineLength);
    GP_ASSERT(xPositionsIt);

    int spaceIndex = getGlyphIndexByCode(' ');
    float spaceAdvance = spaceIndex >= 0 && spaceIndex < (int)_glyphCount ? _glyphs[spaceIndex].advance * scale : size * 0.5f;

    wchar_t delimiter = *token[0];
    bool nextLine = true;
    while (delimiter == L' ' ||
            delimiter == L'\t' ||
            delimiter == L'\r' ||
            delimiter == L'\n' ||
            delimiter == 0)
    {
        if ((stopAtPosition &&
            stopAtPosition->x >= *xPos && stopAtPosition->x < *xPos + size * 0.5f &&
            stopAtPosition->y >= *yPos && stopAtPosition->y < *yPos + size) ||
            (currentIndex >= 0 && destIndex >= 0 && currentIndex + (int)*lineLength == destIndex))
        {
            // Success + stopAtPosition was reached.
            return 2;
        }

        switch (delimiter)
        {
            case L' ':
                *xPos += spaceAdvance;
                (*lineLength)++;
                if (charIndex)
                {
                    (*charIndex)++;
                }
                break;
            case L'\r':
            case L'\n':
                *yPos += size;

                // Only use next xPos for first newline character (in case of multiple consecutive newlines).
                if (nextLine)
                {
                    if (*xPositionsIt != xPositionsEnd)
                    {
                        *xPos = **xPositionsIt;
                        (*xPositionsIt)++;
                    }
                    else
                    {
                        *xPos = areaX;
                    }
                    nextLine = false;
                    *lineLength = 0;
                    if (charIndex)
                    {
                        (*charIndex)++;
                    }
                }
                break;
            case L'\t':
                *xPos += spaceAdvance * 4;
                (*lineLength)++;
                if (charIndex)
                {
                    (*charIndex)++;
                }
                break;
            case 0:
                // EOF reached.
                return 0;
        }

        *token += iteration;
        delimiter = *token[0];
    }

    // Success.
    return 1;
}

void Font::addLineInfo(const Rectangle& area, float lineWidth, int lineLength, Justify hAlign,
                       std::vector<float>* xPositions, std::vector<unsigned int>* lineLengths, bool rightToLeft) const
{
    float hWhitespace = area.width - lineWidth;
    if (hAlign == ALIGN_HCENTER)
    {
        GP_ASSERT(xPositions);
        (*xPositions).push_back(area.x + hWhitespace / 2);
    }
    else if (hAlign == ALIGN_RIGHT)
    {
        GP_ASSERT(xPositions);
        (*xPositions).push_back(area.x + hWhitespace);
    }

    if (rightToLeft)
    {
        GP_ASSERT(lineLengths);
        (*lineLengths).push_back(lineLength);
    }
}

SpriteBatch* Font::getSpriteBatch(unsigned int size) const
{
    if (size == 0)
        return _batch;

    // Find the closest sized child font
    return const_cast<Font*>(this)->findClosestSize(size)->_batch;
}

Font::Justify Font::getJustify(const char* justify)
{
    if (!justify)
    {
        return Font::ALIGN_TOP_LEFT;
    }

    if (strcmpnocase(justify, "ALIGN_LEFT") == 0)
    {
        return Font::ALIGN_LEFT;
    }
    else if (strcmpnocase(justify, "ALIGN_HCENTER") == 0)
    {
        return Font::ALIGN_HCENTER;
    }
    else if (strcmpnocase(justify, "ALIGN_RIGHT") == 0)
    {
        return Font::ALIGN_RIGHT;
    }
    else if (strcmpnocase(justify, "ALIGN_TOP") == 0)
    {
        return Font::ALIGN_TOP;
    }
    else if (strcmpnocase(justify, "ALIGN_VCENTER") == 0)
    {
        return Font::ALIGN_VCENTER;
    }
    else if (strcmpnocase(justify, "ALIGN_BOTTOM") == 0)
    {
        return Font::ALIGN_BOTTOM;
    }
    else if (strcmpnocase(justify, "ALIGN_TOP_LEFT") == 0)
    {
        return Font::ALIGN_TOP_LEFT;
    }
    else if (strcmpnocase(justify, "ALIGN_VCENTER_LEFT") == 0)
    {
        return Font::ALIGN_VCENTER_LEFT;
    }
    else if (strcmpnocase(justify, "ALIGN_BOTTOM_LEFT") == 0)
    {
        return Font::ALIGN_BOTTOM_LEFT;
    }
    else if (strcmpnocase(justify, "ALIGN_TOP_HCENTER") == 0)
    {
        return Font::ALIGN_TOP_HCENTER;
    }
    else if (strcmpnocase(justify, "ALIGN_VCENTER_HCENTER") == 0)
    {
        return Font::ALIGN_VCENTER_HCENTER;
    }
    else if (strcmpnocase(justify, "ALIGN_BOTTOM_HCENTER") == 0)
    {
        return Font::ALIGN_BOTTOM_HCENTER;
    }
    else if (strcmpnocase(justify, "ALIGN_TOP_RIGHT") == 0)
    {
        return Font::ALIGN_TOP_RIGHT;
    }
    else if (strcmpnocase(justify, "ALIGN_VCENTER_RIGHT") == 0)
    {
        return Font::ALIGN_VCENTER_RIGHT;
    }
    else if (strcmpnocase(justify, "ALIGN_BOTTOM_RIGHT") == 0)
    {
        return Font::ALIGN_BOTTOM_RIGHT;
    }
    else
    {
        GP_WARN("Invalid alignment string: '%s'. Defaulting to ALIGN_TOP_LEFT.", justify);
    }

    // Default.
    return Font::ALIGN_TOP_LEFT;
}


int Font::getGlyphIndexByCode( int characterCode ) const
{
    for( unsigned i = 0; i < _glyphCount; i++ )
    {
        if( _glyphs[ i ].code == characterCode )
            return i;
    }

    return -1;
}

const Font::Glyph * Font::getGlyphByCode( int characterCode ) const
{
    for( unsigned i = 0; i < _glyphCount; i++ )
    {
        if( _glyphs[ i ].code == characterCode )
            return &_glyphs[ i ];
    }

    return NULL;
}

}
