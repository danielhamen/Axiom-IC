# TRY

`TRY <catch_label>`

Installs an exception handler for the current function. If a runtime error or `THROW` occurs before `END_TRY`, execution jumps to `<catch_label>`.

Requires `.import <exception>`.

