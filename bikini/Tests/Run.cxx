#include "../Bikini.h"

#include "Basics.hpp"
#include "Memo.hpp"

int main()

    run_test_set (basics_tests, std::cout)
    run_test_set (memo_tests,   std::cout)
    return 0
