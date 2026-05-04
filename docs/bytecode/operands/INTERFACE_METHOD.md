# `INTERFACE_METHOD` Instruction

Adds a required method to an interface.

## Category

- `interface`

## Syntax

```aic
INTERFACE_METHOD <interface>, <method>
```

## Behavior

Adds the string method name `<method>` to the interface requirement list.

## Errors

Raises an error if `<interface>` is not an interface or `<method>` is not a string.
