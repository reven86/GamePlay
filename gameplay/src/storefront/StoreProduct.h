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
    const std::string id;

    /**
     * The localized UTF8 name of the product (read-only).
     */
    const std::string name;

    /**
     * The localized UTF8 description of the product (read-only).
     */
    const std::string description;

    /**
     * The product's price in local currency (read-only).
     */
    const float price;

    /**
     * Constructor
     */
    StoreProduct( const std::string& _id, const std::string& _name, const std::string& _description, float _price );

    /**
     * Destructor
     */
    ~StoreProduct();
};

}

#endif