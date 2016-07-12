# ATL vs. STL String Performance Tests

by Giovanni Dicanio

I wrote some tests for comparing the ATL vs. STL string performance.

This simple C++ code tests **inserting** strings into an STL `std::vector`, and then **sorting** the aforementioned string vector.

Both ATL’s `CStringW` and STL’s `wstring` are tested.

Building that code with VS2015 results in the STL’s `std::wstring` class winning for both the general case and for tiny strings (in this case the STL’s _small string optimization_ (**SSO**) determines a huge performance increase over ATL’s `CString`).
 
