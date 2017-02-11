#include "Base.h"
#include "GoogleStoreFront.h"
#include "storefront/StoreListener.h"
#include <jni.h>
#include <android_native_app_glue.h>



extern struct android_app* __state;
extern jclass __mainActivityClass;


namespace gameplay
{
    class GoogleStoreFront;
}

static gameplay::GoogleStoreFront * __instance = NULL;
static std::vector< gameplay::StoreProduct > __products;
static std::set<std::string> __requestedProducts;
static bool __iabEnabled = false;
static jmethodID __midQueueSKURequest;
static jmethodID __midFlushSkuDetailsQueue;
static jmethodID __midPurchaseItem;
static jmethodID __midRestorePurchases;

extern "C"
{

JNIEXPORT jint JNICALL GameplayJNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv* env = NULL;
    vm->GetEnv((void **)&env, JNI_VERSION_1_6);

    __mainActivityClass = (jclass)env->NewGlobalRef(env->FindClass("org/gameplay3d/GamePlayNativeActivity"));
    __midQueueSKURequest = env->GetMethodID(__mainActivityClass, "queueSkuDetailsRequest", "(Ljava/lang/String;)V");
    __midFlushSkuDetailsQueue = env->GetMethodID(__mainActivityClass, "flushSkuDetailsQueue", "()V");
    __midPurchaseItem = env->GetMethodID(__mainActivityClass, "purchaseItem", "(Ljava/lang/String;)V");
    __midRestorePurchases = env->GetMethodID(__mainActivityClass, "restorePurchases", "()V");

    return JNI_VERSION_1_6;
}

// JNI stuff
void Java_org_gameplay3d_GamePlayNativeActivity_setIABEnabled(JNIEnv* env, jobject thiz)
{
    __iabEnabled = true;
}

int Java_org_gameplay3d_GamePlayNativeActivity_isItemConsumable(JNIEnv* env, jobject thiz, jstring sku)
{
    const char* productID = env->GetStringUTFChars(sku, NULL);
    bool res = __instance->getListener()->isProductConsumable(productID);
    env->ReleaseStringUTFChars(sku, productID);

    return res ? 1 : 0;
}

void Java_org_gameplay3d_GamePlayNativeActivity_itemRestored(JNIEnv* env, jobject thiz, jstring sku, jlong time, jstring orderId)
{
    const char* productID = env->GetStringUTFChars(sku, NULL);
    const char* orderID = env->GetStringUTFChars(orderId, NULL);

    __instance->getListener()->paymentTransactionRestoredEvent(productID, 1, time, orderID, NULL);

    env->ReleaseStringUTFChars(orderId, orderID);
    env->ReleaseStringUTFChars(sku, productID);
}

void Java_org_gameplay3d_GamePlayNativeActivity_itemPurchased(JNIEnv* env, jobject thiz, jstring sku, jlong time, jstring orderId)
{
    const char* productID = env->GetStringUTFChars(sku, NULL);
    const char* orderID = env->GetStringUTFChars(orderId, NULL);

    __instance->getListener()->paymentTransactionSucceededEvent(productID, 1, time, orderID, NULL);

    env->ReleaseStringUTFChars(orderId, orderID);
    env->ReleaseStringUTFChars(sku, productID);
}

void Java_org_gameplay3d_GamePlayNativeActivity_itemPurchaseFailed(JNIEnv* env, jobject thiz, jstring sku, jint error, jstring message)
{
    const char* productID = env->GetStringUTFChars(sku, NULL);
    const char* msg = env->GetStringUTFChars(message, NULL);

    __instance->getListener()->paymentTransactionFailedEvent(productID, 1, error, msg);

    env->ReleaseStringUTFChars(message, msg);
    env->ReleaseStringUTFChars(sku, productID);
}

void Java_org_gameplay3d_GamePlayNativeActivity_getProductsFailed(JNIEnv* env, jobject thiz, jint error, jstring message)
{
    const char* msg = env->GetStringUTFChars(message, NULL);
    __instance->getListener()->getProductsFailedEvent(error, msg);
    env->ReleaseStringUTFChars(message, msg);
}

void Java_org_gameplay3d_GamePlayNativeActivity_productValidated(JNIEnv* env, jobject thiz, jstring sku, jstring price, jstring title, jstring descr, jstring priceAmount, jstring priceCurrency)
{
    const char* productID = env->GetStringUTFChars(sku, NULL);
    const char* cPrice = env->GetStringUTFChars(price, NULL);
    const char* cTitle = env->GetStringUTFChars(title, NULL);
    const char* cDescr = env->GetStringUTFChars(descr, NULL);
    const char* cPriceAmount = env->GetStringUTFChars(priceAmount, NULL);
    const char* cPriceCurrency = env->GetStringUTFChars(priceCurrency, NULL);

    gameplay::StoreProduct product(productID, cTitle, cDescr, static_cast<float>(atoi(cPriceAmount)) * 0.000001f, cPrice, cPriceCurrency);
    __products.push_back(product);

    env->ReleaseStringUTFChars(sku, productID);
    env->ReleaseStringUTFChars(price, cPrice);
    env->ReleaseStringUTFChars(title, cTitle);
    env->ReleaseStringUTFChars(descr, cDescr);
    env->ReleaseStringUTFChars(priceAmount, cPriceAmount);
    env->ReleaseStringUTFChars(priceCurrency, cPriceCurrency);
}

void Java_org_gameplay3d_GamePlayNativeActivity_finishProductsValidation(JNIEnv* env, jobject thiz)
{
    std::vector<std::string> invalidProducts;
    for (const std::string &product : __requestedProducts)
    {
        if (std::find_if(__products.begin(), __products.end(), [&](const gameplay::StoreProduct& p){return p.id == product; }) == __products.end())
            invalidProducts.push_back(product);
    }

    __instance->getListener()->getProductsEvent(__products, invalidProducts);
}

}




namespace gameplay
{



GoogleStoreFront::GoogleStoreFront()
    : _listener( NULL )
{
    __instance = this;
}

GoogleStoreFront::~GoogleStoreFront()
{
}

void GoogleStoreFront::setListener(StoreListener * listener)
{
    _listener = listener;
}

StoreListener * GoogleStoreFront::getListener()
{
    return _listener;
}

void GoogleStoreFront::getProducts(const char ** productIDs) const
{
    __products.clear();
    __requestedProducts.clear();

    android_app* app = __state;
    JNIEnv* env = app->activity->env;
    JavaVM* vm = app->activity->vm;
    vm->AttachCurrentThread(&env, NULL);

    while (*productIDs)
    {
        __requestedProducts.insert(*productIDs);

        jstring paramString = env->NewStringUTF(*productIDs++);
        env->CallVoidMethod(app->activity->clazz, __midQueueSKURequest, paramString);
    }

    env->CallVoidMethod(app->activity->clazz, __midFlushSkuDetailsQueue);

    vm->DetachCurrentThread();
}

void GoogleStoreFront::makePayment(const char * productID, int quantity, const char * usernameHash)
{
    android_app* app = __state;
    JNIEnv* env = app->activity->env;
    JavaVM* vm = app->activity->vm;
    vm->AttachCurrentThread(&env, NULL);

    jstring paramString = env->NewStringUTF(productID);

    while (quantity-- > 0)
        env->CallVoidMethod(app->activity->clazz, __midPurchaseItem, paramString);

    vm->DetachCurrentThread();

    getListener()->paymentTransactionInProcessEvent(productID, quantity);
}

bool GoogleStoreFront::canMakePayments() const
{
    return __iabEnabled;
}

void GoogleStoreFront::restoreTransactions(const char * usernameHash)
{
    android_app* app = __state;
    JNIEnv* env = app->activity->env;
    JavaVM* vm = app->activity->vm;
    vm->AttachCurrentThread(&env, NULL);

    env->CallVoidMethod(app->activity->clazz, __midRestorePurchases);

    vm->DetachCurrentThread();
}

float GoogleStoreFront::getShippingCost(const StoreProduct& product, int quantity) const
{
    return product.price * quantity * 0.3f;
}

void GoogleStoreFront::finishTransaction(void * transactionObject)
{
    // do nothing
}

void GoogleStoreFront::requestReceipt()
{
    GP_ASSERT(!"Not implemented.")
}

}