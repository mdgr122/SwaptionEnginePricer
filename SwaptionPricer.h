#pragma once

#include <ql/quantlib.hpp>
#include <vector>
#include <string>

// Structure to hold parameters for building yield curves
struct YieldCurveParameters {
    // Common parameters
    QuantLib::Calendar calendar;
    QuantLib::DayCounter dayCounter;

    // For OIS curves
    QuantLib::Natural settlementDays;
    QuantLib::ext::shared_ptr<QuantLib::OvernightIndex> overnightIndex;

    // For Swap curves
    QuantLib::Frequency fixedFrequency;
    QuantLib::BusinessDayConvention fixedConvention;
    QuantLib::DayCounter fixedDayCount;
    QuantLib::ext::shared_ptr<QuantLib::IborIndex> iborIndex;
};

// Structure to hold market data for a currency
struct MarketData {
    QuantLib::ext::shared_ptr<QuantLib::IborIndex> iborIndex;
    QuantLib::ext::shared_ptr<QuantLib::OvernightIndex> overnightIndex;
    QuantLib::Handle<QuantLib::YieldTermStructure> forecastingCurve;
    QuantLib::Handle<QuantLib::YieldTermStructure> discountCurve;
    QuantLib::Currency currency;
};

// Function to build the OIS yield curve
QuantLib::Handle<QuantLib::YieldTermStructure> buildOISYieldCurve(
    const QuantLib::Date& today,
    const YieldCurveParameters& params,
    const std::vector<QuantLib::Period>& tenors,
    const std::vector<QuantLib::Rate>& rates);

// Function to build the Swap yield curve
QuantLib::Handle<QuantLib::YieldTermStructure> buildSwapYieldCurve(
    const QuantLib::Date& today,
    const YieldCurveParameters& params,
    const std::vector<QuantLib::Period>& tenors,
    const std::vector<QuantLib::Rate>& rates);

// Function to create and price a swap
QuantLib::ext::shared_ptr<QuantLib::VanillaSwap> createSwap(
    const QuantLib::Date& startDate,
    const QuantLib::Date& endDate,
    QuantLib::Real notional,
    const QuantLib::ext::shared_ptr<QuantLib::IborIndex>& iborIndex,
    const QuantLib::Handle<QuantLib::YieldTermStructure>& discountCurve,
    const QuantLib::Rate& fixedRate = 0.0);

// Function to create and price a swaption using Bachelier's model
QuantLib::Real priceSwaptionBachelier(
    const QuantLib::ext::shared_ptr<QuantLib::VanillaSwap>& swap,
    const QuantLib::Date& exerciseDate,
    const QuantLib::Volatility& volatility,
    const QuantLib::Handle<QuantLib::YieldTermStructure>& discountCurve);

// Utility function to parse tenors
std::vector<QuantLib::Period> parseTenors(const std::vector<std::string>& tenors);

// Utility function to parse rates
std::vector<QuantLib::Rate> parseRates(const std::vector<double>& rates);