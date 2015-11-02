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
     * The string that identifies the product to the Store provider.
     */
    std::string id;

    /**
     * The localized UTF8 name of the product.
     */
    std::string name;

    /**
     * The localized UTF8 description of the product.
     */
    std::string description;

    /**
     * The product's price in local currency.
     */
    float price;

    /**
     * The product's localized price string in local currency (UTF-8).
     */
    std::string localizedPrice;
    
    /**
     * The product's currency code ISO 4217.
     */
    std::string currencyCode;
    
    /**
     * Constructor
     */
    StoreProduct( const char * _id, const char * _name, const char * _description, float _price, const char * _localizedPrice, const char * _currencyCode );
    
    /**
     * Destructor
     */
    ~StoreProduct();
};

}

#endif