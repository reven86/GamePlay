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

    enum ProductType
    {
        CONSUMABLE,
        NONCONSUMABLE,
        RENEWABLE_SUBSCRIPTION,
        NONRENEWABLE_SUBSCRIPTION
    };

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
     * Start a payment transaction.
     */
    virtual void makePayment(const char * productID, int quantity) = 0;

protected:

    /**
     * Contructor
     */
    StoreFront() { }

    /**
     * Destructor
     */
    virtual ~StoreFront() { }

};

}

#endif
