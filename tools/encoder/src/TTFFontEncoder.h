#include <ft2build.h>
#include FT_FREETYPE_H

#define START_INDEX     32
#define END_INDEX       127
#define GLYPH_PADDING   4

namespace gameplay
{

// Structure of Glyph.
class Glyph
{
public:
    unsigned int index;
    unsigned int width;
    float uvCoords[4];
};

/**
 * Writes the font gpb file.
 * 
 * @param inFilePath Input file path to the tiff file.
 * @param outFilePath Output file path to write the gpb to.
 * @param fontSize Size of the font.
 * @param id ID string of the font in the ref table.
 * @param fontpreview True if the pgm font preview file should be written. (For debugging)
 * @param characterSet Character set used to draw image or NULL (standard ASCII chars will be used).
 * 
 * @return 0 if successful, -1 if error.
 */
int writeFont(const char* inFilePath, const char* outFilePath, unsigned int fontSize, const char* id, bool fontpreview, const wchar_t * characterSet);


/**
 * Writes the font gpb file from PNG image.
 * 
 * @param inFilePath Input file path to the tiff file.
 * @param outFilePath Output file path to write the gpb to.
 * @param fontSize Size of the font.
 * @param id ID string of the font in the ref table.
 * @param characterSet Character set used in image.
 * 
 * @return 0 if successful, -1 if error.
 */
int writeFontFromImage(const char* inFilePath, const char* outFilePath, unsigned int fontSize, const char* id, const wchar_t * characterSet);

}
