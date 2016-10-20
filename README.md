# jsoncons: a C++ library for json construction

jsoncons is a C++ library for the construction of [JavaScript Object Notation (JSON)](http://www.json.org). It supports parsing a JSON file or string into a `json` value, building a `json` value in C++ code, and serializing a `json` value to a file or string. It also provides an API for generating json read and write events in code, somewhat analogously to SAX processing in the XML world. 

jsoncons uses some features that are new to C++ 11, including [move semantics](http://thbecker.net/articles/rvalue_references/section_02.html) and the [AllocatorAwareContainer](http://en.cppreference.com/w/cpp/concept/AllocatorAwareContainer) concept. It has been tested with MS VC++ 2013, MS VC++ 2015, GCC 4.8, GCC 4.9, GCC 6.2.0 and recent versions of clang. Note that `std::regex` isn't fully implemented in GCC 4.8., so `jsoncons_ext/jsonpath` regular expression filters aren't supported for that compiler.

The [code repository](https://github.com/danielaparker/jsoncons) and [releases](https://github.com/danielaparker/jsoncons/releases) are on github. It is distributed under the [Boost Software License](http://www.boost.org/users/license.html)

The library has a number of features, which are listed below:

- Uses the standard C++ input/output streams library
- Supports converting to and from the standard library sequence and associative containers
- Supports object members sorted alphabetically by name or in original order
- Implements parsing and serializing JSON text in UTF-8 for narrow character strings and streams
- Supports UTF16 (UTF32) encodings with size 2 (size 4) wide characters
- Correctly handles surrogate pairs in \uXXXX escape sequences
- Supports event based JSON parsing and serializing with user defined input and output handlers
- Accepts and ignores single line comments that start with //, and multi line comments that start with /* and end with */
- Supports optional escaping of the solidus (/) character
- Supports Nan, Inf and -Inf replacement
- Supports reading a sequence of JSON texts from a stream
- Supports optional escaping of non-ascii UTF-8 octets
- Allows extensions to the types accepted by the json class constructors, accessors and modifiers
- Supports reading (writing) JSON values from (to) CSV files
- Supports serializing JSON values to [JSONx](http://www.ibm.com/support/knowledgecenter/SS9H2Y_7.5.0/com.ibm.dp.doc/json_jsonx.html) (XML)
- Passes all tests from [JSON_checker](http://www.json.org/JSON_checker/) except `fail1.json`, which is allowed in [RFC7159](http://www.ietf.org/rfc/rfc7159.txt)
- Handles JSON texts of arbitrarily large depth of nesting, a limit can be set if desired
- Supports [Stefan Goessner's JsonPath](http://goessner.net/articles/JsonPath/)

As the `jsoncons` library has evolved, names have sometimes changed. To ease transition, jsoncons deprecates the old names but continues to support many of them. See the [deprecated list](https://github.com/danielaparker/jsoncons/wiki/deprecated) for the status of old names. The deprecated names can be suppressed by defining macro `JSONCONS_NO_DEPRECATED`, which is recommended for new code.

## Benchmarks

[json_benchmarks](https://github.com/danielaparker/json_benchmarks) provides some measurements about how `jsoncons` compares to other `json` libraries.

## What's new

- New release 0.99.3a
- The deprecated class `json::any` has been removed from master
- The jsoncons `boost` extension has been removed. That extension contained a sample `json_type_traits` specialization for `boost::gregorian::date`, which may still be found in the [Type Extensibility](https://github.com/danielaparker/jsoncons/wiki/Type-Extensibility) tutorial.  
- The member `json_type_traits` member function `assign` has been removed and replaced by `to_json`. if you have implemented your own type specializations, you will also have to change your `assign` function to `to_json`.

## Get jsoncons

The jsoncons library is header-only: it consists solely of header files containing templates and inline functions, and requires no separately-compiled library binaries when linking. It has no dependence on other libraries. The accompanying test suite uses boost, but not the library itself.

To install the jsoncons library, download the zip file, unpack the release, under `src` find the directory `jsoncons`, and copy it to your `include` directory. If you wish to use extensions, copy the `jsoncons_ext` directory as well. 

## A simple program using jsoncons

```c++
#include <iostream>
#include "jsoncons/json.hpp"

// For convenience
using jsoncons::json;

int main()
{
    json color_spaces = json::array();
    color_spaces.add("sRGB");
    color_spaces.add("AdobeRGB");
    color_spaces.add("ProPhoto RGB");

    json image_sizing; // empty object
    image_sizing["Resize To Fit"] = true; // a boolean 
    image_sizing["Resize Unit"] = "pixels"; // a string
    image_sizing["Resize What"] = "long_edge"; // a string
    image_sizing["Dimension 1"] = 9.84; // a double
    
    json file_export;
    file_export["File Format Options"]["Color Spaces"] =
        std::move(color_spaces);
    file_export["Image Sizing"] = std::move(image_sizing);

    std::cout << "(1)" << "\n";
    std::cout << pretty_print(file_export) << "\n\n";

    const json& val = file_export["Image Sizing"];

    std::cout << "(2) " << "Dimension 1 = " << val["Dimension 1"].as<double>() << "\n\n";

    std::cout << "(3) " << "Dimension 2 = " << val.get_with_default("Dimension 2","null") << "\n";
}
```
Output:
```json
(1)
{
    "File Format Options": {
        "Color Spaces": ["sRGB","AdobeRGB","ProPhoto RGB"]
    },
    "Image Sizing": {
        "Dimension 1": 9.84,
        "Resize To Fit": true,
        "Resize Unit": "pixels",
        "Resize What": "long_edge"
    }
}

(2) Dimension 1 = 9.84

(3) Dimension 2 = null
```

For a quick guide, see the article [jsoncons: a C++ library for json construction](http://danielaparker.github.io/jsoncons). Consult the [wiki](https://github.com/danielaparker/jsoncons/wiki) for the latest documentation, tutorials and roadmap. 

## Building the test suite and examples with CMake

[CMake](https://cmake.org/) is a cross-platform build tool that generates makefiles and solutions for the compiler environment of your choice. On Windows you can download a [Windows Installer package](https://cmake.org/download/). On Linux it is usually available as a package, e.g., on Ubuntu,
```
    sudo apt-get install cmake
```

Instructions for building the test suite with CMake may be found in

    jsoncons/test_suite/build/cmake/README.txt

Instructions for building the examples with CMake may be found in

    jsoncons/examples/build/cmake/README.txt

## About jsoncons::basic_json

The jsoncons library provides a `basic_json` class template, which is the generalization of a `json` value for different character types, different policies for ordering name-value pairs, etc.
```c++
typedef basic_json<char,
                   JsonTraits = json_traits<char>,
                   Allocator = std::allocator<char>> json;
```
The library includes four instantiations of `basic_json`:

- [json](https://github.com/danielaparker/jsoncons/wiki/json) constructs a narrow character json value that sorts name-value members alphabetically

- [ojson](https://github.com/danielaparker/jsoncons/wiki/ojson) constructs a narrow character json value that retains the original name-value insertion order

- [wjson](https://github.com/danielaparker/jsoncons/wiki/wjson) constructs a wide character json value that sorts name-value members alphabetically

- [wojson](https://github.com/danielaparker/jsoncons/wiki/wojson) constructs a wide character json value that retains the original name-value insertion order

## Features

### Meaningful error messages

```c++
try 
{
    json val = json::parse("[1,2,3,4,]");
} 
catch(const std::exception& e) 
{
    std::cout << e.what() << std::endl;
}
```
Output:
```
Extra comma at line 1 and column 10
```

### Use range-based for loops with arrays

```c++
json j = json::array{1,2,3,4};

for (auto element : book.elements())
{
    std::cout << element << std::endl;
}
```

### Use range-based for loops with objects

```c++
json book = json::object{
    {"author", "Haruki Murakami"},
    {"title", "Kafka on the Shore"},
    {"price", 25.17}
};

for (const auto& member : book.members())
{
    std::cout << member.name() << "=" 
              << member.value() << std::endl;
}
```

### Construct multi-dimensional json arrays
```c++
    json j = json::make_array<3>(4,3,2,0.0);
    j[0][2][0] = 2.1;
    j[0][2][1] = 3.1;
```
See [Arrays](https://github.com/danielaparker/jsoncons/wiki/Arrays) for details

### Convert from and to standard library sequence containers

```c++
std::vector<int> v = {1,2,3,4};
json j(v);
std::deque<int> d = j.as<std::deque<int>();
```

### Convert from and to standard library associative containers

```c++
std::map<std::string, int> m{{"one", 1}, 
                             {"two", 2}, 
                             {"three", 3}};
json j(m);
std::unordered_map<std::string, int> um = j.as<std::unordered_map<std::string, int>>();
```

### multimap containers too

```c++
std::multimap<std::string, int> mm{{"one", 1}, 
                                   {"two", 1}, 
                                   {"three", 0}, 
                                   {"three", 1}};
json j(mm); // two entries for key "three"
std::cout << "(1) " << j << "\n\n";
std::cout << "(2) " << j.count("three") << "\n\n";
std::cout << "(3)" << "\n";
for (auto member: j.members("three"))
{
    std::cout << member.value().as<int>() << "\n";
}
```
Output:
```
(1) {"one":1,"three":0,"three":1,"two":1}

(2) 2

(3)
0
1
```
Note that [rfc7159](https://tools.ietf.org/html/rfc7159) _discourages_ duplicate names - "The names within an object SHOULD be unique" - but does not forbid them. 

### Convert from and to user defined types (and standard library containers of user defined types)

```c++
struct book
{
    std::string author;
    std::string title;
    double price;
};

namespace jsoncons
{
    template<class Json>
    struct json_type_traits<Json, book>
    {
        // Implement static functions is, as and to_json 
    };
}        

book book1{"Haruki Murakami", "Kafka on the Shore", 25.17};
book book2{"Charles Bukowski", "Women: A Novel", 12.0};

std::vector<book> v{book1, book2};

json j = v;

std::list<book> l = j.as<std::list<book>>();
```

See [Type Extensibility](https://github.com/danielaparker/jsoncons/wiki/Type%20Extensibility) for details.

### Query json values with JsonPath

```c++
std::ifstream is("booklist.json");
json booklist;
is >> booklist;

// The authors of books that cost less than $12
json result = json_query(booklist, "$[*][?(@.price < 12)].author");

// The number of books
result = json_query(booklist, "$.length");

// The third book
result = json_query(booklist, "$[2]");

// The authors of books that were published in 2004
result = json_query(booklist, "$[*][?(@.date =~ /2004.*?/)].author");

// The titles of all books that have ratings
result = json_query(booklist, "$[*][?(@.ratings)].title");

// All authors and titles of books
result = json_query(booklist, "$..['author','title']");
```

See [json_query](https://github.com/danielaparker/jsoncons/wiki/json_query) and [Basics](https://github.com/danielaparker/jsoncons/wiki/Basics) for details.

### Read a csv file as a json value

```c++
std::fstream is("tasks.csv");

json_deserializer handler;

csv_parameters params;
params.assume_header(true)
      .trim(true)
      .ignore_empty_values(true)
      .column_types({"integer","string","string","string"});

csv_reader reader(is,handler,params);
reader.read();
json val = handler.get_result();
```
See [csv_reader](https://github.com/danielaparker/jsoncons/wiki/csv_reader) for details

## Acknowledgements

Special thanks to our [contributors](https://github.com/danielaparker/jsoncons/blob/master/acknowledgements.txt)

