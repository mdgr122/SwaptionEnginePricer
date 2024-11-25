#include "SwaptionPricer.h"

using namespace QuantLib;

QuantLib::Handle<QuantLib::YieldTermStructure> buildOISYieldCurve(
    const Date& today,
    const YieldCurveParameters& params,
    const std::vector<Period>& tenors,
    const std::vector<Rate>& rates)
{
    // Create OIS rate helpers
    std::vector<ext::shared_ptr<RateHelper>> rateHelpers;
    for (size_t i = 0; i < tenors.size(); ++i) {
        ext::shared_ptr<Quote> rateQuote = ext::make_shared<SimpleQuote>(rates[i]);
        auto oisHelper = ext::make_shared<OISRateHelper>(
            params.settlementDays,
            tenors[i],
            Handle<Quote>(rateQuote),
            params.overnightIndex);
        rateHelpers.push_back(oisHelper);
    }

    // Build the yield curve
    auto yieldCurve = ext::make_shared<PiecewiseYieldCurve<ZeroYield, Linear>>(
        today, rateHelpers, params.dayCounter);

    return Handle<YieldTermStructure>(yieldCurve);
}

QuantLib::Handle<QuantLib::YieldTermStructure> buildSwapYieldCurve(
    const Date& today,
    const YieldCurveParameters& params,
    const std::vector<Period>& tenors,
    const std::vector<Rate>& rates)
{
    // Create Swap rate helpers
    std::vector<ext::shared_ptr<RateHelper>> rateHelpers;
    for (size_t i = 0; i < tenors.size(); ++i) {
        ext::shared_ptr<Quote> rateQuote = ext::make_shared<SimpleQuote>(rates[i]);
        auto swapHelper = ext::make_shared<SwapRateHelper>(
            Handle<Quote>(rateQuote),
            tenors[i],
            params.calendar,
            params.fixedFrequency,
            params.fixedConvention,
            params.fixedDayCount,
            params.iborIndex);
        rateHelpers.push_back(swapHelper);
    }

    // Build the yield curve
    auto yieldCurve = ext::make_shared<PiecewiseYieldCurve<ZeroYield, Linear>>(
        today, rateHelpers, params.dayCounter);

    return Handle<YieldTermStructure>(yieldCurve);
}

QuantLib::ext::shared_ptr<QuantLib::VanillaSwap> createSwap(
    const Date& startDate,
    const Date& endDate,
    Real notional,
    const ext::shared_ptr<IborIndex>& iborIndex,
    const Handle<YieldTermStructure>& discountCurve,
    const Rate& fixedRate)
{
    Schedule fixedSchedule(
        startDate, endDate, Period(Annual), iborIndex->fixingCalendar(),
        ModifiedFollowing, ModifiedFollowing, DateGeneration::Forward, false);

    Schedule floatSchedule(
        startDate, endDate, iborIndex->tenor(), iborIndex->fixingCalendar(),
        ModifiedFollowing, ModifiedFollowing, DateGeneration::Forward, false);

    // If fixedRate is 0.0, we'll compute the fair rate
    Rate swapFixedRate = fixedRate;

    if (fixedRate == 0.0) {
        // Build a temporary swap to calculate the fair rate
        VanillaSwap tempSwap(
            VanillaSwap::Payer,
            notional,
            fixedSchedule, 0.0, iborIndex->dayCounter(),
            floatSchedule, iborIndex, 0.0, iborIndex->dayCounter());

        auto discountEngine = ext::make_shared<DiscountingSwapEngine>(discountCurve);
        tempSwap.setPricingEngine(discountEngine);
        swapFixedRate = tempSwap.fairRate();
    }

    // Create the actual swap
    auto swap = ext::make_shared<VanillaSwap>(
        VanillaSwap::Payer,
        notional,
        fixedSchedule, swapFixedRate, iborIndex->dayCounter(),
        floatSchedule, iborIndex, 0.0, iborIndex->dayCounter());

    auto discountEngine = ext::make_shared<DiscountingSwapEngine>(discountCurve);
    swap->setPricingEngine(discountEngine);

    return swap;
}

QuantLib::Real priceSwaptionBachelier(
    const ext::shared_ptr<VanillaSwap>& swap,
    const Date& exerciseDate,
    const Volatility& volatility,
    const Handle<YieldTermStructure>& discountCurve)
{
    auto calendar = TARGET(); // You might want to make this configurable
    auto europeanExercise = ext::make_shared<EuropeanExercise>(exerciseDate);

    Swaption swaption(swap, europeanExercise);

    Handle<Quote> volHandle(ext::make_shared<SimpleQuote>(volatility));
    auto volStructure = Handle<SwaptionVolatilityStructure>(
        ext::make_shared<ConstantSwaptionVolatility>(
            discountCurve->referenceDate(), calendar, ModifiedFollowing,
            volHandle->value(), Actual365Fixed(), Normal));

    auto swaptionEngine = ext::make_shared<BachelierSwaptionEngine>(discountCurve, volStructure);
    swaption.setPricingEngine(swaptionEngine);

    return swaption.NPV();
}

std::vector<QuantLib::Period> parseTenors(const std::vector<std::string>& tenors) {
    std::vector<Period> result;
    for (const auto& tenorStr : tenors) {
        result.push_back(PeriodParser::parse(tenorStr));
    }
    return result;
}

std::vector<QuantLib::Rate> parseRates(const std::vector<double>& rates) {
    return std::vector<Rate>(rates.begin(), rates.end());
}