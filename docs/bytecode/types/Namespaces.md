# Namespaces

Namespaces group values and function bindings under dot-separated paths.

```aic
NAMESPACE_NEW $0, #~"System"
NAMESPACE_BIND_FN $0, #~"Out.printLn", system_out_println
NAMESPACE_CALL $1, $0, #~"Out.printLn"
```

Use namespaces for static API paths such as `System.Out.printLn`, `Math.sin`, or `Json.parse`. Use structs for data with instance state.

Namespace value members and function bindings are separate. `NAMESPACE_GET` reads value members, while `NAMESPACE_CALL` invokes function bindings.
