# ERR_GET

## Syntax

`ERR_GET <dst>`

## Behavior

Writes the current handled error to `<dst>`. If no error is active, writes `null`.

Requires `.import <exception>`.
