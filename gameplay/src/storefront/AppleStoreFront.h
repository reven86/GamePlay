#if defined (__APPLE__) && defined (GP_USE_STOREFRONT)

#ifndef APPLESTOREFRONT_H_
#define APPLESTOREFRONT_H_

#include "StoreFront.h"


namespace gameplay
{

/**
 * Defines an class for store front that uses StoreKit.
 *
 * @script{ignore}
 */
class AppleStoreFront : public StoreFront
{
    friend class StoreController;

public:
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
     * @param productIDs    Null-terminated array of product identifiers.
     */
    virtual void getProducts( const char ** productIDs ) const;
    
    /**
     * Start a payment transaction.
     */
    virtual void makePayment(const char * productID, int quantity, const char * usernameHash = NULL);
    
protected:
    
    /**
     * Contructor
     */
    AppleStoreFront();
    
    /**
     * Destructor
     */
    virtual ~AppleStoreFront();
    
private:
    StoreListener * _listener;
};

}

#endif
#endif
