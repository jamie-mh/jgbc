#pragma once

#define GET_BIT(data, bit) (((data) >> (bit)) & 1)

#define ASSERT_NOT_REACHED() assert(false)
