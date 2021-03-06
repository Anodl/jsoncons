### jsoncons::decode_json

Converts a JSON formatted string to a C++ object. `decode_json` attempts to 
perform the conversion by streaming using `json_convert_traits`, and if
streaming is not supported, falls back to using `json_type_traits`. `decode_json` will 
work for all types that have `json_type_traits` defined.

#### Header

```c++
#include <jsoncons/json.hpp>

template <class T, class CharT>
T decode_json(const std::basic_string<CharT>& s)
{
    return json_convert_traits<T>::decode(s);
}
```

### Examples

#### Map with string-tuple pairs

```c++
#include <iostream>
#include <map>
#include <tuple>
#include <jsoncons/json.hpp>

using namespace jsoncons;

int main()
{
    typedef std::map<std::string,std::tuple<std::string,std::string,double>> employee_collection;

    employee_collection employees = 
    { 
        {"John Smith",{"Hourly","Software Engineer",10000}},
        {"Jane Doe",{"Commission","Sales",20000}}
    };

    std::string s;
    jsoncons::encode_json(employees, s, jsoncons::indenting::indent);
    std::cout << "(1)\n" << s << std::endl;
    auto employees2 = jsoncons::decode_json<employee_collection>(s);

    std::cout << "\n(2)\n";
    for (const auto& pair : employees2)
    {
        std::cout << pair.first << ": " << std::get<1>(pair.second) << std::endl;
    }
}
```
Output:
```
(1)
{
    "Jane Doe": ["Commission","Sales",20000.0],
    "John Smith": ["Hourly","Software Engineer",10000.0]
}

(2)
Jane Doe: Sales
John Smith: Software Engineer
```

