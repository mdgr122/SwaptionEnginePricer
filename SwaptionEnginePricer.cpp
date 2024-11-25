#include <ql/quantlib.hpp>
#include <boost/config.hpp>
#include <iostream>
#include <vector>
#include <stdexcept>
#include "YieldCurveFuncs.h"

//using namespace QuantLib;


// Function to build the yield curve
//Handle<YieldTermStructure> buildYieldCurve_ibor(
//    const Date& date,
//    const Calendar& calendar,
//    const Frequency& fixedLegFrequency,
//    const Frequency& floatLegResetFrequency,
//    const DayCounter& fixedDayCountConvention,
//    const BusinessDayConvention& fixedBusinessDayConvention,
//    const ext::shared_ptr<IborIndex>& floatingLegIndex,
//    const std::vector<Period>& tenors,
//    const std::vector<Rate>& rates
//) {
//    if (tenors.size() != rates.size()) {
//        throw std::invalid_argument("Tenors and rates must have the same size.");
//    }
//
//    // Create rate helpers from tenors and rates
//    std::vector<ext::shared_ptr<RateHelper>> rateHelpers;
//    for (size_t i = 0; i < tenors.size(); ++i) {
//        rateHelpers.push_back(ext::make_shared<SwapRateHelper>
//            (
//                Handle<Quote>(ext::make_shared<SimpleQuote>(rates[i])),
//                tenors[i],        // Swap maturity
//                calendar,         // Calendar
//                fixedLegFrequency,           // Fixed leg frequency
//                fixedBusinessDayConvention,// Fixed leg convention
//                fixedDayCountConvention,      // Fixed leg day count
//                floatingLegIndex // Floating leg index
//            )
//        );
//    }
//
//    // Construct the yield curve
//    ext::shared_ptr<YieldTermStructure> yieldCurve(
//        new PiecewiseYieldCurve<ZeroYield, Linear>(
//            date,            // Settlement date
//            rateHelpers,      // Rate helpers
//            fixedDayCountConvention        // Day count convention
//        )
//    );
//
//    return Handle<YieldTermStructure>(yieldCurve);
//}


// Function to build the yield curve for OIS swaps
//Handle<YieldTermStructure> buildYieldCurve_ois(
//    const Date& date,
//    const Calendar& calendar,
//    const DayCounter& fixedDayCountConvention,
//    const BusinessDayConvention& fixedBusinessDayConvention,
//    const ext::shared_ptr<OvernightIndex>& floatingLegIndex,
//    const std::vector<Period>& tenors,
//    const std::vector<Rate>& rates
//) {
//    if (tenors.size() != rates.size()) {
//        throw std::invalid_argument("Tenors and rates must have the same size.");
//    }
//
//    // Create rate helpers from tenors and rates
//    std::vector<ext::shared_ptr<RateHelper>> rateHelpers;
//    for (size_t i = 0; i < tenors.size(); ++i) {
//        rateHelpers.push_back(ext::make_shared<OISRateHelper>(
//            2,                             // Settlement days (ensure this is a Natural, e.g., 2u)
//            tenors[i],                     // Swap maturity (Period)
//            Handle<Quote>(ext::make_shared<SimpleQuote>(rates[i])), // Fixed OIS rate (Handle<Quote>)
//            floatingLegIndex               // Overnight index (ext::shared_ptr<OvernightIndex>)
//        ));
//    }
//
//    // Construct the yield curve
//    ext::shared_ptr<YieldTermStructure> yieldCurve(
//        new PiecewiseYieldCurve<ZeroYield, Linear>(
//            date,                          // Settlement date
//            rateHelpers,                   // Rate helpers
//            fixedDayCountConvention        // Day count convention
//        )
//    );
//
//    return Handle<YieldTermStructure>(yieldCurve);
//}

// Function to define market instruments
//std::vector<Period> defineTenors() {
//    return {
//        Period(1, Years), Period(18, Months), Period(2, Years), Period(3, Years),
//        Period(4, Years), Period(5, Years), Period(6, Years), Period(7, Years),
//        Period(8, Years), Period(9, Years), Period(10, Years), Period(12, Years),
//        Period(15, Years), Period(20, Years), Period(25, Years), Period(30, Years)
//    };
//}
//
//// Function to define market rates
//std::vector<Rate> defineRates() {
//    return {
//        0.026, 0.02462, 0.0242, 0.023895, 0.023415, 0.023818, 0.023912,
//        0.024032, 0.024168, 0.0243, 0.02443, 0.02465, 0.02482, 0.02414,
//        0.02303, 0.02199
//    };
//}

//std::vector<Period> defineTenors()
//{
//    return {
//        Period(1, Weeks), Period(2, Weeks), Period(3, Weeks), Period(1, Months),
//        Period(2, Months), Period(3, Months), Period(4, Months), Period(5, Months),
//        Period(6, Months), Period(7, Months), Period(8, Months), Period(9, Months),
//        Period(10, Months), Period(11, Months), Period(1, Years), Period(15, Months),
//        Period(18, Months), Period(21, Months), Period(2, Years), Period(3, Years),
//        Period(4, Years), Period(5, Years), Period(6, Years), Period(7, Years),
//        Period(8, Years), Period(9, Years), Period(10, Years), Period(12, Years),
//        Period(15, Years), Period(20, Years), Period(25, Years), Period(30, Years),
//        Period(40, Years), Period(50, Years)
//    };
//}

//std::vector<Rate> defineRates()
//{
//    return {
//        0.03249, 0.0325, 0.03251, 0.03252, 0.031665, 0.031055, 0.030115, 0.02911, 0.028275, 0.02756, 
//        0.0268997, 0.02632, 0.02583, 0.02541, 0.02507, 0.024165, 0.02362, 0.023295, 0.023095, 0.022825, 
//        0.022775, 0.022825, 0.02301, 0.023225, 0.02347, 0.023715, 0.02395, 0.024145, 0.02436, 0.024765, 
//        0.024415, 0.023545, 0.02269, 0.02131, 0.01997
//    };
//}


using namespace QuantLib;
int main() {
    std::vector<std::string> input_tenors_eonia{ "1W","2W","3W","1M","2M","3M","4M","5M","6M","7M","8M","9M","10M","11M","1Y","15M","18M","21M","2Y","3Y","4Y","5Y","6Y","7Y","8Y","9Y","10Y","11Y","12Y","15Y","20Y","25Y","30Y","40Y","50Y" };
    std::vector<std::string> input_tenors_estr{ "1W","2W","3W","1M","2M","3M","4M","5M","6M","7M","8M","9M","10M","11M","1Y","15M","18M","21M","2Y","3Y","4Y","5Y","6Y","7Y","8Y","9Y","10Y","11Y","12Y","15Y","20Y","25Y","30Y","40Y","50Y" };
    std::vector<std::string> input_tenors_euribor_6m{ "1Y","18M","2Y","3Y","4Y","5Y","6Y","7Y","8Y","9Y","10Y","12Y","15Y","20Y","25Y","30Y","40Y","50Y","60Y","70Y" };

    std::vector<float> input_rates_eonia
    {
        0.03249, 0.0325, 0.03251, 0.03252, 0.031665, 0.031055, 0.030115, 0.02911, 0.028275, 0.02756,
        0.0268997, 0.02632, 0.02583, 0.02541, 0.02507, 0.024165, 0.02362, 0.023295, 0.023095, 0.022825,
        0.022775, 0.022825, 0.02301, 0.023225, 0.02347, 0.023715, 0.02395, 0.024145, 0.02436, 0.024765,
        0.024415, 0.023545, 0.02269, 0.02131, 0.01997
    };


    std::vector<float> input_rates_estr{ 0.03164,0.031655,0.031665,0.031675,0.030819,0.03019,0.02926,0.028255,0.0274045,0.026695,0.026043,0.025464,0.0249665,0.024548,0.024207,0.023299,0.0227545,0.022433,0.022237,0.0219645,0.021903,0.021952,0.0221345,0.022352,0.0225925,0.0228395,0.0230765,0.023277,0.0234875,0.0238965,0.023541,0.022661,0.0217935,0.0204395,0.0191035 };
    std::vector<float> input_rates_euribor_6m{ 0.026,0.024616,0.0242,0.023895,0.023815,0.023818,0.023912,0.024032,0.024168,0.024304,0.024425,0.024645,0.0248197,0.024142,0.023032,0.0219885,0.02024,0.01865,0.017464767,0.016728967 };

    std::vector<QuantLib::Period> output_tenors_eonia = defineTenors(input_tenors_eonia);
    std::vector<QuantLib::Rate> output_rates_eonia = defineRates(input_rates_eonia);

    std::vector<QuantLib::Period> output_tenors_estr = defineTenors(input_tenors_estr);
    std::vector<QuantLib::Rate> output_rates_estr = defineRates(input_rates_estr);

    std::vector<QuantLib::Period> output_tenors_euribor_6m = defineTenors(input_tenors_euribor_6m);
    std::vector<QuantLib::Rate> output_rates_euribor_6m = defineRates(input_rates_euribor_6m);

    // Environment Setup
    QuantLib::Calendar calendar = QuantLib::TARGET();
    QuantLib::Date date(31, QuantLib::October, 2024);
    QuantLib::Settings::instance().evaluationDate() = date;

    // OIS Instrument Setup
    QuantLib::Frequency fixedLegFreq = QuantLib::Semiannual;
    QuantLib::BusinessDayConvention businessDayConv = QuantLib::ModifiedFollowing;
    QuantLib::DayCounter dayCountConv = QuantLib::Actual365Fixed();
    QuantLib::ext::shared_ptr<QuantLib::OvernightIndex> floatLegIndex_eonia = QuantLib::ext::make_shared<QuantLib::Eonia>();
    QuantLib::ext::shared_ptr<QuantLib::OvernightIndex> floatLegIndex_estr = QuantLib::ext::make_shared<QuantLib::Estr>();
    QuantLib::Compounding compounding = QuantLib::Continuous;

    QuantLib::Handle<QuantLib::YieldTermStructure> eonia_curve = buildYieldCurve_ois(date, calendar, dayCountConv, businessDayConv, floatLegIndex_eonia, output_tenors_eonia, output_rates_eonia);
    QuantLib::Handle<QuantLib::YieldTermStructure> estr_curve = buildYieldCurve_ois(date, calendar, dayCountConv, businessDayConv, floatLegIndex_estr, output_tenors_estr, output_rates_estr);

    // LIBOR Instrument Setup
    QuantLib::Frequency fixedLegFreq_ibor = QuantLib::Annual;
    QuantLib::DayCounter dayCountConv_ibor = QuantLib::Thirty360(QuantLib::Thirty360::European);
    QuantLib::BusinessDayConvention businessDayConv_ibor = QuantLib::ModifiedFollowing;
    QuantLib::ext::shared_ptr<QuantLib::IborIndex> floatLegIndex_euribor_6m = QuantLib::ext::make_shared<QuantLib::Euribor6M>();
    QuantLib::Compounding compounding_ibor = QuantLib::Continuous;

    QuantLib::Handle<QuantLib::YieldTermStructure> euribor_6m_curve = buildYieldCurve_ibor(date, calendar, fixedLegFreq_ibor, dayCountConv_ibor, businessDayConv_ibor, floatLegIndex_euribor_6m, output_tenors_euribor_6m, output_rates_euribor_6m);



    // Print OIS curve
    std::cout << "\nOIS Yield Curve (EONIA):\n";
    std::cout << "SettlementDate,MaturityDate,Tenor,Swap_Rate,Zero_Rate,DF" << std::endl;


    for (size_t i = 0; i < output_tenors_eonia.size(); i++)
    {
        QuantLib::Date maturityDate = calendar.advance(date, output_tenors_eonia[i]);
        QuantLib::Rate eonia_zero_rate = eonia_curve->zeroRate(maturityDate, dayCountConv, compounding);
        QuantLib::DiscountFactor eonia_df = eonia_curve->discount(maturityDate);
        std::cout << formatQuantLibDate(date) << "," << formatQuantLibDate(maturityDate) << "," << output_tenors_eonia[i] << "," << output_rates_eonia[i] << "," << eonia_zero_rate << "," << eonia_df << '\n';
    }
    std::cout << "\n\n";

    // Print OIS curve
    std::cout << "\nOIS Yield Curve (ESTR):\n";
    std::cout << "SettlementDate,MaturityDate,Tenor,Swap_Rate,Zero_Rate,DF" << std::endl;


    for (size_t i = 0; i < output_tenors_estr.size(); i++)
    {
        QuantLib::Date maturityDate = calendar.advance(date, output_tenors_estr[i]);
        QuantLib::Rate estr_zero_rate = estr_curve->zeroRate(maturityDate, dayCountConv, compounding);
        QuantLib::DiscountFactor estr_df = estr_curve->discount(maturityDate);
        std::cout << formatQuantLibDate(date) << "," << formatQuantLibDate(maturityDate) << "," << output_tenors_estr[i] << "," << output_rates_estr[i] << "," << estr_zero_rate << "," << estr_df << '\n';
    }


    std::cout << "\n\n";

    // Print OIS curve
    std::cout << "\n Yield Curve (euribor_6m):\n";
    std::cout << "SettlementDate,MaturityDate,Tenor,Swap_Rate,Zero_Rate,DF,Forward" << std::endl;


    for (size_t i = 0; i < output_tenors_euribor_6m.size(); i++)
    {
        QuantLib::Date maturityDate = calendar.advance(date, output_tenors_euribor_6m[i]);
        QuantLib::Rate euribor_6m_zero_rate = euribor_6m_curve->zeroRate(maturityDate, dayCountConv, compounding);
        QuantLib::DiscountFactor euribor_6m_df = euribor_6m_curve->discount(maturityDate);
        std::cout << formatQuantLibDate(date) << "," << formatQuantLibDate(maturityDate) << "," << output_tenors_euribor_6m[i] << "," << output_rates_euribor_6m[i] << "," << euribor_6m_zero_rate << "," << euribor_6m_df << '\n';

    }



    // Swaption details
    QuantLib::Period exercisePeriod = 10 * QuantLib::Years; // 1 month to exercise
    QuantLib::Period swapTenor = 10 * QuantLib::Years;      // 10-year swap starting at exercise date

    // Forward start date (1 month from settlement date)
    QuantLib::Date startDate = calendar.advance(date, exercisePeriod);


    // Swap end date (10 years from the forward start date)
    QuantLib::Date endDate = calendar.advance(startDate, swapTenor);

    // Create floating index (e.g., EURIBOR 6M) linked to euribor_6m_curve
    QuantLib::ext::shared_ptr<QuantLib::IborIndex> euribor6m = QuantLib::ext::make_shared<QuantLib::Euribor6M>(euribor_6m_curve);

    //// Build the fixed and floating leg schedules
    //QuantLib::Schedule fixedSchedule(
    //    startDate, endDate, QuantLib::Period(QuantLib::Annual), calendar,
    //    QuantLib::ModifiedFollowing, QuantLib::ModifiedFollowing,
    //    QuantLib::DateGeneration::Forward, false);

    //QuantLib::Schedule floatSchedule(
    //    startDate, endDate, euribor6m->tenor(), calendar,
    //    QuantLib::ModifiedFollowing, QuantLib::ModifiedFollowing,
    //    QuantLib::DateGeneration::Forward, false);

    //// Create the vanilla swap
    //QuantLib::VanillaSwap swap(
    //    QuantLib::VanillaSwap::Payer,             // Swap type: Payer or Receiver
    //    1e6,                            // Nominal (EUR 1,000,000)
    //    fixedSchedule,                  // Fixed leg schedule
    //    0.0,                            // Fixed rate (initially 0 to calculate par rate)
    //    QuantLib::Thirty360(QuantLib::Thirty360::European),                    // Fixed leg day count convention
    //    floatSchedule,                  // Floating leg schedule
    //    euribor6m,                      // Floating index
    //    0.0,                            // Spread on the floating leg
    //    QuantLib::Actual360());                   // Floating leg day count convention

    //// Set the pricing engine with the euribor_6m_curve for discounting
    //QuantLib::ext::shared_ptr<QuantLib::PricingEngine> swapEngine = QuantLib::ext::make_shared<QuantLib::DiscountingSwapEngine>(eonia_curve);
    //swap.setPricingEngine(swapEngine);

    //// Calculate the par swap rate
    //QuantLib::Rate parSwapRate = swap.fairRate();

    //// Output the par swap rate
    //std::cout << "Settlement Date: " << date << std::endl;
    //std::cout << "Start Date: " << startDate << std::endl;
    //std::cout << "End Date: " << endDate << std::endl;
    //std::cout << "Par Swap Rate: " << parSwapRate << std::endl;

    // Build fixed and floating leg schedules
    Schedule fixedSchedule(
        startDate, endDate, Period(Annual), calendar,
        ModifiedFollowing, ModifiedFollowing,
        DateGeneration::Forward, false);

    Schedule floatSchedule(
        startDate, endDate, euribor6m->tenor(), calendar,
        ModifiedFollowing, ModifiedFollowing,
        DateGeneration::Forward, false);

    // Create the underlying swap (VanillaSwap)
    VanillaSwap forwardSwap(
        VanillaSwap::Payer,             // Swap type: Payer or Receiver
        1e6,                            // Nominal (EUR 1,000,000)
        fixedSchedule,                  // Fixed leg schedule
        0.0,                            // Fixed rate (set to calculate par rate)
        QuantLib::Thirty360(QuantLib::Thirty360::European),                    // Fixed leg day count convention
        floatSchedule,                  // Floating leg schedule
        euribor6m,                      // Floating index
        0.0,                            // Spread on the floating leg
        Actual360());                   // Floating leg day count convention

    // Set the pricing engine for the swap
    ext::shared_ptr<PricingEngine> swapEngine = ext::make_shared<DiscountingSwapEngine>(eonia_curve);
    forwardSwap.setPricingEngine(swapEngine);

    // Calculate par rate for the ATM swaption
    Rate atmStrike = forwardSwap.fairRate();

    // Define the swaption exercise (European)
    ext::shared_ptr<Exercise> europeanExercise = ext::make_shared<EuropeanExercise>(startDate);

    // Create the physically settled swaption
    Swaption swaption(ext::make_shared<VanillaSwap>(forwardSwap), europeanExercise);

    // Set the pricing engine for the swaption
    ext::shared_ptr<PricingEngine> swaptionEngine = ext::make_shared<BlackSwaptionEngine>(
        eonia_curve, 0.69683); // Volatility (e.g., 20%)
    swaption.setPricingEngine(swaptionEngine);

    // Calculate the swaption NPV
    Real swaptionNPV = swaption.NPV();

    // Output results
    std::cout << "Settlement Date: " << date << std::endl;
    std::cout << "Start Date: " << startDate << std::endl;
    std::cout << "End Date: " << endDate << std::endl;
    std::cout << "ATM Strike (Par Rate): " << atmStrike << std::endl;
    std::cout << "Swaption NPV: " << swaptionNPV << std::endl;


    Real spotPrice = 603.5;
    Real forwardPrice = 767.5;

    // Calculate discount factor to forward start date
    Real discountFactor = spotPrice / forwardPrice;

    // Assume we have a constant implied volatility (e.g., 20%)
    Volatility impliedVol = 0.2;

    // Set up swaption parameters
    Real nominal = 1.0; // Assume unit nominal
    //Period swapTenor = 10 * Years; // Underlying swap tenor
    Period forwardStart = 10 * Years; // Forward start date

    // Placeholder for annuity
    Real annuity = 0.0;

    // Iterative method to approximate annuity (simplified for demonstration)
    // Assume a Black model formula approximation for the swaption price
    Real guessAnnuity = 1.0; // Initial guess for annuity
    Real tolerance = 1e-6;
    Real error = 1.0;

    while (error > tolerance) {
        Real calculatedForwardPrice = guessAnnuity * BlackCalculator(
            Option::Call,
            /* Forward Rate (assume ATM) */ 0.03,
            /* Strike */ 0.03,
            impliedVol * std::sqrt(forwardStart.length()),
            nominal).value();

        error = std::fabs(calculatedForwardPrice - forwardPrice);

        // Update annuity
        guessAnnuity -= error * 0.1; // Adjust step size
    }

    annuity = guessAnnuity;

    // Output results
    std::cout << "Discount Factor: " << discountFactor << std::endl;
    std::cout << "Calculated Annuity: " << annuity << std::endl;

    return 0;
}


//// Inspect the default properties of the IborIndex
//std::cout << "Tenor: " << iborIndex->tenor() << std::endl;
//std::cout << "Day Counter: " << iborIndex->dayCounter() << std::endl;
//std::cout << "Business Day Convention: " << iborIndex->businessDayConvention() << std::endl;
//std::cout << "Fixing Calendar: " << iborIndex->fixingCalendar().name() << std::endl;
//std::cout << "Fixing Days: " << iborIndex->fixingDays() << std::endl;


///// Define forward period: starts in 1 month, lasts 10 years
//QuantLib::Date startDate(29, QuantLib::November, 2024);//QuantLib::(29, QuantLib::November, 2024);//calendar.advance(date, QuantLib::Period(29, QuantLib::November, 2024));  // T1 = 1M
//QuantLib::Date endDate = calendar.advance(startDate, QuantLib::Period(10, QuantLib::Years));         // T2 = T1 + 10Y

//// Extract discount factors
//QuantLib::DiscountFactor dfStart = euribor_6m_curve->discount(startDate);
//QuantLib::DiscountFactor dfEnd = euribor_6m_curve->discount(endDate);

//// Compute the forward rate
//QuantLib::Time t1 = dayCountConv.yearFraction(date, startDate);
//QuantLib::Time t2 = dayCountConv.yearFraction(date, endDate);
//QuantLib::Rate forwardRate = (dfStart / dfEnd - 1.0) / (t2 - t1);

//// Print results
//std::cout << "Settlement Date: " << date << std::endl;
//std::cout << "Start Date (T1): " << startDate << std::endl;
//std::cout << "End Date (T2): " << endDate << std::endl;
//std::cout << "Discount Factor at T1: " << dfStart << std::endl;
//std::cout << "Discount Factor at T2: " << dfEnd << std::endl;
//std::cout << "10Y Forward Rate Starting 1M from Settlement: " << forwardRate << std::endl;