#include "Base.h"
#include "StoreProduct.h"

namespace gameplay
{

StoreProduct::StoreProduct( const char * _id, const char * _name,
    const char * _description, float _price, const char * _localizedPrice,
    const char * _currencyCode )
    : id( _id ), name( _name ? _name : "(null)" ), description( _description ? _description : "(null)" ), price( _price )
    , localizedPrice( _localizedPrice ? _localizedPrice : "(null)" ), currencyCode( _currencyCode )
{
}

StoreProduct::~StoreProduct()
{
}

}


