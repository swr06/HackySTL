#include "../../cpp/UnorderedMap.hpp"

#include <iostream>
#include <unordered_map>
#include <benchmark/benchmark.h>

static void hsdMap(benchmark::State& state)
{
    for(auto _ : state)
    {
        hsd::unordered_map<const char*, int> map;
        map["key1"] = 0;
        map["key2"] = 1;
        map["key3"] = 2;
        map["key4"] = 0;
        map["key5"] = 1;
        map["key6"] = 2;
        map["key7"] = 0;
        map["key8"] = 1;

        for(auto _it : map)
            std::cout << _it.second << '\r';

        benchmark::DoNotOptimize(map);
    }
}
    
static void stdMap(benchmark::State& state)
{
    for(auto _ : state)
    {
        std::unordered_map<const char*, int> map;
        map["key1"] = 0;
        map["key2"] = 1;
        map["key3"] = 2;
        map["key4"] = 0;
        map["key5"] = 1;
        map["key6"] = 2;
        map["key7"] = 0;
        map["key8"] = 1;
    
        for(auto _it : map)
            std::cout << _it.second << '\r';

        benchmark::DoNotOptimize(map);
    }
}

BENCHMARK(hsdMap);

BENCHMARK(stdMap);

BENCHMARK_MAIN();