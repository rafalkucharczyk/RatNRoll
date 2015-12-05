#include "InAppPurchase.h"
#import <StoreKit/StoreKit.h>

using namespace std;

@interface InAppPurchaseWrapper
    : NSObject <SKRequestDelegate, SKProductsRequestDelegate, SKPaymentTransactionObserver> {
    InAppPurchase *owner;
}

@property(strong, nonatomic) SKProduct *product;

@end

static InAppPurchaseWrapper *wrapper = nil;

@implementation InAppPurchaseWrapper

@synthesize product;

+ (id)sharedInstance
{
    if (wrapper == nil)
        wrapper = [[InAppPurchaseWrapper alloc] init];

    return wrapper;
}

- (void)assignOwner:(InAppPurchase *)owner_ { owner = owner_; }

- (void)releaseProduct
{
    if (product) {
        [product release];
        product = nil;
    }
}

- (void)sendNotification:(InAppPurchase::NotificationType)notification
             withMessage:(std::string)message
     andSecondaryMessage:(std::string)secondaryMessage
{
    if (owner) {
        owner->sendNotification(notification, message, secondaryMessage);
    }
}

- (void)sendNotification:(InAppPurchase::NotificationType)notification
             withMessage:(std::string)message
{
    [self sendNotification:notification withMessage:message andSecondaryMessage:""];
}

- (void)sendNotification:(InAppPurchase::NotificationType)notification
{
    [self sendNotification:notification withMessage:"" andSecondaryMessage:""];
}

- (void)requestProduct:(NSString *)productID
{
    if ([SKPaymentQueue canMakePayments]) {
        NSLog(@"User can make payments");
        SKProductsRequest *productsRequest =
            [[SKProductsRequest alloc] initWithProductIdentifiers:[NSSet setWithObject:productID]];
        productsRequest.delegate = self;
        [productsRequest start];
    } else {
        NSLog(@"User cannot make payments.");
        [self sendNotification:InAppPurchase::error withMessage:"User cannot make payments."];
    }
}

- (void)productsRequest:(SKProductsRequest *)request
     didReceiveResponse:(SKProductsResponse *)response
{
    product = nil;
    NSUInteger count = [response.products count];

    if (count > 0) {
        product = [response.products objectAtIndex:0];
        [product retain];
        [self sendNotification:InAppPurchase::productReceived];
    } else if (!product) {
        [self sendNotification:InAppPurchase::error withMessage:"Product not valid."];
    }
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
    // wknd - let's notify higher layer the product is available to purchase and
    // handle errors during actual purchase
    [self sendNotification:InAppPurchase::productReceived];
}

- (void)purchase
{
    if (product) {
        SKPayment *payment = [SKPayment paymentWithProduct:product];
        [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
        [[SKPaymentQueue defaultQueue] addPayment:payment];
    } else {
        [self sendNotification:InAppPurchase::error withMessage:"Product not available."];
    }
}

- (void)restore
{
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue
{
    NSLog(@"Received restored transactions: %lu", (unsigned long)queue.transactions.count);

    for (SKPaymentTransaction *transaction in queue.transactions) {
        if ([transaction transactionState] == SKPaymentTransactionStateRestored) {
            NSLog(@"Transaction state -> Restored");
            NSLog(@"%@", [[transaction payment] productIdentifier]);
            [self sendNotification:InAppPurchase::productRestored];
            [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
        }

        if ([transaction transactionState] == SKPaymentTransactionStateFailed) {
            NSLog(@"Transaction state -> Error");
            [self sendNotification:InAppPurchase::error];
            [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
        }
    }
}

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction *transaction in transactions) {
        int transactionState = transaction.transactionState;

        switch (transactionState) {
        case SKPaymentTransactionStatePurchasing:
            NSLog(@"Transaction state -> Purchasing");
            [self sendNotification:InAppPurchase::purchasingProduct];
            break;

        case SKPaymentTransactionStatePurchased:
            NSLog(@"Transaction state -> Purchased");
            [self sendNotification:InAppPurchase::productPurchased];
            [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
            break;

        case SKPaymentTransactionStateFailed: {
            string errorMessage = "Purchase Failed.";
            string secondaryMessage = "";

            switch (transaction.error.code) {
            case SKErrorUnknown:
                [self sendNotification:InAppPurchase::error
                            withMessage:errorMessage
                    andSecondaryMessage:secondaryMessage];
                break;

            case SKErrorClientInvalid:
                secondaryMessage = "Client is invalid";
                [self sendNotification:InAppPurchase::error
                            withMessage:errorMessage
                    andSecondaryMessage:secondaryMessage];
                break;

            case SKErrorPaymentCancelled:
                [self sendNotification:InAppPurchase::productPurchaseCancelled
                            withMessage:errorMessage
                    andSecondaryMessage:secondaryMessage];
                break;

            case SKErrorPaymentInvalid:
                secondaryMessage = "Payment is invalid.";
                [self sendNotification:InAppPurchase::error
                            withMessage:errorMessage
                    andSecondaryMessage:secondaryMessage];
                break;

            case SKErrorPaymentNotAllowed:
                secondaryMessage = "Payment is not allowed.";
                [self sendNotification:InAppPurchase::error
                            withMessage:errorMessage
                    andSecondaryMessage:secondaryMessage];
                break;

            case SKErrorStoreProductNotAvailable:
                secondaryMessage = "Product is not available.";

                [self sendNotification:InAppPurchase::error
                            withMessage:errorMessage
                    andSecondaryMessage:secondaryMessage];
                break;

            default:
                break;
            }

            [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
            break;
        }
        }
    }
}

- (NSString *)getLocalizedProductDescription
{
    NSString *desc = @"";

    if (product)
        desc = product.localizedDescription;

    return desc;
}

- (NSString *)getLocalizedProductTitle
{
    NSString *title = @"";

    if (product)
        title = product.localizedTitle;

    return title;
}

- (NSString *)getProductPriceInLocalCurrency
{
    NSString *price = @"";

    if (product) {
        NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
        [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
        [numberFormatter setLocale:product.priceLocale];
        price = [numberFormatter stringFromNumber:product.price];
    }

    return price;
}

@end

InAppPurchase::InAppPurchase() { [[InAppPurchaseWrapper sharedInstance] assignOwner:this]; }

InAppPurchase::~InAppPurchase()
{
    [[InAppPurchaseWrapper sharedInstance] assignOwner:nil];
    [[InAppPurchaseWrapper sharedInstance] releaseProduct];
}

void InAppPurchase::requestProduct(std::string productID)
{
    NSString *nsProductID =
        [NSString stringWithCString:productID.c_str() encoding:[NSString defaultCStringEncoding]];

    [[InAppPurchaseWrapper sharedInstance] requestProduct:nsProductID];
}

void InAppPurchase::purchaseProduct() { [[InAppPurchaseWrapper sharedInstance] purchase]; }

void InAppPurchase::restoreProduct() { [[InAppPurchaseWrapper sharedInstance] restore]; }

string InAppPurchase::getLocalizedProductDescription()
{
    CFStringRef cfDesc =
        (CFStringRef)[[InAppPurchaseWrapper sharedInstance] getLocalizedProductDescription];
    return string([(NSString *)cfDesc UTF8String]);
}

string InAppPurchase::getLocalizedProductTitle()
{
    CFStringRef cfTitle =
        (CFStringRef)[[InAppPurchaseWrapper sharedInstance] getLocalizedProductTitle];
    return string([(NSString *)cfTitle UTF8String]);
}

string InAppPurchase::getProductPriceInLocalCurrency()
{
    CFStringRef cfPrice =
        (CFStringRef)[[InAppPurchaseWrapper sharedInstance] getProductPriceInLocalCurrency];
    return string([(NSString *)cfPrice UTF8String]);
}

void InAppPurchase::sendNotification(int notification, string message, string secondaryMessage)
{
    if (listener)
        listener->inAppPurchaseNotificationReceived(notification, message, secondaryMessage);
}