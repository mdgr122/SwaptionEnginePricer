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
        rateHelpers.push_back(ext::make_shared<OISRateHelper>(
            2,                             // Settlement days (ensure this is a Natural, e.g., 2u)
            tenors[i],                     // Swap maturity (Period)
            Handle<Quote>(ext::make_shared<SimpleQuote>(rates[i])), // Fixed OIS rate (Handle<Quote>)
            floatingLegIndex               // Overnight index (ext::shared_ptr<OvernightIndex>)
        ));
    }

    // Construct the yield curve
    ext::shared_ptr<YieldTermStructure> yieldCurve(
        new PiecewiseYieldCurve<ZeroYield, Linear>(
            date,                          // Settlement date
            rateHelpers,                   // Rate helpers
            fixedDayCountConvention        // Day count convention
        )
    );

    return Handle<YieldTermStructure>(yieldCurve);
}

// Function to build the yield curve
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
        rateHelpers.push_back(ext::make_shared<SwapRateHelper>
            (
                Handle<Quote>(ext::make_shared<SimpleQuote>(rates[i])),
                tenors[i],        // Swap maturity
                calendar,         // Calendar
                fixedLegFrequency,           // Fixed leg frequency
                fixedBusinessDayConvention,// Fixed leg convention
                fixedDayCountConvention,      // Fixed leg day count
                floatingLegIndex // Floating leg index
            )
        );
    }

    // Construct the yield curve
    ext::shared_ptr<YieldTermStructure> yieldCurve(
        new PiecewiseYieldCurve<ZeroYield, Linear>(
            date,            // Settlement date
            rateHelpers,      // Rate helpers
            fixedDayCountConvention        // Day count convention
        )
    );

    return Handle<YieldTermStructure>(yieldCurve);
}

// Function to map unit character to QuantLib::TimeUnit
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


std::vector<Period> defineTenors(std::vector<std::string> vec)
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

std::vector<QuantLib::Rate> defineRates(std::vector<float> vec)
{
    std::vector<QuantLib::Rate> result;

    for (const auto& elem : vec)
    {
        result.push_back(elem);
    }

    return result;
}


std::string formatQuantLibDate(const QuantLib::Date& date) {
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