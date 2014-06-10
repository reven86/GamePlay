 #include "Base.h"

#if defined (__APPLE__) && defined (GP_USE_STOREFRONT)

#include "StoreListener.h"

#import "AppleStoreFront.h"
#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>



@interface AppleStoreKitController : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver>
{
    @public gameplay::AppleStoreFront * _storeFront;
}
@end

@implementation AppleStoreKitController

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    GP_ASSERT( _storeFront->getListener( ) );
    
    std::vector< gameplay::StoreProduct > products;
    std::vector< std::string > invalidProducts;
    
    NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
    [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
    [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
    for (SKProduct * product in response.products)
    {
        [numberFormatter setLocale:product.priceLocale];
        NSString *formattedString = [numberFormatter stringFromNumber:product.price];
        
        products.push_back(gameplay::StoreProduct(
            [product.productIdentifier UTF8String],
            [product.localizedTitle UTF8String],
            [product.localizedDescription UTF8String],
            [product.price floatValue],
            [formattedString UTF8String]
            ));
    }
    [numberFormatter release];
    
    for (NSString *invalidProductId in response.invalidProductIdentifiers)
    {
        invalidProducts.push_back([invalidProductId UTF8String]);
    }
    
    _storeFront->getListener()->getProductsEvent(products, invalidProducts);
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
    GP_ASSERT( _storeFront->getListener( ) );
    //NSLog( @"Payments request fail %@", error );
    
    _storeFront->getListener()->getProductsFailedEvent(error.code, [error.localizedDescription UTF8String]);
    
    [request release];
}

- (void)requestDidFinish:(SKRequest *)request
{
    //NSLog( @"Payments request is completed" );
    [request release];
}

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
                if( _storeFront->getListener()->paymentTransactionSucceededEvent([transaction.payment.productIdentifier UTF8String], transaction.payment.quantity, [transaction.transactionDate timeIntervalSince1970], [transaction.transactionIdentifier UTF8String]) )
                {
                    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                }
                break;
            case SKPaymentTransactionStateFailed:
                _storeFront->getListener()->paymentTransactionFailedEvent([transaction.payment.productIdentifier UTF8String], transaction.payment.quantity, transaction.error.code, [transaction.error.localizedDescription UTF8String]);
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                if( _storeFront->getListener()->paymentTransactionRestoredEvent([transaction.originalTransaction.payment.productIdentifier UTF8String], transaction.originalTransaction.payment.quantity, [transaction.originalTransaction.transactionDate timeIntervalSince1970], [transaction.originalTransaction.transactionIdentifier UTF8String]) )
                {
                    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                }
                break;
            default:
                GP_ASSERT( transaction.transactionState == SKPaymentTransactionStatePurchasing );
                _storeFront->getListener()->paymentTransactionInProcessEvent([transaction.payment.productIdentifier UTF8String], transaction.payment.quantity);
                break;
        }
    }
}

@end


static AppleStoreKitController * gStoreKitController = nil;


namespace gameplay {


AppleStoreFront::AppleStoreFront()
    : _listener( NULL )
{
    if( gStoreKitController == nil )
    {
        gStoreKitController = [[AppleStoreKitController alloc] init];
        gStoreKitController->_storeFront = this;
        [[SKPaymentQueue defaultQueue] addTransactionObserver:gStoreKitController];
    }
}

AppleStoreFront::~AppleStoreFront()
{
    if( gStoreKitController != nil )
    {
        [[SKPaymentQueue defaultQueue] removeTransactionObserver:gStoreKitController];
        [gStoreKitController release];
        gStoreKitController = nil;
    }
}
    
void AppleStoreFront::setListener(class StoreListener *listener)
{
    _listener = listener;
}

StoreListener* AppleStoreFront::getListener()
{
    return _listener;
}

bool AppleStoreFront::canMakePayments() const
{
    return [SKPaymentQueue canMakePayments];
}
    
void AppleStoreFront::getProducts(const char ** productIDs) const
{
    NSMutableArray * productsArray = [[NSMutableArray alloc] init];
    while( *productIDs )
    {
        [productsArray addObject:[NSString stringWithUTF8String:*productIDs]];
        productIDs++;
    }
    
    SKProductsRequest *productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:[NSSet setWithArray:productsArray]];
    productsRequest.delegate = gStoreKitController;
    [productsRequest start];
    
    [productsArray release];
}
    
void AppleStoreFront::makePayment(const char * productID, int quantity, const char * usernameHash)
{
    SKMutablePayment * payment = [SKMutablePayment paymentWithProductIdentifier:[NSString stringWithUTF8String:productID]];
    payment.quantity = quantity;
#if __IPHONE_OS_VERSION_MIN_REQUIRED >= 70000
    if( usernameHash )
        payment.applicationUsername = [NSString stringWithUTF8String:usernameHash];
#endif
    [[SKPaymentQueue defaultQueue] addPayment:payment];
}

float AppleStoreFront::getShippingCost( const gameplay::StoreProduct& product, int quantity ) const
{
    return product.price * quantity * 0.3f;
}

}

#endif
