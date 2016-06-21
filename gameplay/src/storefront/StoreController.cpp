#include "Base.h"
#include "StoreController.h"
#include "Game.h"
#include "AppleStoreFront.h"
#include "NullStoreFront.h"
#include "GoogleStoreFront.h"

namespace gameplay
{

StoreController::StoreController()
    : _storeFront(NULL)
{
}

StoreController::~StoreController()
{
}

void StoreController::initialize()
{
#if defined(GP_USE_STOREFRONT)
#if defined(__QNX__)
#elif defined(__ANDROID__)
    _storeFront = new GoogleStoreFront( );
#elif defined(__APPLE__) && (__MAC_OS_X_VERSION_MIN_REQUIRED >= 1070 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 50000)
    _storeFront = new AppleStoreFront( );
#else
    _storeFront = new NullStoreFront( );
#endif
#else
    _storeFront = new NullStoreFront( );
#endif
}

void StoreController::finalize()
{
    SAFE_DELETE( _storeFront );
}

void StoreController::pause()
{
}

void StoreController::resume()
{
}

void StoreController::update(float elapsedTime)
{
}

void StoreController::replaceStoreFront(StoreFront * storeFront)
{
    // reassign listener as well
    StoreListener * listener = _storeFront ? _storeFront->getListener() : NULL;

    SAFE_DELETE(_storeFront);
    _storeFront = storeFront;

    if (_storeFront && listener)
        _storeFront->setListener(listener);
}

}
