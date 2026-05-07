# Function Overloads

Functions can share the same name when they declare parameter metadata with `PARAM` or `PARAM_DEFAULT`.

```aic
.fn round
  PARAM_DEFAULT #0, #~"Float", #3.0
  ARG_GET_DEFAULT $0, #0, #3.0
  ROUND $1, $0
  RET $1

.fn round
  PARAM_DEFAULT #0, #~"Float", #3.0
  PARAM #1, #~"Integer"
  ARG_GET_DEFAULT $0, #0, #3.0
  ARG_GET $1, #1
  ROUND $2, $0
  RET $2
```

Overload resolution uses pending `ARG` values. The selected overload must accept the supplied argument count and every supplied argument must match its declared type metadata.

When multiple overloads match, the VM prefers exact argument-count matches and more specific signatures. Equally specific matches raise an ambiguity error.
