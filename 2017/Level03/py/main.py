"""--- Day 3: Spiral Memory ---

You come across an experimental new kind of memory stored on an infinite
 two-dimensional grid.

Each square on the grid is allocated in a spiral pattern starting at a location
marked 1 and then counting up while spiraling outward. For example, the first
few squares are allocated like this:

17  16  15  14  13
18   5   4   3  12
19   6   1   2  11
20   7   8   9  10
21  22  23---> ...
While this is very space-efficient (no squares are skipped), requested data must
 be carried back to square 1 (the location of the only access port for this
 memory system) by programs that can only move up, down, left, or right. They
 always take the shortest path: the Manhattan Distance between the location of
 the data and square 1.

For example:
-   Data from square 1 is carried 0 steps, since it's at the access port.
-   Data from square 12 is carried 3 steps, such as: down, left, left.
-   Data from square 23 is carried only 2 steps: up twice.
-   Data from square 1024 must be carried 31 steps.
How many steps are required to carry the data from the square identified in your
 puzzle input all the way to the access port?

Your puzzle input is 361527.

--- Part Two ---

As a stress test on the system, the programs here clear the grid and then store
the value 1 in square 1. Then, in the same allocation order as shown above, they
 store the sum of the values in all adjacent squares, including diagonals.

So, the first few squares' values are chosen as follows:
-   Square 1 starts with the value 1.
-   Square 2 has only one adjacent filled square (with value 1), so it also
    stores 1.
-   Square 3 has both of the above squares as neighbors and stores the sum of
    their values, 2.
-   Square 4 has all three of the aforementioned squares as neighbors and stores
    the sum of their values, 4.
-   Square 5 only has the first and fourth squares as neighbors, so it gets the
    value 5.
Once a square is written, its value does not change. Therefore, the first few
squares would receive the following values:

147  142  133  122   59
304    5    4    2   57
330   10    1    1   54
351   11   23   25   26
362  747  806--->   ...

What is the first value written that is larger than your puzzle input?

Your puzzle input is still 361527.
"""


def calc_steps(square):
    """
    65  64  63  62  61  60  59  58  57
    66  37  36  35  34  33  32  31  56
    67  38  17  16  15  14  13  30  55
    68  39  18   5   4   3  12  29  54
    69  40  19   6   1   2  11  28  53   1   9  17  25
    70  41  20   7   8   9  10  27  52     8   8   8
    71  42  21  22  23  24  25  26  51
    72  43  44  45  46  47  48  49  50
    73  74  75  76  77  78  79  80  81
                     7
                        8
                     15
                        8
                     23
                        8
                     31
                     1   8  16  24  32  40  48
                     1  2-9 10-25 26-49 50-81
                     0  1   2     3     4
    1 -> 0
        2 -> 1
            3 -> 2
        4 -> 1
            5 -> 2
        6 -> 1
            7 -> 2
        8 -> 1
            9 -> 2
                10 -> 3
            11 -> 2
                12 -> 3
                    13 -> 4
                14 -> 3
            15 -> 2
                16 -> 3
                    17 -> 4
                18 -> 3
            19 -> 2
                20 -> 3
                    21 -> 4
                22 -> 3
            23 -> 2
                24 -> 3
                    25 -> 4
                        26 -> 5
                    27 -> 4
                28 -> 3
                    29 -> 4
                        30 -> 5
                            31 -> 6
                        32 -> 5
                    33 -> 4
                34 -> 3
                    35 -> 4
                        36 -> 5
                            37 -> 6
                        38 -> 5
                    39 -> 4
                40 -> 3
                    41 -> 4
                        42 -> 5
                            43 -> 6
                        44 -> 5
                    45 -> 4
                46 -> 3
                    47 -> 4
                        48 -> 5
                            49 -> 6
                                50 -> 7
                            51 -> 6
                        52 -> 5
    """
    layer = 0
    layer_step = 8
    layer_max = 1
    while layer_max < square:
        layer += 1
        layer_max += (layer * layer_step)
    gate4 = layer_max - layer
    gate3 = gate4 - 2 * layer
    gate2 = gate3 - 2 * layer
    gate1 = gate2 - 2 * layer

    if square >= gate4:
        return square - gate4 + layer
    if square >= gate3:
        return min((square - gate3), (gate4 - square)) + layer
    if square >= gate2:
        return min((square - gate2), (gate3 - square)) + layer
    if square >= gate1:
        return min((square - gate1), (gate2 - square)) + layer
    return gate1 - square + layer


def main():
    square = 361527
    print(square,"->", calc_steps(square))


if __name__ == "__main__":
    main()
