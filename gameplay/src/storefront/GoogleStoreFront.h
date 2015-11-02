#ifndef GOOGLESTOREFRONT_H_
#define GOOGLESTOREFRONT_H_

#include "Storefront.h"


namespace gameplay
{

/**
 * Defines a Google Play storefront.
 *
 * @script{ignore}
 */
class GoogleStoreFront : public StoreFront
{
    friend class StoreController;

public:
    /**
     * Returns the name of the Store.
     */
    virtual const char * getName( ) const {return "GooglePlay";};

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
    virtual bool canMakePayments() const;

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
    virtual float getShippingCost( const StoreProduct& product, int quantity ) const;

    /**
     * Restore completed transactions.
     */
    virtual void restoreTransactions( const char * usernameHash );
    
    /**
     * Finish transaction and remove it from the queue.
     */
    virtual void finishTransaction(void * transactionObject);

//protected:

    /**
     * Contructor
     */
    GoogleStoreFront();

    /**
     * Destructor
     */
    virtual ~GoogleStoreFront();

private:
    class StoreListener * _listener;
};

}

#endif
