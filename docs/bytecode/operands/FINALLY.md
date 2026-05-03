# FINALLY

`FINALLY <label>`

Attaches a finally target to the active `TRY` handler. If an error is thrown while the handler is active, execution jumps to `<label>` with the current error available via `ERR_GET`.

Requires `.import <exception>`.

