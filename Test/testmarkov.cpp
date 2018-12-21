#include "Markov/mmc.h"

#include "gtest/gtest.h"

namespace uiiit {
namespace markov {

struct TestMarkov : public ::testing::Test {};

TEST_F(TestMarkov, test_average) {
  ASSERT_FLOAT_EQ(1.0, MMC(7, 8, 1, 0).averageResponseTime());
  ASSERT_FLOAT_EQ(1.33333333, MMC(1, 1, 2, 0).averageResponseTime());
  ASSERT_FLOAT_EQ(0.12883875, MMC(7, 8, 3, 0).averageResponseTime());
  ASSERT_THROW(MMC(1, 1, 1, 0).averageResponseTime(), std::runtime_error);
}

} // namespace markov
} // namespace uiiit
