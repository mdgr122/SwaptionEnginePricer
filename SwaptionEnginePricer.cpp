#include <ql/quantlib.hpp>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>

// Assuming "YieldCurveFuncs.h" includes the functions defineTenors and defineRates
#include "YieldCurveFuncs.h"

using namespace QuantLib;

// Function to compute swaption price using the Bachelier formula
Real bachelierSwaptionPrice(Real forward, Real strike, Real annuity, Volatility vol, Time T)
{
    CumulativeNormalDistribution N;
    Real stdDev = vol * std::sqrt(T);
    if (stdDev == 0.0)
    {
        // Avoid division by zero
        return std::max(forward - strike, 0.0) * annuity;
    }
    Real d = (forward - strike) / stdDev;
    Real price = annuity * ((forward - strike) * N(d) + stdDev * N.derivative(d));
    return price;
}

// Objective function for the solver
class BachelierObjectiveFunction
{
public:
    BachelierObjectiveFunction(Real forward, Real strike, Real annuity, Time expiry, Real targetPrice)
        : forward_(forward), strike_(strike), annuity_(annuity), expiry_(expiry), targetPrice_(targetPrice)
    {
    }

    Real operator()(Volatility vol) const
    {
        Real price = bachelierSwaptionPrice(forward_, strike_, annuity_, vol, expiry_);
        return price - targetPrice_;
    }

private:
    Real forward_;
    Real strike_;
    Real annuity_;
    Time expiry_;
    Real targetPrice_;
};

int main()
{
    // Input data
    std::vector<std::string> input_tenors_estr{
        "1W", "2W", "3W", "1M", "2M", "3M", "4M", "5M", "6M", "7M", "8M", "9M", "10M", "11M", "1Y",
        "15M", "18M", "21M", "2Y", "3Y", "4Y", "5Y", "6Y", "7Y", "8Y", "9Y", "10Y", "11Y", "12Y",
        "15Y", "20Y", "25Y", "30Y", "40Y", "50Y"
    };
    std::vector<float> input_rates_estr{
        0.03164, 0.031655, 0.031665, 0.031675, 0.030819, 0.03019, 0.02926, 0.028255, 0.0274045,
        0.026695, 0.026043, 0.025464, 0.0249665, 0.024548, 0.024207, 0.023299, 0.0227545,
        0.022433, 0.022237, 0.0219645, 0.021903, 0.021952, 0.0221345, 0.022352, 0.0225925,
        0.0228395, 0.0230765, 0.023277, 0.0234875, 0.0238965, 0.023541, 0.022661, 0.0217935,
        0.0204395, 0.0191035
    };
    std::vector<std::string> input_tenors_euribor_6m{
        "1Y", "18M", "2Y", "3Y", "4Y", "5Y", "6Y", "7Y", "8Y", "9Y", "10Y", "12Y", "15Y", "20Y",
        "25Y", "30Y", "40Y", "50Y", "60Y", "70Y"
    };
    std::vector<float> input_rates_euribor_6m{
        0.026, 0.024616, 0.0242, 0.023895, 0.023815, 0.023818, 0.023912, 0.024032, 0.024168,
        0.024304, 0.024425, 0.024645, 0.0248197, 0.024142, 0.023032, 0.0219885, 0.02024,
        0.01865, 0.017464767, 0.016728967
    };

    // Convert input data to QuantLib types
    std::vector<Period> output_tenors_estr = defineTenors(input_tenors_estr);
    std::vector<Rate> output_rates_estr = defineRates(input_rates_estr);

    std::vector<Period> output_tenors_euribor_6m = defineTenors(input_tenors_euribor_6m);
    std::vector<Rate> output_rates_euribor_6m = defineRates(input_rates_euribor_6m);

    // Step 1: Set up the evaluation date and calendar
    Calendar calendar = TARGET();
    Date today(31, October, 2024);
    Settings::instance().evaluationDate() = today;

    // Step 2: Define the forward-starting period and swap tenor
    Period exercisePeriod = 10 * Years; // Swaption starts in 10 years
    Period swapTenor = 10 * Years;      // Swap duration: 10 years
    Date startDate = calendar.advance(today, exercisePeriod, ModifiedFollowing);  // Forward start date
    Date endDate = calendar.advance(startDate, swapTenor, ModifiedFollowing);     // Swap end date

    // Step 3: Construct yield curves using output tenors and rates
    // ESTR Curve (Overnight Index)
    auto estr = ext::make_shared<Estr>();
    Handle<YieldTermStructure> estrCurve = buildYieldCurve_ois(
        today, calendar, Actual365Fixed(), ModifiedFollowing, estr,
        output_tenors_estr, output_rates_estr);
    estr->addFixing(estr->fixingDate(today), estrCurve->zeroRate(today, Actual365Fixed(), Continuous).rate());

    // EURIBOR 6M Curve (Ibor Index)
    auto euribor6mCurve = buildYieldCurve_ibor(
        today, calendar, Annual, Thirty360(Thirty360::European),
        ModifiedFollowing, ext::make_shared<Euribor6M>(), output_tenors_euribor_6m, output_rates_euribor_6m);

    // Create the EURIBOR 6M index with the forecasting curve
    auto euribor6mCurveHandle = Handle<YieldTermStructure>(euribor6mCurve);
    auto euribor6m = ext::make_shared<Euribor6M>(euribor6mCurveHandle);

    // Step 4: Create the swap schedules
    Schedule fixedSchedule(
        startDate, endDate, Period(Annual), calendar,
        ModifiedFollowing, ModifiedFollowing,
        DateGeneration::Forward, false);

    Schedule floatSchedule(
        startDate, endDate, euribor6m->tenor(), calendar,
        ModifiedFollowing, ModifiedFollowing,
        DateGeneration::Forward, false);

    // Step 5: Set up the swap
    Real notional = 1.0;  // Notional amount
    // Compute the fair swap rate
    Rate swapRate = 0.0;
    VanillaSwap tempSwap(
        VanillaSwap::Payer,
        notional,
        fixedSchedule,
        swapRate,
        Thirty360(Thirty360::European),
        floatSchedule,
        euribor6m,
        0.0,
        euribor6m->dayCounter());

    // Use the ESTR curve for discounting
    auto discountEngine = ext::make_shared<DiscountingSwapEngine>(estrCurve);
    tempSwap.setPricingEngine(discountEngine);
    swapRate = tempSwap.fairRate();

    // Now create the par swap with the computed swap rate
    VanillaSwap parSwap(
        VanillaSwap::Payer,
        notional,
        fixedSchedule,
        swapRate,
        Thirty360(Thirty360::European),
        floatSchedule,
        euribor6m,
        0.0,
        euribor6m->dayCounter());

    parSwap.setPricingEngine(discountEngine);

    // Swap Details
    std::cout << "\nPar Swap Details (Using ESTR Discounting):\n";
    std::cout << "Swaption Begin Date: " << startDate << "\nSwap Maturity Date: " << endDate << std::endl;
    std::cout << "Market Swap Rate (Fixed Leg): " << swapRate << std::endl;
    std::cout << "NPV: " << parSwap.NPV() << std::endl;
    std::cout << "Fixed Leg NPV: " << parSwap.fixedLegNPV() << std::endl;
    std::cout << "Floating Leg NPV: " << parSwap.floatingLegNPV() << std::endl;

    // Cash Flows
    std::cout << "\nFixed Leg Cash Flows:" << std::endl;
    for (const auto& cf : parSwap.fixedLeg())
    {
        std::cout << "Date: " << cf->date() << ", Amount: " << cf->amount() << std::endl;
    }

    std::cout << "\nFloating Leg Cash Flows:" << std::endl;
    for (const auto& cf : parSwap.floatingLeg())
    {
        if (auto coupon = ext::dynamic_pointer_cast<Coupon>(cf))
        {
            Rate forwardRate = coupon->rate();
            std::cout << "Date: " << cf->date()
                << ", Amount: " << cf->amount()
                << ", Forward Rate: " << forwardRate << std::endl;
        }
    }

    // Spot & Forward Annuity Calculations
    DayCounter fixedLegDayCounter = Thirty360(Thirty360::European);

    std::cout << "\nAnnuity Calculations (Using ESTR Discounting):\n";
    std::cout << "Payment Date,Discount Factor,Delta (tau_i),Spot Contribution,Forward Contribution" << std::endl;

    double spotAnnuity = 0.0;
    double forwardAnnuity = 0.0;

    Date previousDate = startDate;
    DiscountFactor discountAtStartDate = estrCurve->discount(startDate);

    for (const auto& paymentDate : fixedSchedule)
    {
        if (paymentDate > startDate)
        {
            DiscountFactor discountFactor = estrCurve->discount(paymentDate);

            // Accrual period between payment dates
            double delta = fixedLegDayCounter.yearFraction(previousDate, paymentDate);

            // Spot Annuities
            double spotContribution = discountFactor * delta;
            spotAnnuity += spotContribution;

            // Forward Annuities
            double forwardDiscountFactor = discountFactor / discountAtStartDate;
            double forwardContribution = forwardDiscountFactor * delta;
            forwardAnnuity += forwardContribution;

            // Output details
            std::cout << formatQuantLibDate(paymentDate) << ","
                << discountFactor << ","
                << delta << ","
                << spotContribution << ","
                << forwardContribution << std::endl;

            // Update previous date
            previousDate = paymentDate;
        }
    }

    // Annuity Sums
    std::cout << "Total Spot Annuity:," << spotAnnuity << std::endl;
    std::cout << "Total Forward Annuity:," << forwardAnnuity << std::endl;

    // Creating swaption object & price w Bachelier's
    auto europeanExercise = ext::make_shared<EuropeanExercise>(startDate);

    // Clone the underlying swap for the swaption
    auto swap = ext::make_shared<VanillaSwap>(parSwap);

    // Create the swaption
    Swaption swaption(swap, europeanExercise);

    // Define the volatility (hypothetical value)
    Volatility vol = 0.01; // 1% normal volatility

    // Flat vol structure assumption
    auto volatilityHandle = Handle<Quote>(ext::make_shared<SimpleQuote>(vol));
    auto volStructure = Handle<SwaptionVolatilityStructure>(
        ext::make_shared<ConstantSwaptionVolatility>(
            today, calendar, ModifiedFollowing, vol, Actual365Fixed(), Normal));

    // Set up the pricing engine using Bachelier's model
    auto swaptionEngine = ext::make_shared<BachelierSwaptionEngine>(estrCurve, volStructure);

    // Assign the pricing engine to the swaption
    swaption.setPricingEngine(swaptionEngine);

    // Price the swaption
    double swaptionNPV = swaption.NPV();
    std::cout << "\nSwaption NPV (Using ESTR Discounting): " << swaptionNPV << std::endl;

    // Output additional details
    std::cout << "ATM Strike: " << swapRate << std::endl;
    Time timeToExpiry = Actual365Fixed().yearFraction(today, startDate);
    std::cout << "Time to Expiry: " << timeToExpiry << " years" << std::endl;
    std::cout << "Discount Factor at Forward Start Date: " << discountAtStartDate << std::endl;

    // Compute Implied Normal Volatility using Bachelier's model
    Date swaptionExpiryDate = startDate; // Swaption expiry date
    DiscountFactor dfToExpiry = estrCurve->discount(swaptionExpiryDate);
    std::cout << "Discount Factor to Swaption Expiry: " << dfToExpiry << std::endl;

    // Define the market swaption price (replace with the actual market price)
    Real marketSwaptionPrice_spot = 0.06035; // Example market price (NPV)
    Real marketSwaptionPrice_fwd = 0.07675;  // Example market price (Forward price)
    Real marketSwaptionPrice_fwd_discounted = marketSwaptionPrice_fwd * dfToExpiry;
    std::cout << "Discounted Forward Price: " << marketSwaptionPrice_fwd_discounted << std::endl;

    // Set up an initial guess for the volatility
    Volatility initialGuessVol = 0.02;
    Real accuracy = 1e-6;
    Volatility minVol = 1e-7;
    Volatility maxVol = 1.0;

    // Set up the pricing engine without specifying volatility (volatility will be set during the calculation)
    auto impliedVolSwaptionEngine = ext::make_shared<BachelierSwaptionEngine>(estrCurve, 0.0);
    swaption.setPricingEngine(impliedVolSwaptionEngine);

    // Compute the implied volatility using QuantLib's built-in method (Spot Annuity)
    Volatility impliedVolSpot = swaption.impliedVolatility(
        marketSwaptionPrice_spot,
        estrCurve,          // Discount curve
        initialGuessVol,    // Initial guess
        accuracy,           // Accuracy
        100,                // Max evaluations
        minVol,             // Min volatility
        maxVol,             // Max volatility,
        Normal);            // Volatility type

    Volatility impliedVolFwdDiscounted = swaption.impliedVolatility(
        marketSwaptionPrice_fwd_discounted,
        estrCurve,          // Discount curve
        initialGuessVol,    // Initial guess
        accuracy,           // Accuracy
        100,                // Max evaluations
        minVol,             // Min volatility
        maxVol,             // Max volatility,
        Normal);            // Volatility type

    // Output the results
    std::cout << "\nImplied Normal Volatility (Spot Price): " << impliedVolSpot << std::endl;
    std::cout << "Implied Normal Volatility (Forward Price, Discounted): " << impliedVolFwdDiscounted << std::endl;

    // Optional: Step 10: Compute Implied Volatility using Forward Annuity
    /*
    // Define the parameters for manual calculation
    Real forward = swap->fairRate();     // Forward swap rate
    Real strike = swapRate;              // Swap rate (strike of the swaption)
    Time expiry = timeToExpiry;          // Time to expiry

    // Use the forward annuity
    double annuityToUse = forwardAnnuity;

    // Create the objective function
    Real targetPriceForward = marketSwaptionPrice_fwd; // Use the forward price (undiscounted)
    BachelierObjectiveFunction objective(forward, strike, annuityToUse, expiry, targetPriceForward);

    // Solve for implied volatility using the Brent solver
    Brent solver;
    Volatility impliedVolForward = solver.solve(objective, accuracy, initialGuessVol, minVol, maxVol);

    // Output the result
    std::cout << "Computed Implied Normal Volatility (Using Forward Annuity): " << impliedVolForward << std::endl;
    */

    return 0;
}