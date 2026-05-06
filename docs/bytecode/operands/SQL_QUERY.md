# `SQL_QUERY` Instruction

Queries AIC row data with a SQL string.

## Category

- `sql`

## Syntax

```
SQL_QUERY <dst>, <rows>, <query>
```

`<query>` is normally a SQL string literal such as `#sql"SELECT * WHERE name = 'Ada'"`.

## Behavior

Supports `SELECT *` and `SELECT * WHERE field = value` over a list of map rows. Writes the matching rows as a list.
