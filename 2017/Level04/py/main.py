""" --- Day 4: High-Entropy Passphrases ---
A new system policy has been put in place that requires all accounts to use a passphrase instead of simply a password. A passphrase consists of a series of words (lowercase letters) separated by spaces.

To ensure security, a valid passphrase must contain no duplicate words.

For example:
    
    aa bb cc dd ee is valid.
    aa bb cc dd aa is not valid - the word aa appears more than once.
    aa bb cc dd aaa is valid - aa and aaa count as different words.
    The system's full passphrase list is available as your puzzle input. How many passphrases are valid?

--- Part Two ---
For added security, yet another system policy has been put in place. Now, a valid passphrase must contain no two words that are anagrams of each other - that is, a passphrase is invalid if any word's letters can be rearranged to form any other word in the passphrase.

For example:

    abcde fghij is a valid passphrase.
    abcde xyz ecdab is not valid - the letters from the third word can be rearranged to form the first word.
    a ab abc abd abf abj is a valid passphrase, because all letters need to be used when forming another word.
    iiii oiii ooii oooi oooo is valid.
    oiii ioii iioi iiio is not valid - any of these words can be rearranged to form any other word.
    Under this new system policy, how many passphrases are valid?

"""

import argparse


class Solver:
    def __init__(self, part, input_file):
        self.__inf = input_file
        if part == '1':
            self.__part1()
        else:
            self.__part2()

    def __part1(self):
        with open(self.__inf) as f:
            valid = 0
            for passphrase in f:
                words = set()
                duplicate = False
                for w in passphrase.split():
                    if w in words:
                        duplicate = True
                        break
                    else:
                        words.add(w)
                if not duplicate:
                    valid += 1
            print("How many passphrases are valid? {}".format(valid))

    def __anagram(self, w, words):
        anagram = False
        for ow in words:
            if len(ow) == len(w):
                a = True
                owc = "".join(ow)
                for l in w:
                    parts = owc.partition(l)
                    if parts[0] == owc:
                        a = False
                        break
                    owc = "".join((parts[0], parts[2]))
                if a:
                    anagram = True
                    break
        return anagram

    def __part2(self):
        with open(self.__inf) as f:
            valid = 0
            for passphrase in f:
                words = set()
                contains_anagram = False
                for w in passphrase.split():
                    if self.__anagram(w, words):
                        contains_anagram = True
                        break
                    else:
                        words.add(w)
                if not contains_anagram:
                    valid += 1
            print("Under this new system policy, "
                  "how many passphrases are valid? {}".format(valid))


def main():
   parser = argparse.ArgumentParser(description='Solve Level 4.')
   parser.add_argument("-i", "--input", help="input file", required=True)
   parser.add_argument("-p", "--part", help="solve part 1 or 2", choices=['1', '2'], default='1')
   args = parser.parse_args()
   Solver(args.part, args.input)


if "__main__" == __name__:
    main()

