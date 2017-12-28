""" --- Day 6: Memory Reallocation ---
A debugger program here is having an issue: it is trying to repair a memory reallocation routine, but it keeps getting stuck in an infinite loop.

In this area, there are sixteen memory banks; each memory bank can hold any number of blocks. The goal of the reallocation routine is to balance the blocks between the memory banks.

The reallocation routine operates in cycles. In each cycle, it finds the memory bank with the most blocks (ties won by the lowest-numbered memory bank) and redistributes those blocks among the banks. To do this, it removes all of the blocks from the selected bank, then moves to the next (by index) memory bank and inserts one of the blocks. It continues doing this until it runs out of blocks; if it reaches the last memory bank, it wraps around to the first one.

The debugger would like to know how many redistributions can be done before a blocks-in-banks configuration is produced that has been seen before.

For example, imagine a scenario with only four memory banks:

   * The banks start with 0, 2, 7, and 0 blocks. The third bank has the most blocks, so it is chosen for redistribution.
   * Starting with the next bank (the fourth bank) and then continuing to the first bank, the second bank, and so on, the 7 blocks are spread out over the memory banks. The fourth, first, and second banks get two blocks each, and the third bank gets one back. The final result looks like this: 2 4 1 2.
   * Next, the second bank is chosen because it contains the most blocks (four). Because there are four memory banks, each gets one block. The result is: 3 1 2 3.
   * Now, there is a tie between the first and fourth memory banks, both of which have three blocks. The first bank wins the tie, and its three blocks are distributed evenly over the other three banks, leaving it with none: 0 2 3 4.
   * The fourth bank is chosen, and its four blocks are distributed such that each of the four banks receives one: 1 3 4 1.
   * The third bank is chosen, and the same thing happens: 2 4 1 2.

At this point, we've reached a state we've seen before: 2 4 1 2 was already seen. The infinite loop is detected after the fifth block redistribution cycle, and so the answer in this example is 5.

Given the initial block counts in your puzzle input, how many redistribution cycles must be completed before a configuration is produced that has been seen before?

--- Part Two ---
Out of curiosity, the debugger would also like to know the size of the loop: starting from a state that has already been seen, how many block redistribution cycles must be performed before that same state is seen again?

In the example above, 2 4 1 2 is seen again after four cycles, and so the answer in that example would be 4.

How many cycles are in the infinite loop that arises from the configuration in your puzzle input?
"""

import argparse
import copy


class Solver:
    def __init__(self, part, input_file):
        self.__inf = input_file
        self.__setups = set()
        self.__current = None
        if part == '1':
            self.__part1()
        else:
            self.__part2()

    def __repeated(self):
        if self.__current in self.__setups:
            for e in self.__setups:
                if e == self.__current:
                    self.__origin = e
                    break
            return True
        s = copy.deepcopy(self.__current)
        self.__setups.discard(s)
        self.__setups.add(s)
        #print(s)
        return False

    def __balance(self):
        setup = self.__current
        mxi, mxv = setup.get_max()
        #print("{}:{} {}".format(setup, mxi, mxv))
        setup.banks[mxi] = 0
        while mxv > 0:
            mxi = (mxi + 1) % self.__banks_count
            setup.banks[mxi] += 1
            mxv -= 1

    def __reallocation(self):
        print("reallocating...")
        while not self.__repeated():
            self.__balance()
            self.__current.cycle += 1
            if (self.__current.cycle % 1000) == 0:
                print("cycle: {}".format(self.__current.cycle))

    def __gen_proc(self):
        with open(self.__inf) as f:
            banks = []
            for line in f:
                for blocks in line.split():
                    banks.append(int(blocks))
            self.__current = Solver.Setup(banks)
            self.__banks_count = self.__current.get_size()
        self.__reallocation()

    def __part1(self):
        self.__gen_proc()
        print("Given the initial block counts in your puzzle input, how many redistribution cycles must be completed before a configuration is produced that has been seen before? {}".format(self.__current.cycle))

    def __part2(self):
        self.__gen_proc()
        #print("{} vs {}".format(self.__current, self.__origin))
        print("How many cycles are in the infinite loop that arises from the configuration in your puzzle input? {}".format(self.__current.cycle - self.__origin.cycle))

    class Setup:
        def __init__(self, banks, cycle=0):
            self.banks = copy.deepcopy(banks)
            self.cycle = cycle

        def __hash__(self):
            return hash(tuple(self.banks))

        def __eq__(self, other):
            if isinstance(self, other.__class__):
                return self.banks == other.banks
            return False

        def __ne__(self, other):
            return not(self == other)

        def __str__(self):
            return "[{}:{}]".format(self.banks, self.cycle)

        def get_max(self):
            mi = 0
            mv = 0
            for i, val in enumerate(self.banks):
                if val > mv:
                    mi = i
                    mv = val
            return (mi, mv)

        def get_size(self):
            return len(self.banks)

def main():
   parser = argparse.ArgumentParser(description='Solve Level 5')
   parser.add_argument("-i", "--input", help="input file", required=True)
   parser.add_argument("-p", "--part", help="solve part 1 or 2", choices=['1', '2'], default='1')
   args = parser.parse_args()
   Solver(args.part, args.input)


if "__main__" == __name__:
    main()
