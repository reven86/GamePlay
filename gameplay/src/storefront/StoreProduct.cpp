#include "Base.h"
#include "StoreProduct.h"

namespace gameplay
{

StoreProduct::StoreProduct( const std::string& _id, const std::string& _name, const std::string& _description, float _price )
    : id( _id ), name( _name ), description( _description ), price( _price )
{
}

StoreProduct::~StoreProduct()
{
}

}


