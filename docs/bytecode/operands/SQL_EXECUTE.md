# `SQL_EXECUTE` Instruction

Executes a SQL-style query over AIC row data.

## Category

- `sql`

## Syntax

```
SQL_EXECUTE <dst>, <rows>, <query>
```

## Behavior

Currently behaves like `SQL_QUERY` and returns matching rows for `SELECT *` queries.
