#pragma once

#include "../Bikini.h"

#include <iostream>
#include <array>

test_set basics_tests
    std::string("Basics"), {
        std::make_tuple(
        std::string("Basics test"),
        std::function<bool()>([]() -> bool
            a <- 0
            std::array<int, 3> arr = {1, 2, 3}
            apply(arr, [](int z)
                return (z + 1)
            )
            foreach(arr, [&](int x)
                std::cout << "x: " << x << std::endl
            )
            unless(false)
                repeat(2)
                    until(a > 2)
                        std::cout << "a: " << a << std::endl
                        a++
            //
            return arr[2] == 4 && a == 3
        )) /*;*/
;
