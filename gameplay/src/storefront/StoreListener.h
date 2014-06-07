#ifndef STORELISTENER_H_
#define STORELISTENER_H_

#include "StoreProduct.h"

namespace gameplay
{

/**
 * Defines a class for handling storefront responses.
 *
 * @script{ignore}
 */
class StoreListener
{
public:
    
    /**
     * Constructor
     */
    StoreListener() {};
    
    /**
     * Destructor
     */
    virtual ~StoreListener() {};
    
    virtual void getProductsEvent( const std::vector<StoreProduct>& products, const std::vector<std::string>& invalidProducts ) {};
    
    virtual void getProductsFailedEvent( const char * error ) {};
    
    virtual void paymentTransactionInProcessEvent( const char * productID, int quantity ) {};

    /**
     * Return true for this event to complete the transaction.
     */
    virtual bool paymentTransactionSucceededEvent( const char * productID, int quantity, double timestamp, const char * transactionID ) { return true; };
    
    virtual void paymentTransactionFailedEvent( const char * productID, int quantity, const char * error ) {};
    
    /**
     * Return true for this event to complete the transaction.
     */
    virtual bool paymentTransactionRestoredEvent( const char * productID, int quantity, double timestamp, const char * transactionID ) { return true; };
};

}


#endif
