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
    
    /**
     * Get called when information about submitted products is received from store.
     *
     * @param products List of valid products.
     * @param invalidProducts IDs of invalid products. These products can't purchased and should not be displayed to user.
     *
     * @see StoreFront::getProducts
     */
    virtual void getProductsEvent( const std::vector<StoreProduct>& products, const std::vector<std::string>& invalidProducts ) {};
    
    /**
     * Get called in case of error in obtaining products' information.
     *
     * @param errorCode Error code.
     * @param errorUTF8 UTF-8 error string.
     */
    virtual void getProductsFailedEvent( int errorCode, const char * errorUTF8 ) {};
    
    /**
     * Transaction is being processed event.
     *
     * @param productID Product for which transaction is being processed.
     * @param quantity Product's quantity.
     *
     * @see StoreFront::makePayment.
     */
    virtual void paymentTransactionInProcessEvent( const char * productID, int quantity ) {};

    /**
     * Transaction is succeeded.
     * You must manually call StoreFront::finishTransaction to remove transaction from the queue,
     * when you've done processing and validating it.
     *
     * @param productID Product for which transaction is being processed.
     * @param quantity Product's quantity.
     * @param timestamp UNIX timestamp for a transaction.
     * @param transactionID Transaction ID.
     * @param transactionObject Transaction object.
     *
     * @see StoreFront::makePayment.
     */
    virtual void paymentTransactionSucceededEvent(const char * productID, int quantity, double timestamp, const char * transactionID, void * transactionObject) {};
    
    /**
     * Transaction is failed.
     *
     * @param productID Product's ID.
     * @param quantity Product's quantity.
     * @param errorCode Error code.
     * @param error UTF-8 error string.
     *
     * @see StoreFront::makePayment.
     */
    virtual void paymentTransactionFailedEvent( const char * productID, int quantity, int errorCode, const char * error ) {};
    
    /**
     * Previous transaction is restored.
     * You must manually call StoreFront::finishTransaction to remove transaction from the queue,
     * when you've done processing and validating it.
     *
     * @param productID Product's ID.
     * @param quantity Product's quantity.
     * @param timestamp Original transaction's UNIX timestamp.
     * @param transactionID Original transaction's ID.
     * @param transactionObject Transaction object.
     *
     * @see StoreFront::makePayment.
     */
    virtual void paymentTransactionRestoredEvent( const char * productID, int quantity, double timestamp, const char * transactionID, void * transactionObject ) {};

    /**
     * Return true if the item should be consumed.
     * Note: this method is not called for App Store market.
     */
    virtual bool isProductConsumable(const char * productID) { return true; };

    /**
     * Up-to-date receipt file has been returned.
     *
     * @param receiptFile Contents of receipt when receipt file was successfully requested.
     * @param errorCode Error code in case of any error or 0.
     * @param errorUTF8 Error string in UTF8 coding or empty string.
     */
    virtual void receiptRequested(void * receiptFile, int errorCode, const char * errorUTF8);
};

}


#endif
