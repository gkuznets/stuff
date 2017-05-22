file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.hpp)

set(CLANG_TIDY_CHECKS
    "google-default-arguments,\
    google-explicit-constructor,\
    misc-assert-side-effect,\
    misc-dangling-handle,\
    misc-definitions-in-headers,\
    misc-fold-init-type,\
    misc-forwarding-reference-overload,\
    misc-inaccurate-erase,\
    misc-incorrect-roundings,\
    misc-inefficient-algorithm,\
    misc-macro-parentheses,\
    misc-macro-repeated-side-effects,\
    misc-misplaced-const,\
    misc-misplaced-widening-cast,\
    misc-move-const-arg,\
    misc-move-constructor-init,\
    misc-move-forwarding-reference,\
    misc-multiple-statement-macro,\
    misc-new-delete-overloads,\
    misc-noexcept-move-constructor,\
    misc-non-copyable-objects,\
    misc-redundant-expression,\
    misc-sizeof-expression,\
    misc-static-assert,\
    misc-string-compare,\
    misc-string-constructor,\
    misc-string-integer-assignment,\
    misc-string-literal-with-embedded-nul,\
    misc-suspicious-enum-usage,\
    misc-suspicious-missing-comma,\
    misc-suspicious-semicolon,\
    misc-suspicious-string-compare,\
    misc-swapped-arguments,\
    misc-throw-by-value-catch-by-reference,\
    misc-unconventional-assign-operator,\
    misc-undelegated-constructor,\
    misc-unused-alias-decls,\
    misc-unused-parameters,\
    misc-unused-raii,\
    misc-unused-using-decls,\
    misc-virtual-near-miss,\
    modernize-loop-convert,\
    modernize-make-shared,\
    modernize-make-unique,\
    modernize-pass-by-value,\
    modernize-use-equals-default,\
    modernize-use-equals-delete,\
    modernize-return-braced-init-list,\
    modernize-use-default-member-init,\
    modernize-use-nullptr,\
    modernize-use-override,\
    modernize-use-transparent-functors,\
    modernize-use-using,\
    performance-faster-string-find,\
    performance-for-range-copy,\
    performance-implicit-cast-in-loop,\
    performance-inefficient-string-concatenation,\
    performance-inefficient-vector-operation,\
    performance-type-promotion-in-math-fn,\
    performance-unnecessary-copy-initialization,\
    performance-unnecessary-value-param,\
    readability-avoid-const-params-in-decls,\
    readability-braces-around-statements,\
    readability-container-size-empty,\
    readability-delete-null-pointer,\
    readability-deleted-default,\
    readability-else-after-return,\
    readability-implicit-bool-cast,\
    readability-misleading-indentation,\
    readability-misplaced-array-index,\
    readability-non-const-parameter,\
    readability-redundant-control-flow,\
    readability-redundant-declaration,\
    readability-redundant-function-ptr-dereference,\
    readability-redundant-member-init,\
    readability-redundant-smartptr-get,\
    readability-redundant-string-cstr,\
    readability-redundant-string-init,\
    readability-simplify-boolean-expr,\
    readability-static-definition-in-anonymous-namespace")

add_custom_target(
    tidy
    COMMAND run-clang-tidy.py -j 3 -header-filter=.* -checks ${CLANG_TIDY_CHECKS}
)
