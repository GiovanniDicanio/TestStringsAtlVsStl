////////////////////////////////////////////////////////////////////////////////
//
// Test Performance of STL's std::wstring vs. ATL's CStringW.
//
// by Giovanni Dicanio <giovanni.dicanio@gmail.com>
//
// C++ Compiler: Visual Studio 2015
//
////////////////////////////////////////////////////////////////////////////////


// Uncomment the following line to do a test with tiny strings (activates STL's string SSO)
//#define TEST_TINY_STRINGS


#include <algorithm>        // For std::shuffle, std::sort
#include <exception>        // For std::exception
#include <iomanip>          // For std::setprecision, std::setw
#include <ios>              // For std::fixed, std::hex
#include <iostream>         // For std::cout
#include <random>           // For std::mt19937
#include <string>           // For std::wstring
#include <vector>           // For std::vector

#include <Windows.h>        // For Windows SDK
#include <atlexcept.h>      // For CAtlException
#include <atlstr.h>         // For ATL's CStringW

#include "Stopwatch.h"      // For measuring time intervals


static void Test();

int main()
{
    constexpr int kExitOk = 0;
    constexpr int kExitError = 1;
    int exitCode = kExitOk;

    try
    {
        Test();
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n*** Standard exception caught: " << e.what() << '\n';
        exitCode = kExitError;
    }
    catch (const ATL::CAtlException& e)
    {
        std::cerr << "ATL exception caught; hr=0x" << std::hex << static_cast<HRESULT>(e) << '\n';
        exitCode = kExitError;
    }
    catch (...)
    {
        std::cerr << "\n*** Unknown Exception ***\n";
        exitCode = kExitError;
    }

    return exitCode;
}


static std::vector<std::wstring> BuildTestStrings()
{
    const std::wstring lorem[] = 
    {
        L"Lorem ipsum dolor sit amet, consectetuer adipiscing elit.",
        L"Maecenas porttitor congue massa. Fusce posuere, magna sed",
        L"pulvinar ultricies, purus lectus malesuada libero,",
        L"sit amet commodo magna eros quis urna.",
        L"Nunc viverra imperdiet enim. Fusce est. Vivamus a tellus.",
        L"Pellentesque habitant morbi tristique senectus et netus et",
        L"malesuada fames ac turpis egestas. Proin pharetra nonummy pede.",
        L"Mauris et orci. [*** add more chars to prevent SSO ***]"
    };

    std::vector<std::wstring> v;

#ifdef _DEBUG
    constexpr int kLoopCount = 10;
#else
    constexpr int kLoopCount = 200'000;
#endif

    for (int i = 0; i < kLoopCount; ++i) 
    {
        for (const auto& s : lorem) 
        {
#ifdef TEST_TINY_STRINGS
            v.push_back(L"#" + std::to_wstring(i));
#else
            v.push_back(s + L" (#" + std::to_wstring(i) + L")");
#endif
        }
    }

    std::mt19937 prng(1980);

    std::shuffle(v.begin(), v.end(), prng);

    return v;
}


struct TimeResult
{
    double InsertTimeMs;
    double SortTimeMs;

    TimeResult() : InsertTimeMs(0.0), SortTimeMs(0.0) {}
};


template <typename StringType>
static TimeResult TestStringType(const std::vector<const wchar_t*> & shuffledPtrs)
{
    TimeResult result;

    win32::Stopwatch sw;
    sw.Start();
    std::vector<StringType> testStrings(shuffledPtrs.begin(), shuffledPtrs.end());
    sw.Stop();
    result.InsertTimeMs = sw.ElapsedMilliseconds();

    sw.Start();
    std::sort(testStrings.begin(), testStrings.end());
    sw.Stop();
    result.SortTimeMs = sw.ElapsedMilliseconds();

    return result;
}


static void PrintTime(const char* const testName, const TimeResult& result)
{
    std::cout << "  " << testName;
    std::cout << "  Insert: " << std::setw(8) << std::setprecision(2) << std::fixed << result.InsertTimeMs << " ms"
        << "    Sort: " << std::setw(8) << std::setprecision(2) << std::fixed << result.SortTimeMs << " ms\n";
}


static void Test()
{
    std::cout << "*** Testing ATL vs. STL String Performance ***\n";
    std::cout << "            by Giovanni Dicanio\n\n";

#ifdef TEST_TINY_STRINGS
    std::cout << "(Testing with tiny strings - activates STL's SSO)\n\n";
#endif

    const std::vector<std::wstring> shuffledStrings = BuildTestStrings();

    const auto shuffledPtrs = [&]() -> std::vector<const wchar_t *> 
    {
        std::vector<const wchar_t *> v;

        for (const auto& s : shuffledStrings) 
        {
            v.push_back(s.c_str());
        }

        return v;
    }();

    std::cout << "\nTesting " << shuffledPtrs.size() << " UTF-16 strings.\n\n";

    TimeResult atl1 = TestStringType<ATL::CStringW>(shuffledPtrs);
    PrintTime("ATL1", atl1);

    TimeResult stl1 = TestStringType<std::wstring>(shuffledPtrs);
    PrintTime("STL1", stl1);

    TimeResult atl2 = TestStringType<ATL::CStringW>(shuffledPtrs);
    PrintTime("ATL2", atl2);

    TimeResult stl2 = TestStringType<std::wstring>(shuffledPtrs);
    PrintTime("STL2", stl2);

    TimeResult atl3 = TestStringType<ATL::CStringW>(shuffledPtrs);
    PrintTime("ATL3", atl3);

    TimeResult stl3 = TestStringType<std::wstring>(shuffledPtrs);
    PrintTime("STL3", stl3);

    std::cout << '\n';
}

