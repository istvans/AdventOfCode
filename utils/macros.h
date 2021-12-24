/// A collection of useful macros.

#define CLANG_DIAG_IGNORE_BOOST_WARNINGS() \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Wcomma\"") \
    _Pragma("clang diagnostic ignored \"-Wdeprecated-dynamic-exception-spec\"") \
    _Pragma("clang diagnostic ignored \"-Wdisabled-macro-expansion\"") \
    _Pragma("clang diagnostic ignored \"-Wdocumentation-unknown-command\"") \
    _Pragma("clang diagnostic ignored \"-Wdocumentation\"") \
    _Pragma("clang diagnostic ignored \"-Wdouble-promotion\"") \
    _Pragma("clang diagnostic ignored \"-Wextra-semi-stmt\"") \
    _Pragma("clang diagnostic ignored \"-Wglobal-constructors\"") \
    _Pragma("clang diagnostic ignored \"-Wold-style-cast\"") \
    _Pragma("clang diagnostic ignored \"-Wpadded\"") \
    _Pragma("clang diagnostic ignored \"-Wredundant-parens\"") \
    _Pragma("clang diagnostic ignored \"-Wreserved-identifier\"") \
    _Pragma("clang diagnostic ignored \"-Wreserved-macro-identifier\"") \
    _Pragma("clang diagnostic ignored \"-Wshadow-field\"") \
    _Pragma("clang diagnostic ignored \"-Wsuggest-destructor-override\"") \
    _Pragma("clang diagnostic ignored \"-Wsuggest-override\"") \
    _Pragma("clang diagnostic ignored \"-Wundef\"") \
    _Pragma("clang diagnostic ignored \"-Wweak-vtables\"") \
    _Pragma("clang diagnostic ignored \"-Wzero-as-null-pointer-constant\"")

#define CLANG_DIAG_POP() \
    _Pragma("clang diagnostic pop")

#define NUM_ELEMENTS_OF_ARRAY(arr_) (sizeof((arr_))/sizeof((arr_)[0]))
