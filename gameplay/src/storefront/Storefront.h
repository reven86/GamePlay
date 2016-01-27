#ifndef STOREFRONT_H_
#define STOREFRONT_H_


namespace gameplay
{

/**
 * Defines an abstract class for typical store front.
 *
 * @script{ignore}
 */
class StoreFront
{
    friend class StoreController;

public:
    /**
     * Returns the name of the Store.
     */
    virtual const char * getName( ) const = 0;

    /**
     * Sets the asynchronous response listener that registered for this session.
     */
    virtual void setListener(class StoreListener* listener) = 0;
    
    /**
     * Gets the asynchronous response listener that registered for this session.
     *
     * @return The asynchronous response listener that registered for this session.
     */
    virtual class StoreListener* getListener() = 0;

    /**
     * Determine whether the user can make payments.
     */
    virtual bool canMakePayments() const = 0;

    /**
     * Get list of the products.
     *
     * @param productIDs    Null-terminated array of product identifiers.
     */
    virtual void getProducts(const char ** productIDs) const = 0;
    
    /**
     * Start a payment transaction.
     */
    virtual void makePayment(const char * productID, int quantity, const char * usernameHash = NULL) = 0;

    /**
     * Get a product's shipping cost in product's currency.
     */
    virtual float getShippingCost( const struct StoreProduct& product, int quantity ) const = 0;

    /**
     * Restore completed transactions.
     */
    virtual void restoreTransactions( const char * usernameHash ) = 0;

    /**
     * Finish transaction and remove it from the queue.
     */
    virtual void finishTransaction(void * transactionObject) = 0;

    /**
     * Request up-to-date receipt file. Receipt is a platform specific
     * container, that stores all information about purchases made by
     * user. Receipt can be used to restore previous purchases of non-consumable
     * items or to be sent to server to validate purchases as it is 
     * digitally signed by Store's certificate. The internal format of 
     * receipt file is dependent on platform, where it is used.
     */
    virtual void requestReceipt() = 0;

protected:

    /**
     * Contructor
     */
    StoreFront() { };

    /**
     * Destructor
     */
    virtual ~StoreFront() { };

};

}

#endif
