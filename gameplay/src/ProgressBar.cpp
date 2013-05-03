#include "ProgressBar.h"

namespace gameplay
{

ProgressBar::ProgressBar() : _value(0.0f), _trackImage(NULL), _orientation( ORIENTATION_HORIZONTAL )
{
}

ProgressBar::~ProgressBar()
{
}

ProgressBar* ProgressBar::create(const char* id, Theme::Style* style)
{
    GP_ASSERT(style);

    ProgressBar* progressBar = new ProgressBar();
    if (id)
        progressBar->_id = id;
    progressBar->setStyle(style);

    return progressBar;
}

ProgressBar* ProgressBar::create(Theme::Style* style, Properties* properties)
{
    GP_ASSERT(properties);

    ProgressBar* progressBar = new ProgressBar();
    progressBar->initialize(style, properties);

    progressBar->_value = properties->getFloat("value");
    if( properties->exists( "orientation" ) )
        progressBar->_orientation = strcmp( properties->getString("orientation"), "VERTICAL" ) == 0 ? ORIENTATION_VERTICAL : ORIENTATION_HORIZONTAL;

    return progressBar;
}

float ProgressBar::getValue() const
{
    return _value;
}

void ProgressBar::setValue(float value)
{
    _value = MATH_CLAMP(value, 0.0f, 1.0f);
}

const ProgressBar::OrientationType& ProgressBar::getOrientation( ) const
{
    return _orientation;
}

void ProgressBar::setOrientation( const OrientationType& orientation )
{
    _orientation = orientation;
}

void ProgressBar::update(const Control* container, const Vector2& offset)
{
    Control::update(container, offset);

    _trackImage = getImage("track", _state);
}

void ProgressBar::drawImages(SpriteBatch* spriteBatch, const Rectangle& clip)
{
    GP_ASSERT(spriteBatch);
    GP_ASSERT(_trackImage);

    const Rectangle& trackRegion = _trackImage->getRegion();
    const Theme::UVs& track = _trackImage->getUVs();
    Vector4 trackColor = _trackImage->getColor();

    trackColor.w *= _opacity;

    // Draw order: track, caps, marker.
    if( _orientation == ORIENTATION_HORIZONTAL )
        spriteBatch->draw(
            _viewportBounds.x, _viewportBounds.y, _viewportBounds.width * _value, _viewportBounds.height, 
            track.u1, track.v1, track.u1 + ( track.u2 - track.u1 ) * _value, track.v2, trackColor, _viewportClipBounds);
    else
        spriteBatch->draw(
            _viewportBounds.x, _viewportBounds.y + _viewportBounds.height * ( 1.0f - _value ), 
            _viewportBounds.width, _viewportBounds.height * _value, 
            track.u1, track.v2 + ( track.v1 - track.v2 ) * _value, track.u2, track.v2, trackColor, _viewportClipBounds);
}

const char* ProgressBar::getType() const
{
    return "progressbar";
}

}
