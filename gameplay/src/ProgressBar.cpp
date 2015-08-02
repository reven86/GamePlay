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
    progressBar->_id = id ? id : "";
    progressBar->initialize("ProgressBar", style, NULL);

    return progressBar;
}

Control* ProgressBar::create(Theme::Style* style, Properties* properties)
{
    ProgressBar* progressBar = new ProgressBar();
    progressBar->initialize("Image", style, properties);
    return progressBar;
}

void ProgressBar::initialize(const char* typeName, Theme::Style* style, Properties* properties)
{
	Control::initialize(typeName, style, properties);

    if(properties)
    {
        _value = properties->getFloat("value");
        if( properties->exists( "orientation" ) )
            _orientation = strcmpnocase( properties->getString("orientation"), "VERTICAL" ) == 0 ? ORIENTATION_VERTICAL : ORIENTATION_HORIZONTAL;
    }
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

void ProgressBar::updateState(State state)
{
    Control::updateState(state);

    _trackImage = getImage("track", _state);
}

unsigned ProgressBar::drawImages(Form* form) const
{
    if(!_trackImage)
        return 0;

    const Theme::UVs& track = _trackImage->getUVs();
    Vector4 trackColor = _trackImage->getColor();

    trackColor.w *= _opacity;

    // Draw order: track, caps, marker.
    SpriteBatch* batch = _style->getTheme()->getSpriteBatch();
    startBatch(form, batch);
    if( _orientation == ORIENTATION_HORIZONTAL )
        batch->draw(
            _viewportBounds.x, _viewportBounds.y, _viewportBounds.width * _value, _viewportBounds.height, 
            track.u1, track.v1, track.u1 + ( track.u2 - track.u1 ) * _value, track.v2, trackColor, _viewportClipBounds);
    else
        batch->draw(
            _viewportBounds.x, _viewportBounds.y + _viewportBounds.height * ( 1.0f - _value ), 
            _viewportBounds.width, _viewportBounds.height * _value, 
            track.u1, track.v2 + ( track.v1 - track.v2 ) * _value, track.u2, track.v2, trackColor, _viewportClipBounds);

    finishBatch(form, batch);
    return 1;
}

const char* ProgressBar::getTypeName() const
{
    return "ProgressBar";
}

}
