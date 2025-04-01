#pragma once

#include <optional>


namespace gameplay
{

/**
 * Defines a base product.
 *
 * This can either be the consumable, non-consumable or renewable product.
 *
 * @script{ignore}
 */
struct StoreProduct
{
    /**
     * The string that identifies the product to the Store provider.
     */
    std::string id;

    /**
     * The localized UTF8 name of the product.
     */
    std::string name;

    /**
     * The localized UTF8 description of the product.
     */
    std::string description;

    /**
     * The product's price in local currency.
     */
    float price;

    /**
     * The product's localized price string in local currency (UTF-8).
     */
    std::string localizedPrice;
    
    /**
     * The product's currency code ISO 4217.
     */
    std::string currencyCode;

    /**
     * Product type (consumable, non-consumable, subscription, etc.)
     */
    enum class ProductType {
        CONSUMABLE,
        NON_CONSUMABLE,
        SUBSCRIPTION,
        AUTO_RENEWABLE_SUBSCRIPTION
    } type;

    // Subscription-specific properties (only valid when type is SUBSCRIPTION or AUTO_RENEWABLE_SUBSCRIPTION)
    struct SubscriptionInfo {
        /**
         * Subscription period in days (30 for monthly, 365 for yearly, etc.)
         */
        int periodDays;

        /**
         * Number of periods in a subscription group (for multi-period subscriptions)
         */
        int periodCount;

        /**
         * Subscription group identifier (for grouping different subscription tiers)
         */
        std::string groupId;

        /**
         * Whether this subscription is introductory price
         */
        bool isIntroductory;

        /**
         * Introductory price (if available)
         */
        float introductoryPrice;

        /**
         * Localized introductory price string
         */
        std::string localizedIntroductoryPrice;

        /**
         * Number of periods for introductory price
         */
        int introductoryPricePeriods;

        /**
         * Payment mode for introductory price
         */
        enum class IntroductoryPricePaymentMode {
            PAY_AS_YOU_GO,
            PAY_UP_FRONT,
            FREE_TRIAL
        } introductoryPricePaymentMode;
    };

    /**
     * Optional subscription info (only populated for subscription products)
     */
    std::optional<SubscriptionInfo> subscriptionInfo;

    /**
     * Constructor for non-subscription products
     */
    StoreProduct(const char* _id, const char* _name, const char* _description,
        float _price, const char* _localizedPrice, const char* _currencyCode,
        ProductType _type = ProductType::NON_CONSUMABLE);

    /**
     * Constructor for subscription products
     */
    StoreProduct(const char* _id, const char* _name, const char* _description,
        float _price, const char* _localizedPrice, const char* _currencyCode,
        SubscriptionInfo _subscriptionInfo);

    /**
     * Destructor
     */
    ~StoreProduct();
};

}

