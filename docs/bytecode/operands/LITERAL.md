# LITERAL

`LITERAL <name>, <target>`

Creates a textual literal alias named `<name>` that points to `<target>`.

Requires `.import <literal>`.


## Behavior

The alias stores the target operand, not a copied value. Every later read of the alias rereads the target:

```aic
.import <literal>
.import <memory>
.import <io>
.import <control_flow>

.const 4 FLOAT 3.14159

.main
  LITERAL PI, @4
  PRINTLN PI
  HALT
```

If the alias points to a slot, the current slot value is read each time:

```aic
STORE $4, #10
LITERAL CURRENT, $4
PRINTLN CURRENT ; 10
STORE $4, #99
PRINTLN CURRENT ; 99
```

Aliases that point to slots can also be used as write destinations. The write updates the target slot.

