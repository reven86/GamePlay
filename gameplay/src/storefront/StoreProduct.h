#ifndef STOREFRONT_PRODUCT_H_
#define STOREFRONT_PRODUCT_H_

namespace gameplay
{

/**
 * Defines a base product.
 *
 * This can either be the consumable, non-consumable or renewable product.
 *
 * @script{ignore}
 */
struct StoreProduct
{
    /**
     * The string that identifies the product to the Store provider (read-only).
     */
    std::string id;

    /**
     * The localized UTF8 name of the product (read-only).
     */
    std::string name;

    /**
     * The localized UTF8 description of the product (read-only).
     */
    std::string description;

    /**
     * The product's price in local currency (read-only).
     */
    float price;

    /**
     * The product's localized price in local currency (read-only).
     */
    std::string localizedPrice;
    
    /**
     * Constructor
     */
    StoreProduct( const char * _id, const char * _name, const char * _description, float _price, const char * _localizedPrice );
    
    /**
     * Destructor
     */
    ~StoreProduct();
};

}

#endif