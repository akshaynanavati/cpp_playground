#pragma once

#include <ios>
#include <iostream>
#include <sstream>
#include <string>

using cstr = const char *const;

static constexpr cstr past_last_slash(cstr str, cstr last_slash) {
  return *str == '\0' ? last_slash
                      : *str == '/' ? past_last_slash(str + 1, str + 1)
                                    : past_last_slash(str + 1, last_slash);
}

static constexpr cstr past_last_slash(cstr str) {
  return past_last_slash(str, str);
}

#define ENDL std::endl;

#define __SHORT_FILE__                                                         \
  ({                                                                           \
    constexpr cstr sf__{past_last_slash(__FILE__)};                            \
    sf__;                                                                      \
  })

#define LOG()                                                                  \
  std::cout << '[' << __SHORT_FILE__ << ':' << __LINE__ << " - " << __func__   \
            << "] "

#define PRINT_EXPR(expr) LOG() << #expr << " = " << expr << ENDL

#define PRINT_BOOL(b) LOG() << #b << " = " << ((b) ? "true" : "false") << ENDL

#define PRINT_IF_NOT_GT(e1, e2)                                                \
  do {                                                                         \
    if (!((e1) > (e2))) {                                                      \
      LOG() << #e1 << " = " << (e1) << " !> " << #e2 << " = " << e2 << ENDL;   \
    }                                                                          \
  } while (0)

#define PRINT_IF_NOT_LT(e1, e2)                                                \
  do {                                                                         \
    if (!((e1) < (e2))) {                                                      \
      LOG() << #e1 << " = " << (e1) << " !< " << #e2 << " = " << e2 << ENDL;   \
    }                                                                          \
  } while (0)

#define PRINT_IF_NOT_EQ(e1, e2)                                                \
  do {                                                                         \
    if ((e1) != (e2)) {                                                        \
      LOG() << #e1 << " = " << (e1) << " != " << #e2 << " = " << e2 << ENDL;   \
    }                                                                          \
  } while (0)

#define PRINT_DELIM()                                                          \
  LOG() << "=================================================================" \
           "==============="                                                   \
        << ENDL
