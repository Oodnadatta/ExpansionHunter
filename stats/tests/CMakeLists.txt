add_executable(ReadSupportCalculatorTest ReadSupportCalculatorTest.cpp)
target_link_libraries(ReadSupportCalculatorTest stats gtest_main)
add_test(NAME ReadSupportCalculatorTest COMMAND ReadSupportCalculatorTest)

add_executable(WeightedPurityCalculatorTest WeightedPurityCalculatorTest.cpp)
target_link_libraries(WeightedPurityCalculatorTest stats gtest gmock_main)
add_test(NAME WeightedPurityCalculatorTest COMMAND WeightedPurityCalculatorTest)
