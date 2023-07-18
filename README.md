# Applications

- Exact Cover
- Langford Pairs

# TODO

- exercise 3: MRV heuristic
- Progress Report. Q: how to measure memory access?
- Langford Pairs factor 2
- Queens by secondary
- Sudoku


# Exercise 8

8. Design an algorithm to set up the initial memory contents of an exact cover problem, as needed by Algorithm X and illustrated in Table 1. The input to your algorithm should consist of a sequence of lines with the following format:
• The very first line lists the names of all items.
• Each remaining line specifies the items of a particular option, one option per line.

## Format of input_ext8.out.exp
- 1st line lists `i`
- 2nd line lists `NAME(i)`
- 3rd line lists `LLINK(i)`
- 4th line lists `RLINK(8)`
- 5th line lists `x`
- 6th line lists `LEN(x), for x in i-s; TOP(x) otherwise `
- 7th line lists `ULINK(x)`
- 8th line lists `DLINK(x)`

