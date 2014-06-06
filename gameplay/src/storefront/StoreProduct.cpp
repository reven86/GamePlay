#include "Base.h"
#include "StoreProduct.h"

namespace gameplay
{

StoreProduct::StoreProduct( const char * _id, const char * _name,
    const char * _description, float _price, const char * _localizedPrice )
    : id( _id ), name( _name ), description( _description ), price( _price )
    , localizedPrice( _localizedPrice )
{
}

StoreProduct::~StoreProduct()
{
}

}


