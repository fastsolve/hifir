/*
//@HEADER
//----------------------------------------------------------------------------
//                Copyright (C) 2019 The PSMILU AUTHORS
//----------------------------------------------------------------------------
//@HEADER
*/

/*!
 * \file psmilu_Options.h
 * \brief PS-MILU algorithm parameter controls
 * \authors Qiao,
 * \note Compatible with C99, and must be \b C99 or higher!
 */

#ifndef _PSMILU_OPTIONS_H
#define _PSMILU_OPTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \addtogroup c
 * @{
 */

/*!
 * \brief the verbose level for progress report
 */
enum {
  PSMILU_VERBOSE_NONE = 0,                        /*!< mute */
  PSMILU_VERBOSE_INFO = 1,                        /*!< general information */
  PSMILU_VERBOSE_PRE  = PSMILU_VERBOSE_INFO << 1, /*!< preprocessing */
  PSMILU_VERBOSE_FAC  = PSMILU_VERBOSE_PRE << 1,  /*!< factorization update */
  PSMILU_VERBOSE_MEM  = PSMILU_VERBOSE_FAC << 1,  /*!< memory debug */
};

/*!
 * \struct psmilu_Options
 * \brief POD parameter controls
 * \note Values in parentheses are default settings
 */
struct psmilu_Options {
  double tau_L;     /*!< inverse-based threshold for L (0.01) */
  double tau_U;     /*!< inverse-based threshold for U (0.01) */
  double tau_d;     /*!< threshold for inverse-diagonal (10.) */
  double tau_kappa; /*!< inverse-norm threshold (100.) */
  int    alpha_L;   /*!< growth factor of nnz per col (4) */
  int    alpha_U;   /*!< growth factor of nnz per row (4) */
  double rho;       /*!< density threshold for dense LU (0.25) */
  double c_d;       /*!< size parameter for dense LU (1.0) */
  double c_h;       /*!< size parameter for H-version (2.0) */
  int    N;         /*!< reference size of matrix (-1, system size) */
  int    verbose;   /*!< message output level (1, i.e. info) */
};

/*!
 * \typedef psmilu_Options
 * \brief type wrapper
 */
typedef struct psmilu_Options psmilu_Options;

/*!
 * \brief get the default controls
 * \note See the values of attributes in parentheses
 */
static psmilu_Options psmilu_get_default_options(void) {
  return (psmilu_Options){.tau_L     = 0.01,
                          .tau_U     = 0.01,
                          .tau_d     = 10.0,
                          .tau_kappa = 100.0,
                          .alpha_L   = 4,
                          .alpha_U   = 4,
                          .rho       = 0.25,
                          .c_d       = 1.0,
                          .c_h       = 2.0,
                          .N         = -1,
                          .verbose   = PSMILU_VERBOSE_INFO};
}

/*!
 * @}
 */ /* c interface group */

#ifdef __cplusplus
}

/* C++ interface */
#  include <string>
#  include <unordered_map>

namespace psmilu {

/*!
 * \brief enum wrapper
 * \note The prefix of \a PSMILU will be dropped
 * \ingroup cpp
 */
enum : int {
  VERBOSE_NONE = ::PSMILU_VERBOSE_NONE, /*!< mute */
  VERBOSE_INFO = ::PSMILU_VERBOSE_INFO, /*!< general information */
  VERBOSE_PRE  = ::PSMILU_VERBOSE_PRE,  /*!< preprocessing */
  VERBOSE_FAC  = ::PSMILU_VERBOSE_FAC,  /*!< factorization update */
  VERBOSE_MEM  = ::PSMILU_VERBOSE_MEM,  /*!< memory debug */
};

/*!
 * \typedef Options
 * \brief type wrapper
 * \ingroup cpp
 */
typedef psmilu_Options Options;

/*!
 * \brief get the default configuration
 * \ingroup cpp
 */
inline Options get_default_options() { return ::psmilu_get_default_options(); }

/*!
 * \brief represent an option control with C++ string
 * \param[in] opt input option controls
 * \return string representation of \a opt
 * \ingroup cpp
 */
inline std::string opt_repr(const Options &opt) {
  using std::string;
  using std::to_string;               /* C++11 */
  const static int leading_size = 30; /* should be enough */
  const auto       pack_int     = [](const string &cat, const int v) -> string {
    return cat + string(leading_size - cat.size(), ' ') + to_string(v) + "\n";
  };
  const auto pack_double = [](const string &cat, const double v) -> string {
    return cat + string(leading_size - cat.size(), ' ') + to_string(v) + "\n";
  };
  return pack_double("tau_L", opt.tau_L) + pack_double("tau_U", opt.tau_U) +
         pack_double("tau_d", opt.tau_d) +
         pack_double("tau_kappa", opt.tau_kappa) +
         pack_int("alpha_L", opt.alpha_L) + pack_int("alpha_U", opt.alpha_U) +
         pack_double("rho", opt.rho) + pack_double("c_d", opt.c_d) +
         pack_double("c_h", opt.c_h) + pack_int("N", opt.N) +
         pack_int("verbose", opt.verbose);
}

#  ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace internal {
/*
 * build a byte map, i.e. the value is the leading byte position of the attrs
 * in Options
 */
constexpr static std::size_t option_attr_pos[11] = {
    0,
    sizeof(double),
    option_attr_pos[1] + sizeof(double),
    option_attr_pos[2] + sizeof(double),
    option_attr_pos[3] + sizeof(double),
    option_attr_pos[4] + sizeof(int),
    option_attr_pos[5] + sizeof(int),
    option_attr_pos[6] + sizeof(double),
    option_attr_pos[7] + sizeof(double),
    option_attr_pos[8] + sizeof(double),
    option_attr_pos[9] + sizeof(int),
};

/* data type tags, true for double, false for int */
constexpr static bool option_dtypes[11] = {
    true, true, true, true, false, false, true, true, true, false, false};

/* using unordered map to store the string to index map */
const static std::unordered_map<std::string, int> option_tag2pos = {
    {"tau_L", 0},   {"tau_U", 1},   {"tau_d", 2},   {"tau_kappa", 3},
    {"alpha_L", 4}, {"alpha_U", 5}, {"rho", 6},     {"c_d", 7},
    {"c_h", 8},     {"N", 9},       {"verbose", 10}};

} /* namespace internal */
#  endif /* DOXYGEN_SHOULD_SKIP_THIS */

/// \brief set \ref Options attribute value from key value pairs
/// \tparam T value type, either \a double or \a int
/// \param[in] attr attribute/member name
/// \param[in] v value
/// \param[out] opt output options
/// \ingroup cpp
///
/// This function can be handy while initialing option parameters from string
/// values. Notice that the keys (string values) are the same as the attribute
/// variable names.
template <typename T>
inline bool set_option_attr(const std::string &attr, const T v, Options &opt) {
  constexpr static bool failed  = true;
  char *                opt_raw = reinterpret_cast<char *>(&opt);
  try {
    const int         pos     = internal::option_tag2pos.at(attr);
    const std::size_t pos_raw = internal::option_attr_pos[pos];
    const bool        dtype   = internal::option_dtypes[pos];
    opt_raw += pos_raw;
    if (dtype)
      *reinterpret_cast<double *>(opt_raw) = v;
    else
      *reinterpret_cast<int *>(opt_raw) = v;
    return !failed;
  } catch (const std::out_of_range &) {
    return failed;
  }
}

} /* namespace psmilu */

/*!
 * \def psmilu_verbose2(__LVL, __opt_tag)
 * \brief return \a true if certain verbose level is defined
 * \note __LVL must be upper case and align with the enumerators
 * \note This macro is for algorithm implementation thus available only in C++
 * \ingroup util
 */
#  define psmilu_verbose2(__LVL, __opt_tag) \
    (__opt_tag & ::psmilu::VERBOSE_##__LVL)

/*!
 * \def psmilu_verbose(__LVL, __opt)
 * \brief return \a true if certain verbose level is defined
 * \note __LVL must be upper case and align with the enumerators
 * \note This macro is for algorithm implementation thus available only in C++
 * \ingroup util
 *
 * \code{.cpp}
 * if (psmilu_verbose(INFO, opt)) ...;
 * \endcode
 */
#  define psmilu_verbose(__LVL, __opt) psmilu_verbose2(__LVL, __opt.verbose)

#endif /* __cplusplus */

#endif /* _PSMILU_OPTIONS_H */
