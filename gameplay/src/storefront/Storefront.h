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
    virtual void getProducts( const char ** productIDs ) const = 0;
    
    /**
     * Start a payment transaction.
     */
    virtual void makePayment(const char * productID, int quantity, const char * usernameHash) = 0;

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