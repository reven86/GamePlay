#include "Base.h"

//
// List of error codes: https://developer.apple.com/library/ios/documentation/StoreKit/Reference/StoreKitTypes/Reference/reference.html
//

#if defined (__APPLE__) && defined (GP_USE_STOREFRONT)

#include "StoreListener.h"

#import "AppleStoreFront.h"



@interface AppleStoreKitController : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver>
{
    @public gameplay::AppleStoreFront * _storeFront;
    NSArray * products;
    @public bool _observerAdded;
    @public SKPaymentTransaction * _currentTransactionObject;
}
@property (strong, nonatomic) NSArray * products;
@end

@implementation AppleStoreKitController
@synthesize products;

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    GP_ASSERT( _storeFront->getListener( ) );
    
    self.products = response.products;
    
    std::vector< gameplay::StoreProduct > products;
    std::vector< std::string > invalidProducts;
    
    NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
    [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
    [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
    for (SKProduct * product in response.products)
    {
        [numberFormatter setLocale:product.priceLocale];
        [numberFormatter setCurrencySymbol:numberFormatter.currencyCode];
        NSString *formattedString = [numberFormatter stringFromNumber:product.price];

        // Determine product type based on identifier or other criteria
        gameplay::StoreProduct::ProductType productType = gameplay::StoreProduct::ProductType::NON_CONSUMABLE;
        if (@available(iOS 11.2, *)) {
            if (product.subscriptionPeriod != nil && product.subscriptionPeriod.numberOfUnits > 0) {
                productType = gameplay::StoreProduct::ProductType::AUTO_RENEWABLE_SUBSCRIPTION;
            }
        }

        // Create base product
        gameplay::StoreProduct storeProduct(
            [product.productIdentifier UTF8String],
            [product.localizedTitle UTF8String],
            [product.localizedDescription UTF8String],
            [product.price floatValue],
            [formattedString UTF8String],
            [[numberFormatter currencyCode] UTF8String],
            productType
        );
        
        // Populate subscription info if available
        if (@available(iOS 11.2, *)) {
            if (product.subscriptionPeriod != nil) {
                gameplay::StoreProduct::SubscriptionInfo subInfo;
                
                // Set subscription period
                NSInteger unitCount = product.subscriptionPeriod.numberOfUnits;
                switch (product.subscriptionPeriod.unit) {
                    case SKProductPeriodUnitDay:
                        subInfo.periodDays = (int)unitCount;
                        break;
                    case SKProductPeriodUnitWeek:
                        subInfo.periodDays = (int)unitCount * 7;
                        break;
                    case SKProductPeriodUnitMonth:
                        subInfo.periodDays = (int)unitCount * 30;
                        break;
                    case SKProductPeriodUnitYear:
                        subInfo.periodDays = (int)unitCount * 365;
                        break;
                }
                
                subInfo.periodCount = (int)unitCount;
                
                // Set subscription group if available
                if (product.subscriptionGroupIdentifier != nil) {
                    subInfo.groupId = [product.subscriptionGroupIdentifier UTF8String];
                }

                // Handle introductory price if available
                if (product.introductoryPrice != nil) {
                    subInfo.isIntroductory = true;
                    subInfo.introductoryPrice = [product.introductoryPrice.price floatValue];
                    
                    NSString *introPriceFormatted = [numberFormatter stringFromNumber:product.introductoryPrice.price];
                    subInfo.localizedIntroductoryPrice = [introPriceFormatted UTF8String];
                    
                    // Set introductory price period
                    NSInteger introUnitCount = product.introductoryPrice.subscriptionPeriod.numberOfUnits;
                    switch (product.introductoryPrice.subscriptionPeriod.unit) {
                        case SKProductPeriodUnitDay:
                            subInfo.introductoryPricePeriods = (int)introUnitCount;
                            break;
                        case SKProductPeriodUnitWeek:
                            subInfo.introductoryPricePeriods = (int)introUnitCount * 7;
                            break;
                        case SKProductPeriodUnitMonth:
                            subInfo.introductoryPricePeriods = (int)introUnitCount * 30;
                            break;
                        case SKProductPeriodUnitYear:
                            subInfo.introductoryPricePeriods = (int)introUnitCount * 365;
                            break;
                    }
                    
                    // Set payment mode
                    switch (product.introductoryPrice.paymentMode) {
                        case SKProductDiscountPaymentModePayAsYouGo:
                            subInfo.introductoryPricePaymentMode = 
                                gameplay::StoreProduct::SubscriptionInfo::IntroductoryPricePaymentMode::PAY_AS_YOU_GO;
                            break;
                        case SKProductDiscountPaymentModePayUpFront:
                            subInfo.introductoryPricePaymentMode = 
                                gameplay::StoreProduct::SubscriptionInfo::IntroductoryPricePaymentMode::PAY_UP_FRONT;
                            break;
                        case SKProductDiscountPaymentModeFreeTrial:
                            subInfo.introductoryPricePaymentMode = 
                                gameplay::StoreProduct::SubscriptionInfo::IntroductoryPricePaymentMode::FREE_TRIAL;
                            break;
                    }
                }
                
                storeProduct.subscriptionInfo = subInfo;
            }
        }
        
        products.push_back(storeProduct);        
    }
    [numberFormatter release];
    
    for (NSString *invalidProductId in response.invalidProductIdentifiers)
    {
        invalidProducts.push_back([invalidProductId UTF8String]);
    }
    
    _storeFront->getListener()->getProductsEvent(products, invalidProducts);
    
    if( !_observerAdded )
    {
        _observerAdded = true;
        [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    }
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
        _currentTransactionObject = transaction;
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
                _storeFront->getListener()->paymentTransactionSucceededEvent([transaction.payment.productIdentifier UTF8String], transaction.payment.quantity, [transaction.transactionDate timeIntervalSince1970], [transaction.transactionIdentifier UTF8String], transaction);
                break;
            case SKPaymentTransactionStateFailed:
                _storeFront->getListener()->paymentTransactionFailedEvent([transaction.payment.productIdentifier UTF8String], transaction.payment.quantity, transaction.error.code, [transaction.error.localizedDescription UTF8String]);
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                _storeFront->getListener()->paymentTransactionRestoredEvent([transaction.originalTransaction.payment.productIdentifier UTF8String], transaction.originalTransaction.payment.quantity, [transaction.originalTransaction.transactionDate timeIntervalSince1970], [transaction.originalTransaction.transactionIdentifier UTF8String], transaction);
                break;
            default:
                GP_ASSERT( transaction.transactionState == SKPaymentTransactionStatePurchasing );
                _storeFront->getListener()->paymentTransactionInProcessEvent([transaction.payment.productIdentifier UTF8String], transaction.payment.quantity);
                break;
        }
        _currentTransactionObject = nil;
    }
}

@end



@interface RefreshReceiptDelegate : NSObject <SKRequestDelegate>
{
    @public gameplay::AppleStoreFront * _storeFront;
}
@end

@implementation RefreshReceiptDelegate

- (void)requestDidFinish:(SKRequest *)request
{
    GP_ASSERT( _storeFront->getListener( ) );
    
    NSURL *receiptURL = [[NSBundle mainBundle] appStoreReceiptURL];
    NSData *receipt = [NSData dataWithContentsOfURL:receiptURL];

    _storeFront->getListener()->receiptRequested(receipt, 0, "");
    
    [request release];
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
    GP_ASSERT( _storeFront->getListener( ) );
    //NSLog( @"Refresh receipt failed %@", error );
    
    _storeFront->getListener()->receiptRequested(NULL, error.code, [error.localizedDescription UTF8String]);
    
    [request release];
}

@end


static AppleStoreKitController * __storeKitController = nil;
static RefreshReceiptDelegate * __refreshReceiptDelegate = nil;


namespace gameplay {


AppleStoreFront::AppleStoreFront()
    : _listener( NULL )
{
    if( __storeKitController == nil )
    {
        __storeKitController = [[AppleStoreKitController alloc] init];
        __storeKitController->_storeFront = this;
        __storeKitController->_observerAdded = false;
    }

    if (__refreshReceiptDelegate == nil)
    {
        __refreshReceiptDelegate = [[RefreshReceiptDelegate alloc] init];
        __refreshReceiptDelegate->_storeFront = this;
    }
}

AppleStoreFront::~AppleStoreFront()
{
    if( __storeKitController != nil )
    {
        if( __storeKitController->_observerAdded )
        {
            [[SKPaymentQueue defaultQueue] removeTransactionObserver:__storeKitController];
            __storeKitController->_observerAdded = false;
        }
        [__storeKitController release];
        __storeKitController = nil;
    }

    if (__refreshReceiptDelegate != nil)
    {
        [__refreshReceiptDelegate release];
        __refreshReceiptDelegate = nil;
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
    productsRequest.delegate = __storeKitController;
    [productsRequest start];
    
    [productsArray release];
}
    
void AppleStoreFront::makePayment(const char * productID, int quantity, const char * usernameHash)
{
    NSString * productIdentifier = [NSString stringWithUTF8String:productID];
    for (SKProduct * product in __storeKitController.products)
        if( [product.productIdentifier compare:productIdentifier] == NSOrderedSame )
        {
            SKMutablePayment * payment = [SKMutablePayment paymentWithProduct:product];
            payment.quantity = quantity;
#if (__IPHONE_OS_VERSION_MIN_REQUIRED >= 70000) || (__MAC_OS_X_VERSION_MIN_REQUIRED >= 1090)
            if( usernameHash )
                payment.applicationUsername = [NSString stringWithUTF8String:usernameHash];
#endif
            [[SKPaymentQueue defaultQueue] addPayment:payment];
            return;
        }
}

float AppleStoreFront::getShippingCost( const gameplay::StoreProduct& product, int quantity ) const
{
    return product.price * quantity * 0.3f;
}

void AppleStoreFront::restoreTransactions(const char * usernameHash)
{
    if( usernameHash )
    {
        if ([[SKPaymentQueue defaultQueue] respondsToSelector:@selector(restoreCompletedTransactionsWithApplicationUsername)])
        {
            [[SKPaymentQueue defaultQueue] restoreCompletedTransactionsWithApplicationUsername:[NSString stringWithUTF8String:usernameHash]];
            return;
        }
    }
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}
    
SKPaymentTransaction * AppleStoreFront::getCurrentTransactionObject()
{
    return __storeKitController ? __storeKitController->_currentTransactionObject : nil;
}

void AppleStoreFront::finishTransaction(void * transactionObject)
{
    [[SKPaymentQueue defaultQueue] finishTransaction:(SKPaymentTransaction *)(transactionObject)];
}

void AppleStoreFront::requestReceipt()
{
    SKReceiptRefreshRequest *refresh = [[SKReceiptRefreshRequest alloc] initWithReceiptProperties:nil];
    refresh.delegate = __refreshReceiptDelegate;
    [refresh start];
}
    
}

#endif
