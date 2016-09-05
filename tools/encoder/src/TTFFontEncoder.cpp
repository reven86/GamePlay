#include "Base.h"
#include "TTFFontEncoder.h"
#include "GPBFile.h"
#include "StringUtil.h"
#include "Image.h"

#ifdef WIN32
#include <windows.h>
#include <wingdi.h>
#endif

namespace gameplay
{

static void drawBitmap(unsigned char* dstBitmap, int x, int y, int dstWidth, unsigned char* srcBitmap, int srcWidth, int srcHeight)
{
    // offset dst bitmap by x,y.
    dstBitmap +=  (x + (y * dstWidth));

    for (int i = 0; i < srcHeight; ++i)
    {
        memcpy(dstBitmap, (const void*)srcBitmap, srcWidth);
        srcBitmap += srcWidth;
        dstBitmap += dstWidth;
    }
}

static void writeUint(FILE* fp, unsigned int i)
{
    fwrite(&i, sizeof(unsigned int), 1, fp);
}

static void writeString(FILE* fp, const char* str)
{
    unsigned int len = strlen(str);
    fwrite(&len, sizeof(unsigned int), 1, fp);
    if (len > 0)
    {
        fwrite(str, 1, len, fp);
    }
}

unsigned char* createDistanceFields(unsigned char* img, unsigned int width, unsigned int height)
{
    short* xDistance = (short*)malloc(width * height * sizeof(short));
    short* yDistance = (short*)malloc(width * height * sizeof(short));
    double* gx = (double*)calloc(width * height, sizeof(double));
    double* gy = (double*)calloc(width * height, sizeof(double));
    double* data = (double*)calloc(width * height, sizeof(double));
    double* outside = (double*)calloc(width * height, sizeof(double));
    double* inside = (double*)calloc(width * height, sizeof(double));
    unsigned int i;

    // Convert img into double (data)
    double imgMin = 255;
    double imgMax = -255;
    for (i = 0; i < width * height; ++i)
    {
        double v = img[i];
        data[i] = v;
        if (v > imgMax) 
            imgMax = v;
        if (v < imgMin) 
            imgMin = v;
    }
    // Rescale image levels between 0 and 1
    for (i = 0; i < width * height; ++i)
    {
        data[i] = (img[i] - imgMin) / imgMax;
    }
    // Compute outside = edtaa3(bitmap); % Transform background (0's)
    computegradient(data, width, height, gx, gy);
    edtaa3(data, gx, gy, height, width, xDistance, yDistance, outside);
    for (i = 0; i < width * height; ++i)
    {
        if (outside[i] < 0 )
            outside[i] = 0.0;
    }
    // Compute inside = edtaa3(1-bitmap); % Transform foreground (1's)
    memset(gx, 0, sizeof(double) * width * height);
    memset(gy, 0, sizeof(double) * width * height);
    for (i = 0; i < width * height; ++i)
    {
        data[i] = 1 - data[i];
    }
    computegradient(data, width, height, gx, gy);
    edtaa3(data, gx, gy, height, width, xDistance, yDistance, inside);
    for (i = 0; i < width * height; ++i)
    {
        if( inside[i] < 0 )
            inside[i] = 0.0;
    }
    // distmap = outside - inside; % Bipolar distance field
    unsigned char* out = (unsigned char*)malloc(sizeof(unsigned char) * width * height);
    for (i = 0; i < width * height; ++i)
    {
        outside[i] -= inside[i];
        outside[i] = 128 + outside[i] * 16;
        if (outside[i] < 0) 
            outside[i] = 0;
        if (outside[i] > 255) 
            outside[i] = 255;
        out[i] = 255 - (unsigned char) outside[i];
    }
    free(xDistance);
    free(yDistance);
    free(gx);
    free(gy);
    free(data);
    free(outside);
    free(inside);

    return out;
}

struct KerningPair
{
    wchar_t left;
    wchar_t right;
    int kerning;
};

// Stores a single genreated font size to be written into the GPB
struct FontData
{
    // Array of glyphs for a font
    TTFGlyph * glyphArray;

    int fontSize;

    // Actual size of the underlying glyphs (may be different from fontSize)
    int glyphSize;

    // Font texture
    unsigned char* imageBuffer;
    unsigned int imageWidth;
    unsigned int imageHeight;

    // Kerning pairs
    std::vector<KerningPair> kerningPairs;

    FontData() : fontSize(0), glyphSize(0), imageBuffer(NULL), imageWidth(0), imageHeight(0)
    {
    }

    ~FontData()
    {
        if (imageBuffer)
            free(imageBuffer);
    }
};
 
int writeFont(const std::vector<const char*>& inFilePath, const char* outFilePath, std::vector<unsigned int>& fontSizes, const char* id, bool fontpreview = false, Font::FontFormat fontFormat = Font::BITMAP, const wchar_t * characterSet = NULL)
{
    // Initialize freetype library.
    FT_Library library;
    FT_Error error = FT_Init_FreeType(&library);
    if (error)
    {
        LOG(1, "FT_Init_FreeType error: %d \n", error);
        return -1;
    }

    // Initialize font face.
    std::vector<FT_Face> faces;

    for (const char * filename : inFilePath)
    {
        FT_Face face;
        error = FT_New_Face(library, filename, 0, &face);
        if (error)
        {
            LOG(1, "FT_New_Face error: %d \n", error);
            return -1;
        }

        faces.push_back(face);
    }

    std::vector<FontData*> fonts;

    if( characterSet == NULL || wcslen( characterSet ) == 0 )
    {
        wchar_t * defaultSet = reinterpret_cast< wchar_t * >( calloc( END_INDEX - START_INDEX + 1, sizeof( wchar_t ) ) );
        for( wchar_t ch = START_INDEX; ch < END_INDEX; ch++ )
            defaultSet[ ch - START_INDEX ] = ch;
        defaultSet[ END_INDEX - START_INDEX] = NULL;

        characterSet = defaultSet;
    }

    // retrieve kerning pairs
    // freetype has no API to access kerning pairs table,
    // use brute-force attempt to retrieve all pair for current characters set
    std::vector<std::pair<wchar_t, wchar_t> > kerningPairs;

    // freetype has limited support processing kerning table for OTF fonts, it
    // retrieve data from the outdated 'kern' table instead of GPOS table.
    // Use MSDN functions to retrieve full kerning pairs and offsets
#if 0//def WIN32
    AddFontResourceExA(inFilePath, FR_PRIVATE, 0);
//#else
    for (const wchar_t * ascii1 = characterSet; *ascii1; ++ascii1)
    {
        for (const wchar_t * ascii2 = characterSet; *ascii2; ++ascii2)
        {
            FT_Vector out;
            if (FT_Get_Kerning(face, *ascii1, *ascii2, FT_KERNING_UNSCALED, &out) == 0 && out.x != 0)
                kerningPairs.push_back(std::make_pair(*ascii1, *ascii2));
        }
    }
#endif

    for (size_t fontIndex = 0, count = fontSizes.size(); fontIndex < count; ++fontIndex)
    {
        unsigned int fontSize = fontSizes[fontIndex];

        FontData* font = new FontData();
        font->fontSize = fontSize;
        font->glyphArray = reinterpret_cast<TTFGlyph *>(malloc(wcslen(characterSet) * sizeof(TTFGlyph)));
        if (!font->glyphArray)
        {
            LOG(1, "Not enough memory to allocate glyphs.");
            return -1;
        }

        TTFGlyph* glyphArray = font->glyphArray;

        int rowSize = 0;
        int glyphSize = 0;
        int actualfontHeight = 0;
        int maxTopY = 0;
        int minBottomY = 0;

        FT_GlyphSlot slot = NULL;
        FT_Int32 loadFlags = FT_LOAD_RENDER;

        // We want to generate fonts that fit exactly the requested pixels size.
        // Since free type (due to modern fonts) does not directly correlate requested
        // size to glyph size, we'll brute-force attempt to set the largest font size
        // possible that will fit within the requested pixel size.
        for (unsigned int requestedSize = (unsigned)(fontSize * 1.3f); requestedSize > 0; --requestedSize)
        {
            // Set the pixel size.
            for (FT_Face& face : faces)
            {
                error = FT_Set_Char_Size(face, 0, requestedSize * 64, 0, 0);
                if (error)
                {
                    LOG(1, "FT_Set_Pixel_Sizes error: %d \n", error);
                    return -1;
                }
            }

            rowSize = 0;
            glyphSize = 0;
            actualfontHeight = 0;
            maxTopY = 0;
            minBottomY = 0;

            // Find the width of the image.
            for (const wchar_t * ascii = characterSet; *ascii; ++ascii)
            {
                // Save glyph information (slot contains the actual glyph bitmap).
                slot = faces[0]->glyph;
                error = FT_Load_Glyph(faces[0], 0, loadFlags);

                // Load glyph image into the slot (erase previous one)
                for (FT_Face& face : faces)
                {
                    FT_UInt charIndex = FT_Get_Char_Index(face, *ascii);
                    if (charIndex == 0)
                        continue;

                    error = FT_Load_Glyph(face, charIndex, loadFlags);
                    if (error)
                    {
                        LOG(1, "FT_Load_Char error : %d \n", error);
                    }
                    slot = face->glyph;
                    break;
                }

                int bitmapRows = slot->bitmap.rows;
                actualfontHeight = (actualfontHeight < bitmapRows) ? bitmapRows : actualfontHeight;

                int topY = slot->bitmap_top;
                int bottomY = topY - bitmapRows;
                if (topY > maxTopY)
                    maxTopY = topY;
                if (bottomY < minBottomY)
                    minBottomY = bottomY;
            }

            rowSize = maxTopY - minBottomY;

            // Have we found a pixel size that fits?
            if (rowSize <= (int)fontSize)
            {
                glyphSize = rowSize;
                rowSize = fontSize;

                LOG(1, "Font's size is %d.\n", requestedSize);
                break;
            }
        }

        if (slot == NULL || glyphSize == 0)
        {
            LOG(1, "Cannot generate a font of the requested size: %d\n", fontSize);
            return -1;
        }

        // Include padding in the rowSize.
        rowSize += GLYPH_PADDING;

        // Initialize with padding.
        int penX = 1;
        int penY = 0;
        int row = 0;

        double powerOf2 = 2;
        unsigned int imageWidth = 0;
        unsigned int imageHeight = 0;
        bool textureSizeFound = false;

        int advance;
        int i;

        while (textureSizeFound == false)
        {
            imageWidth = (unsigned int)pow(2.0, powerOf2);
            imageHeight = (unsigned int)pow(2.0, powerOf2);
            penX = 1;
            penY = 0;
            row = 0;

            // Find out the squared texture size that would fit all the require font glyphs.
            i = 0;
            // Find the width of the image.
            for (const wchar_t * ascii = characterSet; *ascii; ++ascii)
            {
                // Save glyph information (slot contains the actual glyph bitmap).
                slot = faces[0]->glyph;

                // Load glyph image into the slot (erase previous one)
                for (FT_Face& face : faces)
                {
                    FT_UInt charIndex = FT_Get_Char_Index(face, *ascii);
                    if (charIndex == 0)
                        continue;

                    error = FT_Load_Glyph(face, charIndex, loadFlags);
                    if (error)
                    {
                        LOG(1, "FT_Load_Char error : %d \n", error);
                    }
                    slot = face->glyph;
                    break;
                }

                // Glyph image.
                int glyphWidth = slot->bitmap.pitch;
                int glyphHeight = slot->bitmap.rows;

                advance = glyphWidth + GLYPH_PADDING;

                // If we reach the end of the image wrap aroud to the next row.
                if ((penX + advance) > (int)imageWidth)
                {
                    penX = 1;
                    row += 1;
                    penY = row * rowSize;
                    if (penY + rowSize > (int)imageHeight)
                    {
                        powerOf2++;
                        break;
                    }
                }

                // Set the pen position for the next glyph
                penX += advance; // Move X to next glyph position
                // Move Y back to the top of the row.
                penY = row * rowSize;

                if (*(ascii + 1) == L'\0')
                {
                    textureSizeFound = true;
                }
                i++;
            }
        }

        // Try further to find a tighter texture size.
        powerOf2 = 1;
        for (;;)
        {
            if ((penY + rowSize) >= pow(2.0, powerOf2))
            {
                powerOf2++;
            }
            else
            {
                imageHeight = (int)pow(2.0, powerOf2);
                break;
            }
        }

        // Allocate temporary image buffer to draw the glyphs into.
        unsigned char* imageBuffer = (unsigned char*)malloc(imageWidth * imageHeight);
        memset(imageBuffer, 0, imageWidth * imageHeight);
        penX = 1;
        penY = 0;
        row = 0;
        i = 0;
        for (const wchar_t * ascii = characterSet; *ascii; ++ascii)
        {
            // Save glyph information (slot contains the actual glyph bitmap).
            slot = faces[0]->glyph;

            // Load glyph image into the slot (erase previous one)
            for (FT_Face& face : faces)
            {
                FT_UInt charIndex = FT_Get_Char_Index(face, *ascii);
                if (charIndex == 0)
                    continue;

                error = FT_Load_Glyph(face, charIndex, loadFlags);
                if (error)
                {
                    LOG(1, "FT_Load_Char error : %d \n", error);
                }
                slot = face->glyph;
                break;
            }

            // Glyph image.
            unsigned char* glyphBuffer = slot->bitmap.buffer;
            int glyphWidth = slot->bitmap.pitch;
            int glyphHeight = slot->bitmap.rows;

            advance = glyphWidth + GLYPH_PADDING;

            // If we reach the end of the image wrap aroud to the next row.
            if ((penX + advance) > (int)imageWidth)
            {
                penX = 1;
                row += 1;
                penY = row * rowSize;
                if (penY + rowSize > (int)imageHeight)
                {
                    free(imageBuffer);
                    LOG(1, "Image size exceeded!");
                    return -1;
                }
            }

            // move pen to baseline
            penY += maxTopY;
            
            // set pen now to the appropriate height of the drawn glyph
            penY -= slot->bitmap_top;

            // Draw the glyph to the bitmap with a one pixel padding.
            drawBitmap(imageBuffer, penX, penY, imageWidth, glyphBuffer, glyphWidth, glyphHeight);

            // Move Y back to the top of the row.
            penY = row * rowSize;

            glyphArray[i].index = *ascii;
            glyphArray[i].width = advance - GLYPH_PADDING;
            glyphArray[i].bearingX = slot->metrics.horiBearingX >> 6;
            glyphArray[i].advance = slot->metrics.horiAdvance >> 6;

            // Generate UV coords.
            glyphArray[i].uvCoords[0] = (float)penX / (float)imageWidth;
            glyphArray[i].uvCoords[1] = (float)penY / (float)imageHeight;
            glyphArray[i].uvCoords[2] = (float)(penX + advance - GLYPH_PADDING) / (float)imageWidth;
            glyphArray[i].uvCoords[3] = (float)(penY + rowSize - GLYPH_PADDING) / (float)imageHeight;

            // Set the pen position for the next glyph
            penX += advance;
            i++;
        }

        font->glyphSize = glyphSize;
        font->imageBuffer = imageBuffer;
        font->imageWidth = imageWidth;
        font->imageHeight = imageHeight;

        // get kerning pairs for font of specific size
#if 0//def WIN32
        HDC hDC = GetDC(NULL); //CreateDCA("DISPLAY", NULL, NULL, NULL);

        LOGFONTA fontStruct;
        memset(&fontStruct, 0, sizeof(fontStruct));
        fontStruct.lfHeight = -(int)fontSize;// -MulDiv(actualfontHeight, GetDeviceCaps(hDC, LOGPIXELSY), 72);
        fontStruct.lfWeight = FW_DONTCARE;
        fontStruct.lfOutPrecision = OUT_OUTLINE_PRECIS;
        fontStruct.lfCharSet = DEFAULT_CHARSET;
        strcpy_s(fontStruct.lfFaceName, face->family_name);

        HFONT fnt = CreateFontIndirectA(&fontStruct);

        SelectObject(hDC, fnt);
        GetTextFaceA(hDC, 32, fontStruct.lfFaceName);

        DWORD numberOfKerningPairs = GetKerningPairs(hDC, INT_MAX, NULL);
        KERNINGPAIR * pairs = new KERNINGPAIR[numberOfKerningPairs];
        GetKerningPairs(hDC, numberOfKerningPairs, pairs);
        for (unsigned int i = 0; i < numberOfKerningPairs; i++)
        {
            if (pairs[i].iKernAmount != 0 && wcschr(characterSet, pairs[i].wFirst) != NULL && wcschr(characterSet, pairs[i].wSecond) != NULL)
                font->kerningPairs.push_back(KerningPair{ pairs[i].wFirst, pairs[i].wSecond, pairs[i].iKernAmount });
        }
        delete[] pairs;

        DeleteObject(fnt);
        //DeleteDC(hDC);
//#else
        for (const std::pair<wchar_t, wchar_t>& kernPair : kerningPairs)
        {
            FT_Vector kerning;
            if (FT_Get_Kerning(face, kernPair.first, kernPair.second, FT_KERNING_DEFAULT, &kerning) == 0 && (kerning.x >> 6) != 0)
                font->kerningPairs.push_back(KerningPair{ kernPair.first, kernPair.second, kerning.x >> 6 });
        }
#endif

        fonts.push_back(font);
    }

#if 0//def WIN32
    RemoveFontResourceExA(inFilePath, FR_PRIVATE, 0);
#endif

    // File header and version.
    FILE *gpbFp = fopen(outFilePath, "wb");    
    char fileHeader[9]     = {'\xAB', 'G', 'P', 'B', '\xBB', '\r', '\n', '\x1A', '\n'};
    fwrite(fileHeader, sizeof(char), 9, gpbFp);
    fwrite(gameplay::GPB_VERSION, sizeof(char), 2, gpbFp);

    // Write Ref table (for a single font)
    writeUint(gpbFp, 1);                // Ref[] count
    writeString(gpbFp, id);             // Ref id
    writeUint(gpbFp, 128);              // Ref type
    writeUint(gpbFp, ftell(gpbFp) + 4); // Ref offset (current pos + 4 bytes)

    // Family name.
    writeString(gpbFp, faces[0]->family_name);

    // Style.
    // TODO: Switch based on TTF style name and write appropriate font style unsigned int for now just hardcoding to 0 = PLAIN.
    writeUint(gpbFp, 0);

    // Number of included font sizes (GPB version 1.3+)
    writeUint(gpbFp, (unsigned int)fonts.size());

    for (size_t i = 0, count = fonts.size(); i < count; ++i)
    {
        FontData* font = fonts[i];

        // Font size (pixels).
        writeUint(gpbFp, font->fontSize);

        // Character set. TODO: Empty for now
        writeString(gpbFp, "");

        // Glyphs.
        unsigned int glyphSetSize = wcslen( characterSet );
        writeUint(gpbFp, glyphSetSize);
        for (unsigned int j = 0; j < glyphSetSize; j++)
        {
            writeUint(gpbFp, font->glyphArray[j].index);
            writeUint(gpbFp, font->glyphArray[j].width);
            fwrite(&font->glyphArray[j].bearingX, sizeof(int), 1, gpbFp);
            writeUint(gpbFp, font->glyphArray[j].advance);
            fwrite(&font->glyphArray[j].uvCoords, sizeof(float), 4, gpbFp);
        }

        // Image dimensions
        unsigned int imageSize = font->imageWidth * font->imageHeight;
        writeUint(gpbFp, font->imageWidth);
        writeUint(gpbFp, font->imageHeight);
        writeUint(gpbFp, imageSize);

        FILE* previewFp = NULL;
        std::string pgmFilePath;
        if (fontpreview)
        {
            // Save out a pgm monochome image file for preview
            std::ostringstream pgmFilePathStream;
            pgmFilePathStream << getFilenameNoExt(outFilePath) << "-" << font->fontSize << ".pgm";
            pgmFilePath = pgmFilePathStream.str();
            previewFp = fopen(pgmFilePath.c_str(), "wb");
            fprintf(previewFp, "P5 %u %u 255\n", font->imageWidth, font->imageHeight);
        }

        if (fontFormat == Font::DISTANCE_FIELD)
        {
            // Flip height and width since the distance field map generator is column-wise.
            unsigned char* distanceFieldBuffer = createDistanceFields(font->imageBuffer, font->imageHeight, font->imageWidth);

            fwrite(distanceFieldBuffer, sizeof(unsigned char), imageSize, gpbFp);
            writeUint(gpbFp, Font::DISTANCE_FIELD);

            if (previewFp)
            {
                fwrite((const char*)distanceFieldBuffer, sizeof(unsigned char), imageSize, previewFp);
                fclose(previewFp);
            }

            free(distanceFieldBuffer);
        }
        else
        {
            fwrite(font->imageBuffer, sizeof(unsigned char), imageSize, gpbFp);
            writeUint(gpbFp, Font::BITMAP);

            if (previewFp)
            {
                fwrite((const char*)font->imageBuffer, sizeof(unsigned char), font->imageWidth * font->imageHeight, previewFp);
                fclose(previewFp);
            }
        }

        if (previewFp)
        {
            fclose(previewFp);
            LOG(1, "%s.pgm preview image created successfully. \n", getBaseName(pgmFilePath).c_str());
        }
    }

    // Close file.
    fclose(gpbFp);

    LOG(1, "%s.gpb created successfully. \n", getBaseName(outFilePath).c_str());

    for (size_t i = 0, count = fonts.size(); i < count; ++i)
    {
        delete fonts[i];
    }

    for (FT_Face& face : faces)
        FT_Done_Face(face);
    FT_Done_FreeType(library);
    return 0;
}

int writeFontFromImage(const char* inFilePath, const char* outFilePath, unsigned int fontSize, const char* id, const wchar_t * characterSet)
{
    Image * image = Image::create( inFilePath );
    if( image == NULL )
    {
        LOG( 1, "Can't load image file: %s", inFilePath );
        return -1;
    }

    Glyph * glyphArray = reinterpret_cast< Glyph * >( calloc( wcslen( characterSet ), sizeof( Glyph ) ) );
    if( !glyphArray )
    {
        LOG( 1, "Not enough stack memory to allocate glyphs." );
        return -1;
    }

    if( image->getFormat( ) != Image::RGBA || image->getBpp( ) != 4 )
    {
        delete image;
        LOG( 1, "Only RGBA images are supported." );
        return -1;
    }

    unsigned width = image->getWidth( );
    unsigned height = image->getHeight( );

    unsigned charHeight = fontSize;

    unsigned long * data = reinterpret_cast< unsigned long * >( image->getData( ) );
    unsigned long * scanline = data;
    unsigned long * oldscanline = scanline;
    unsigned long breakcolor = *scanline & 0x00ffffff;
    unsigned x = 0;
    unsigned y = 0;
    int glyphsCount = 0;

    while( *characterSet )
    {
        wchar_t ch = *characterSet++;

        if( ch == L'\n' )
        {
            scanline += width * charHeight;
            x = 0;
            y += charHeight;

            if( y >= height )
            {
                delete image;
                LOG( 1, "Invalid image pixels data or character set." );
                return -1;
            }

            oldscanline = scanline;
        }
        else if( ch != L'\r' )
        {
            unsigned long * newscanline = oldscanline + 1;
            unsigned charWidth = 1;

            while( ( *newscanline & 0x00ffffff ) != breakcolor && x + charWidth < width )
            {
                newscanline++;
                charWidth++;
            }
            oldscanline = newscanline;

            glyphArray[glyphsCount].index = ch;
            glyphArray[glyphsCount].width = charWidth;
        
            // Generate UV coords.
            glyphArray[glyphsCount].uvCoords[0] = (float)x / (float)width;
            glyphArray[glyphsCount].uvCoords[1] = (float)( y + 1 ) / (float)height;
            glyphArray[glyphsCount].uvCoords[2] = (float)( x + charWidth ) / (float)width;
            glyphArray[glyphsCount].uvCoords[3] = (float)( y + charHeight + 1 ) / (float)height;

            x += charWidth;
            glyphsCount++;
        }
    }


    FILE *gpbFp = fopen(outFilePath, "wb");
    
    // File header and version.
    char fileHeader[9]     = {'�', 'G', 'P', 'B', '�', '\r', '\n', '\x1A', '\n'};
    fwrite(fileHeader, sizeof(char), 9, gpbFp);
    fwrite(gameplay::GPB_VERSION, sizeof(char), 2, gpbFp);

    // Write Ref table (for a single font)
    writeUint(gpbFp, 1);                // Ref[] count
    writeString(gpbFp, id);             // Ref id
    writeUint(gpbFp, 128);              // Ref type
    writeUint(gpbFp, ftell(gpbFp) + 4); // Ref offset (current pos + 4 bytes)
    
    // Write Font object.
    
    // Family name.
    writeString(gpbFp, id);

    // Style.
    // TODO: Switch based on TTF style name and write appropriate font style unsigned int
    // For now just hardcoding to 0.
    //char* style = face->style_name;
    writeUint(gpbFp, 5); // 5 == TEXTURED

    // Font size.
    writeUint(gpbFp, charHeight);

    // Character set.
    // TODO: Empty for now
    writeString(gpbFp, "");//characterSet);
    
    // Glyphs.
    writeUint(gpbFp, glyphsCount);
    fwrite(glyphArray, sizeof(Glyph), glyphsCount, gpbFp);
    
    // Texture.
    unsigned int textureSize = width * height * 4;
    writeUint(gpbFp, width);
    writeUint(gpbFp, height);
    writeUint(gpbFp, textureSize);
    fwrite(image->getData( ), sizeof(unsigned char), textureSize, gpbFp);
    
    // Close file.
    fclose(gpbFp);
    delete image;

    LOG(1, "%s.gpb created successfully. \n", getBaseName(outFilePath).c_str());

    return 0;
}

}
