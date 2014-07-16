#ifndef NULLSTOREFRONT_H_
#define NULLSTOREFRONT_H_

#include "Storefront.h"


namespace gameplay
{

/**
 * Defines a storefront that doesn't support payments at all.
 * This class acts like a placeholder for platforms that have no dedicated Store.
 *
 * @script{ignore}
 */
class NullStoreFront : public StoreFront
{
    friend class StoreController;

public:
    /**
     * Returns the name of the Store.
     */
    virtual const char * getName( ) const {return "Null";};

    /**
     * Sets the asynchronous response listener that registered for this session.
     */
    virtual void setListener(class StoreListener* listener);
    
    /**
     * Gets the asynchronous response listener that registered for this session.
     *
     * @return The asynchronous response listener that registered for this session.
     */
    virtual class StoreListener* getListener();

    /**
     * Determine whether the user can make payments.
     */
    virtual bool canMakePayments() const {return false;};

    /**
     * Get list of the products.
     *
     * NullStoreFront marks all products as invalid.
     *
     * @param productIDs    Null-terminated array of product identifiers.
     */
    virtual void getProducts(const char ** productIDs) const;
    
    /**
     * Start a payment transaction.
     */
    virtual void makePayment(const char * productID, int quantity, const char * usernameHash);

    /**
     * Get a product shipping cost in product's currency.
     */
    virtual float getShippingCost( const StoreProduct& product, int quantity ) const {return 0;};

    /**
     * Restore completed transactions.
     */
    virtual void restoreTransactions( const char * usernameHash ) {};
    
protected:

    /**
     * Contructor
     */
    NullStoreFront();

    /**
     * Destructor
     */
    virtual ~NullStoreFront();

private:
    class StoreListener * _listener;
};

}

#endif
