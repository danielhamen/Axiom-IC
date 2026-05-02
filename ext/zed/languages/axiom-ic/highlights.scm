(comment) @comment

(directive) @keyword.directive

(function_directive
  ".fn" @keyword.directive
  (identifier) @function)

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
(unary_operation) @function.builtin
(binary_operation) @function.builtin
(ternary_operation) @function.builtin
