# Wordle solver

## Build & run

```bash=
cd wordle-solver
g++ solver.cpp  -std=c++11 && ./a.out
```

## Run
```bash=
solver help
solver (one time, no input)
solver --input weary,--yg-,pills,----- (one time)
solver --input weary,--yg-,pills,----- --verbose (one time, verbose)
solver --interactive
```
weary

## Interactive
```
... solver --interactive
>>> lares
... y-yg-
>>> flour
... -gy-g
>>> ulcer
... next  (next to start a new round)
>>> lares
... -yyy-
>>> trade
... -yg-y
>>> yourn
... y--g-
>>> weary
... exit
```