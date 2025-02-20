#pragma once

#define EXPECT_EQUAL_SPAN(a, b) {               \
        EXPECT_EQ(a.size(), b.size());          \
        bool equal = true;                      \
        for (size_t i = 0; i < a.size(); i++) { \
            if (a[i] != b[i]) {                 \
                equal = false;                  \
                break;                          \
            }                                   \
        }                                       \
    }
