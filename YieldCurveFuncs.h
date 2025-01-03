#pragma once
#include <ql/quantlib.hpp>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>
#include <cctype>

// Function declarations for building yield curves
QuantLib::Handle<QuantLib::YieldTermStructure> buildYieldCurve_ois(
    const QuantLib::Date& date,
    const QuantLib::Calendar& calendar,
    const QuantLib::DayCounter& fixedDayCountConvention,
    const QuantLib::BusinessDayConvention& fixedBusinessDayConvention,
    const QuantLib::ext::shared_ptr<QuantLib::OvernightIndex>& floatingLegIndex,
    const std::vector<QuantLib::Period>& tenors,
    const std::vector<QuantLib::Rate>& rates);

QuantLib::Handle<QuantLib::YieldTermStructure> buildYieldCurve_ibor(
    const QuantLib::Date& date,
    const QuantLib::Calendar& calendar,
    const QuantLib::Frequency& fixedLegFrequency,
    const QuantLib::DayCounter& fixedDayCountConvention,
    const QuantLib::BusinessDayConvention& fixedBusinessDayConvention,
    const QuantLib::ext::shared_ptr<QuantLib::IborIndex>& floatingLegIndex,
    const std::vector<QuantLib::Period>& tenors,
    const std::vector<QuantLib::Rate>& rates);

// Utility functions
std::vector<QuantLib::Period> defineTenors(const std::vector<std::string>& vec);
std::vector<QuantLib::Rate> defineRates(const std::vector<float>& vec);
QuantLib::TimeUnit mapCharToTimeUnit(char unit);
std::string formatQuantLibDate(const QuantLib::Date& date);