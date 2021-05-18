/*
    Copyright 2016 Andrew Karpushin (reven86@gmail.com).

    This source file is a Derivative Work from orignal Gameplay3D source files.
    The Gameplay3D project is distributed under the terms of Apache 2.0 License.
    Original Gameplay3D source files can be found at https://github.com/gameplay3d/GamePlay

    Changes to orginal document were done in lines: 2, 9, 57-159.
*/

#include "Base.h"
#include "ImageControl.h"
#include "Material.h"

namespace gameplay
{

ImageControl::ImageControl() :
    _srcRegion(Rectangle::empty()), _dstRegion(Rectangle::empty()), _batch(NULL),
    _tw(0.0f), _th(0.0f), _uvs(Theme::UVs::full()), _color(Vector4::one())
{
}

ImageControl::~ImageControl()
{
    SAFE_DELETE(_batch);
}

ImageControl* ImageControl::create(const char* id, Theme::Style* style)
{
    ImageControl* imageControl = new ImageControl();
    imageControl->_id = id ? id : "";
    imageControl->initialize("Image", style, NULL);
    return imageControl;
}

Control* ImageControl::create(Theme::Style* style, Properties* properties)
{
    ImageControl* imageControl = new ImageControl();
    imageControl->initialize("Image", style, properties);
    return imageControl;
}

void ImageControl::initialize(const char* typeName, Theme::Style* style, Properties* properties)
{
	Control::initialize(typeName, style, properties);

	if (properties)
	{
		std::string path;
		if (properties->getPath("path", &path))
		{
			setImage(path.c_str());
		}

		if (properties->exists("srcRegion"))
		{
			Vector4 region;
			properties->getVector4("srcRegion", &region);
			setRegionSrc(region.x, region.y, region.z, region.w);
		}

		if (properties->exists("dstRegion"))
		{
			Vector4 region;
			properties->getVector4("dstRegion", &region);
			setRegionDst(region.x, region.y, region.z, region.w);
		}

        if (properties->exists("imageColor"))
        {
            Vector4 color(0, 0, 0, 1);
            properties->getColor("imageColor", &color);
            setColor(color);
        }
	}
}

const char* ImageControl::getTypeName() const
{
    return "ImageControl";
}

void ImageControl::setImage(const char* path)
{
    SAFE_DELETE(_batch);

    // check for '.material' extension
    std::string pathString(path);
    std::transform(pathString.begin(), pathString.end(), pathString.begin(), (int(*)(int))tolower);
    
    size_t pathLen = strlen(path);
    if( pathLen > 9 && !strcmp( pathString.c_str() + pathLen - 9, ".material" ) )
    {
        Material* material = Material::create(path);
        _batch = SpriteBatch::create(material);
        _tw = 1.0f / _batch->getSampler()->getTexture()->getWidth();
        _th = 1.0f / _batch->getSampler()->getTexture()->getHeight();
        SAFE_RELEASE(material);
    }
    else
    {
        Texture* texture = Texture::create(path);
        _batch = SpriteBatch::create(texture);
        _tw = 1.0f / texture->getWidth();
        _th = 1.0f / texture->getHeight();
        texture->release();
        _batch->getSampler()->setWrapMode(Texture::CLAMP, Texture::CLAMP);
    }


    if (_autoSize != AUTO_SIZE_NONE)
        setDirty(DIRTY_BOUNDS);
}

void ImageControl::setImage(Image * image)
{
    SAFE_DELETE(_batch);

    Texture * texture = Texture::create(image);
    _batch = SpriteBatch::create(texture);
    _tw = 1.0f / texture->getWidth();
    _th = 1.0f / texture->getHeight();
    texture->release();
    _batch->getSampler()->setWrapMode(Texture::CLAMP, Texture::CLAMP);

    if (_autoSize != AUTO_SIZE_NONE)
        setDirty(DIRTY_BOUNDS);
}

void ImageControl::setRegionSrc(float x, float y, float width, float height)
{
    _srcRegion.set(x, y, width, height);

    _uvs.u1 = x * _tw;
    _uvs.u2 = (x + width) * _tw;
    _uvs.v1 = y * _th;
    _uvs.v2 = (y + height) * _th;
}

void ImageControl::setRegionSrc(const Rectangle& region)
{
    setRegionSrc(region.x, region.y, region.width, region.height);
}

const Rectangle& ImageControl::getRegionSrc() const
{
    return _srcRegion;
}

void ImageControl::setRegionDst(float x, float y, float width, float height)
{
    _dstRegion.set(x, y, width, height);
}

void ImageControl::setRegionDst(const Rectangle& region)
{
    setRegionDst(region.x, region.y, region.width, region.height);
}

const Rectangle& ImageControl::getRegionDst() const
{
    return _dstRegion;
}

unsigned int ImageControl::drawImages(Form* form) const
{
    if (!_batch)
        return 0;

    startBatch(form, _batch);

    Vector4 color = _color;
    color.w *= _opacity;

    if (_dstRegion.isEmpty())
    {
        _batch->draw(_viewportBounds.x, _viewportBounds.y, _viewportBounds.width, _viewportBounds.height,
            _uvs.u1, _uvs.v1, _uvs.u2, _uvs.v2, color, _viewportClipBounds);
    }
    else
    {
        _batch->draw(_viewportBounds.x + _dstRegion.x, _viewportBounds.y + _dstRegion.y,
            _dstRegion.width, _dstRegion.height,
            _uvs.u1, _uvs.v1, _uvs.u2, _uvs.v2, color, _viewportClipBounds);
    }

    finishBatch(form, _batch);

    return 1;
}

void ImageControl::updateBounds()
{
    if (_batch)
    {
        if (_autoSize & AUTO_SIZE_WIDTH)
        {
            setWidthInternal(_batch->getSampler()->getTexture()->getWidth());
        }

        if (_autoSize & AUTO_SIZE_HEIGHT)
        {
            setHeightInternal(_batch->getSampler()->getTexture()->getHeight());
        }
    }

    Control::updateBounds();
}

const SpriteBatch * ImageControl::getSpriteBatch() const
{
    return _batch;
}

const Vector4& ImageControl::getColor() const
{
    return _color;
}

void ImageControl::setColor(const Vector4& color)
{
    _color = color;
}

}
