#include "YieldCurveFuncs.h"

using namespace QuantLib;

Handle<YieldTermStructure> buildYieldCurve_ois(
    const Date& date,
    const Calendar& calendar,
    const DayCounter& fixedDayCountConvention,
    const BusinessDayConvention& fixedBusinessDayConvention,
    const ext::shared_ptr<OvernightIndex>& floatingLegIndex,
    const std::vector<Period>& tenors,
    const std::vector<Rate>& rates
) {
    if (tenors.size() != rates.size()) {
        throw std::invalid_argument("Tenors and rates must have the same size.");
    }

    // Create rate helpers from tenors and rates
    std::vector<ext::shared_ptr<RateHelper>> rateHelpers;
    for (size_t i = 0; i < tenors.size(); ++i) {
        auto quote = ext::make_shared<SimpleQuote>(rates[i]);
        rateHelpers.push_back(ext::make_shared<OISRateHelper>(
            2,                             // Settlement days
            tenors[i],                     // Swap maturity
            Handle<Quote>(quote),          // Fixed OIS rate
            floatingLegIndex               // Overnight index
        ));
    }

    // Construct the yield curve
    auto yieldCurve = ext::make_shared<PiecewiseYieldCurve<ZeroYield, Linear>>(
        date,                          // Settlement date
        rateHelpers,                   // Rate helpers
        fixedDayCountConvention        // Day count convention
    );

    return Handle<YieldTermStructure>(yieldCurve);
}

Handle<YieldTermStructure> buildYieldCurve_ibor(
    const Date& date,
    const Calendar& calendar,
    const Frequency& fixedLegFrequency,
    const DayCounter& fixedDayCountConvention,
    const BusinessDayConvention& fixedBusinessDayConvention,
    const ext::shared_ptr<IborIndex>& floatingLegIndex,
    const std::vector<Period>& tenors,
    const std::vector<Rate>& rates
) {
    if (tenors.size() != rates.size()) {
        throw std::invalid_argument("Tenors and rates must have the same size.");
    }

    // Create rate helpers from tenors and rates
    std::vector<ext::shared_ptr<RateHelper>> rateHelpers;
    for (size_t i = 0; i < tenors.size(); ++i) {
        auto quote = ext::make_shared<SimpleQuote>(rates[i]);
        rateHelpers.push_back(ext::make_shared<SwapRateHelper>(
            Handle<Quote>(quote),
            tenors[i],                            // Swap maturity
            calendar,                             // Calendar
            fixedLegFrequency,                    // Fixed leg frequency
            fixedBusinessDayConvention,           // Fixed leg convention
            fixedDayCountConvention,              // Fixed leg day count
            floatingLegIndex                      // Floating leg index
        ));
    }

    // Construct the yield curve
    auto yieldCurve = ext::make_shared<PiecewiseYieldCurve<ZeroYield, Linear>>(
        date,                          // Settlement date
        rateHelpers,                   // Rate helpers
        fixedDayCountConvention        // Day count convention
    );

    return Handle<YieldTermStructure>(yieldCurve);
}

TimeUnit mapCharToTimeUnit(char unit) {
    switch (std::toupper(unit)) {
    case 'Y': return Years;
    case 'M': return Months;
    case 'W': return Weeks;
    case 'D': return Days;
    default:
        throw std::invalid_argument("Invalid time unit character: " + std::string(1, unit));
    }
}

std::vector<Period> defineTenors(const std::vector<std::string>& vec)
{
    std::vector<Period> result;

    for (const auto& str : vec) {
        int number = 0;
        char unit = '\0';

        // Parse the string to extract numeric and letter parts
        std::string numPart, charPart;
        for (char ch : str) {
            if (std::isdigit(ch)) {
                numPart += ch;
            }
            else if (std::isalpha(ch)) {
                charPart += ch;
            }
        }

        // Ensure valid conversion for numeric part and letter extraction
        if (!numPart.empty()) {
            number = std::stoi(numPart);
        }
        if (!charPart.empty()) {
            unit = charPart[0]; // Assumes a single letter per string
        }

        TimeUnit timeUnit = mapCharToTimeUnit(unit);
        result.emplace_back(number, timeUnit);
    }

    return result;
}

std::vector<Rate> defineRates(const std::vector<float>& vec)
{
    std::vector<Rate> result(vec.begin(), vec.end());
    return result;
}

std::string formatQuantLibDate(const Date& date) {
    // Extract year, month, and day from the QuantLib::Date object
    int year = date.year();
    int month = date.month();
    int day = date.dayOfMonth();

    // Format as YYYY-MM-DD using a stringstream
    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << year << "-"
        << std::setw(2) << std::setfill('0') << month << "-"
        << std::setw(2) << std::setfill('0') << day;

    return oss.str();
}