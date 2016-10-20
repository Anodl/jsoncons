// Copyright 2013 Daniel Parker
// Distributed under Boost license

#include <string>
#include <deque>
#include <map>
#include <list>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <forward_list>
#include <iostream>
#include <fstream>
#include "jsoncons/json.hpp"

using namespace jsoncons;

void container_example1()
{
    std::cout << "Convert from and to standard library containers\n" << std::endl;

    std::vector<int> v{1, 2, 3, 4};
    json j_v(v);
    std::cout << j_v << std::endl;

    std::vector<unsigned long> vec2{1ul, 2ul, 3ul, 4ul};
    json j_vec2(vec2);
    std::cout << j_vec2 << std::endl;

    std::deque<double> deque1{1.123, 2.234, 3.456, 4.567};
    json j_deque1(deque1);
    std::cout << j_deque1 << std::endl;

    std::list<bool> list1{true, true, false, true};
    json j_list1(list1);
    std::cout << j_list1 << std::endl;

    std::forward_list<int64_t>flist1 {12345678909876, 23456789098765, 34567890987654, 45678909876543};
    json j_flist1(flist1);
    std::cout << j_flist1 << std::endl;

    std::array<unsigned long, 4> array1 {{1, 2, 3, 4}};
    json j_array1(array1);

    std::set<std::string> set1{"one", "two", "three", "four", "one"};
    json j_set1(set1); // only one entry for "one" is used
    std::cout << j_set1 << std::endl;
    // ["four", "one", "three", "two"]

    std::unordered_set<std::string> uset1{"one", "two", "three", "four", "one"};
    json j_uset1(uset1); // only one entry for "one" is used
    std::cout << j_uset1 << std::endl;
    // maybe ["two", "three", "four", "one"]

    std::multiset<std::string> mset1{"one", "two", "one", "four"};
    json j_mset1(mset1); // only one entry for "one" is used
    std::cout << j_mset1 << std::endl;
    // maybe ["one", "two", "four"]

    std::unordered_multiset<std::string> umset1 {"one", "two", "one", "four"};
    json j_umset1(umset1); // both entries for "one" are used
    // maybe ["one", "two", "one", "four"]

    std::map<std::string, int> m{{"one", 1}, 
                                 {"two", 2}, 
                                 {"three", 3}};
    json j_m(m);
    std::cout << j_m << std::endl;

    std::unordered_map<std::string, double> umap1{ {"one", 1.2}, {"two", 2.3}, {"three", 3.4} };
    json j_umap1(umap1);
    std::cout << j_umap1 << std::endl;

    std::cout << std::endl;
}

void multimap_example()
{
    std::multimap<std::string, int> mm{{"one", 1}, 
                                       {"two", 1}, 
                                       {"three", 0}, 
                                       {"three", 1}};
    json j_mm(mm); // two entries for key "three"
    std::cout << "(1) " << j_mm << "\n\n";
    std::cout << "(2) " << j_mm.count("three") << "\n\n";
    std::cout << "(3)" << "\n";
    for (auto member: j_mm.members("three"))
    {
        std::cout << member.value().as<int>() << "\n";
    }

    std::cout << std::endl;

    std::multimap<std::string, int> umm{{"one", 1}, {"two", 1}, {"three", 0}, {"three", 1}};
    json j_umm(umm); // two entries for key "three"
    std::cout << "(1) " << j_umm << "\n\n";
    std::cout << "(2) " << j_umm.count("three") << "\n\n";
    std::cout << "(3)" << "\n";
    for (auto member: j_umm.members("three"))
    {
        std::cout << member.value().as<int>() << "\n";
    }

    std::cout << std::endl;
}

void container_examples()
{
    std::cout << "\nContainer examples\n\n";
    container_example1();
    multimap_example();
    std::cout << std::endl;
}

