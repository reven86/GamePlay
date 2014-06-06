#include "Base.h"
#include "StoreController.h"
#include "Game.h"
#include "AppleStoreFront.h"

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
#elif defined(__APPLE__) && (__MAC_OS_X_VERSION_MIN_REQUIRED >= 1070 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 50000)
    _storeFront = new AppleStoreFront( );
#endif
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

}
