#include "Base.h"
#include "NullStoreFront.h"
#include "StoreListener.h"

namespace gameplay
{



NullStoreFront::NullStoreFront( )
    : _listener( NULL )
{
}

NullStoreFront::~NullStoreFront( )
{
}

void NullStoreFront::setListener( StoreListener * listener )
{
    _listener = listener;
}

StoreListener * NullStoreFront::getListener( )
{
    return _listener;
}

void NullStoreFront::getProducts( const char ** productIDs ) const
{
    std::vector< std::string > invalidProducts;

    while( *productIDs )
        invalidProducts.push_back( *productIDs++ );

    GP_ASSERT( _listener );
    _listener->getProductsEvent( std::vector< gameplay::StoreProduct >( ), invalidProducts );
}

void NullStoreFront::makePayment( const char * productID, int quantity, const char * usernameHash )
{
}

}