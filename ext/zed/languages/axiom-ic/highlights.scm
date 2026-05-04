(comment) @comment

(directive) @keyword.directive

(import_directive
  ".import" @keyword.directive
  "<" @punctuation.bracket
  (identifier) @namespace
  ">" @punctuation.bracket)

(module_directive
  ".module" @keyword.directive
  (identifier) @namespace)

(export_directive
  ".export" @keyword.directive
  (identifier) @constant)

(function_directive
  ".fn" @keyword.directive
  (identifier) @function)

(module_path
  (identifier) @namespace)

(selective_import
  "{" @punctuation.bracket
  (identifier) @constant
  "}" @punctuation.bracket)

(label
  (identifier) @label)

(argument) @variable.parameter

(slot
  "$" @punctuation.special
  (integer) @variable.special)

(constant
  "@" @punctuation.special
  (integer) @constant)

(address
  "&" @punctuation.special
  (integer) @constant)

(immediate
  "#" @punctuation.special)

(string) @string
(integer) @number
(float) @number
(boolean) @boolean
(null) @constant.builtin

(nullary_operation) @function.builtin
(return_operation) @function.builtin
(unary_operation) @function.builtin
(binary_operation) @function.builtin
(ternary_operation) @function.builtin
(quaternary_operation) @function.builtin
(variadic_operation) @function.builtin
