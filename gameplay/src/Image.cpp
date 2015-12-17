#include "Base.h"
#include "FileSystem.h"
#include "Image.h"
#include <jpeg/jpeglib.h>

namespace gameplay
{
// Callback for reading a png image using Stream
static void readStream(png_structp png, png_bytep data, png_size_t length)
{
    Stream* stream = reinterpret_cast<Stream*>(png_get_io_ptr(png));
    if (stream == NULL || stream->read(data, 1, length) != length)
    {
        png_error(png, "Error reading PNG.");
    }
}

// Callback for writing a png image using Stream
static void writeStream(png_structp png, png_bytep data, png_size_t length)
{
    Stream* stream = reinterpret_cast<Stream*>(png_get_io_ptr(png));
    if (stream->write(data, 1, length) != length)
    {
        png_error(png, "Error writing PNG.");
    }
}

Image* Image::create(const char* path)
{
    GP_ASSERT(path);

    // Open the file.
    std::unique_ptr<Stream> stream(FileSystem::open(path));
    if (stream.get() == NULL || !stream->canRead())
    {
        GP_ERROR("Failed to open image file '%s'.", path);
        return NULL;
    }

    // Verify PNG signature.
    unsigned char sig[8];
    if (stream->read(sig, 1, 8) == 8 && png_sig_cmp(sig, 0, 8) == 0)
        return createPNG(stream.get(), path);

    stream->rewind();
    return createJPEG(stream.get(), path);
}

Image * Image::createPNG(Stream * stream, const char * path)
{
    // Initialize png read struct (last three parameters use stderr+longjump if NULL).
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png == NULL)
    {
        GP_ERROR("Failed to create PNG structure for reading PNG file '%s'.", path);
        return NULL;
    }

    // Initialize info struct.
    png_infop info = png_create_info_struct(png);
    if (info == NULL)
    {
        GP_ERROR("Failed to create PNG info structure for PNG file '%s'.", path);
        png_destroy_read_struct(&png, NULL, NULL);
        return NULL;
    }

    // Set up error handling (required without using custom error handlers above).
    if (setjmp(png_jmpbuf(png)))
    {
        GP_ERROR("Failed to set up error handling for reading PNG file '%s'.", path);
        png_destroy_read_struct(&png, &info, NULL);
        return NULL;
    }

    // Initialize file io.
    png_set_read_fn(png, stream, readStream);

    // Indicate that we already read the first 8 bytes (signature).
    png_set_sig_bytes(png, 8);

    // Read the entire image into memory.
    png_read_png(png, info, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_GRAY_TO_RGB, NULL);

    Image* image = new Image();
    image->_width = png_get_image_width(png, info);
    image->_height = png_get_image_height(png, info);

    png_byte colorType = png_get_color_type(png, info);
    switch (colorType)
    {
    case PNG_COLOR_TYPE_RGBA:
        image->_format = Image::RGBA;
        break;

    case PNG_COLOR_TYPE_RGB:
        image->_format = Image::RGB;
        break;

    default:
        GP_ERROR("Unsupported PNG color type (%d) for image file '%s'.", (int)colorType, path);
        png_destroy_read_struct(&png, &info, NULL);
        return NULL;
    }

    size_t stride = png_get_rowbytes(png, info);

    // Allocate image data.
    image->_data = new unsigned char[stride * image->_height];

    // Read rows into image data.
    png_bytepp rows = png_get_rows(png, info);
    for (unsigned int i = 0; i < image->_height; ++i)
    {
        memcpy(image->_data+(stride * i), rows[i], stride);
    }

    // Clean up.
    png_destroy_read_struct(&png, &info, NULL);

    return image;
}

Image * Image::createJPEG(Stream * stream, const char * path)
{
    struct jpeg_decompress_struct info; //for our jpeg info
    struct jpeg_error_mgr err;          //the error handler

    // read entire file into memory
    std::unique_ptr<unsigned char[]> buffer(new unsigned char[stream->length()]);
    if (stream->read(buffer.get(), stream->length(), 1) != 1)
    {
        GP_ERROR("Failed to read file '%s'.", path);
        return NULL;
    }

    info.err = jpeg_std_error(&err);
    jpeg_create_decompress(&info);   //fills info structure

    jpeg_mem_src(&info, buffer.get(), stream->length());
    jpeg_read_header(&info, TRUE);   // read jpeg file header

    jpeg_start_decompress(&info);    // decompress the file

    if (info.num_components != 3)
    {
        jpeg_finish_decompress(&info);   //finish decompressing
        jpeg_destroy_decompress(&info);
        GP_ERROR("JPEG loader only supports images with 3 channels: '%s'.", path);
        return NULL;
    }

    Image* image = new Image();
    image->_width = info.output_width;
    image->_height = info.output_height;
    image->_format = Image::RGB;

    int dataSize = image->_width * image->_height * 3;

    //--------------------------------------------
    // read scanlines one at a time & put bytes 
    //    in jdata[] array. Assumes an RGB image
    //--------------------------------------------
    // Allocate image data.
    image->_data = new unsigned char[image->_width * image->_height * 3];

    unsigned char * rowptr[1];
    while (info.output_scanline < info.output_height) // loop
    {
        // Enable jpeg_read_scanlines() to fill our jdata array
        rowptr[0] = (unsigned char *)image->_data + 3 * info.output_width * info.output_scanline;
        jpeg_read_scanlines(&info, rowptr, 1);
    }
    //---------------------------------------------------

    jpeg_finish_decompress(&info);   //finish decompressing
    jpeg_destroy_decompress(&info);

    return image;
}

Image* Image::create(unsigned int width, unsigned int height, Image::Format format, unsigned char* data)
{
    GP_ASSERT(width > 0 && height > 0);
    GP_ASSERT(format >= RGB && format <= RGBA);

    unsigned int pixelSize = 0;
    switch(format)
    {
    case Image::RGB:
        pixelSize = 3;
        break;
    case Image::RGBA:
        pixelSize = 4;
        break;
    }

    Image* image = new Image();

    unsigned int dataSize = width * height * pixelSize;

    image->_width = width;
    image->_height = height;
    image->_format = format;
    image->_data = new unsigned char[dataSize];
    if (data)
        memcpy(image->_data, data, dataSize);

    return image;
}

Image::Image() : _data(NULL), _format(RGB), _width(0), _height(0)
{
}

Image::~Image()
{
    SAFE_DELETE_ARRAY(_data);
}

bool Image::writePNG(gameplay::Stream * stream)
{
    if (stream == NULL || !stream->canWrite())
    {
        GP_WARN("Stream doesn't support writing operations.");
        return false;
    }

    /* initialize stuff */
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        return false;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_write_struct(&png_ptr, NULL);
        return false;
    }

    /* Set up error handling. */
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return false;
    }

    /* Set image attributes. */
    png_set_IHDR(png_ptr,
        info_ptr,
        _width,
        _height,
        8,
        _format == RGB ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */
    unsigned y;
    uint32_t bpp = _format == RGB ? 3 : 4;
    png_byte **row_pointers = reinterpret_cast<png_byte **>(png_malloc(png_ptr, _height * sizeof(png_byte *)));
    for (y = 0; y < _height; ++y) 
        row_pointers[_height - 1 - y] = (png_byte *)(_data + y * _width * bpp);

    /* Actually write the image data. */
    png_set_write_fn(png_ptr, stream, writeStream, NULL);
    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* Cleanup. */
    png_free(png_ptr, row_pointers);

    /* Finish writing. */
    png_destroy_write_struct(&png_ptr, &info_ptr);

    return true;
}

}
