"""--- Day 7: Recursive Circus ---
Wandering further through the circuits of the computer, you come upon a tower of programs that have gotten themselves into a bit of trouble. A recursive algorithm has gotten out of hand, and now they're balanced precariously in a large tower.

One program at the bottom supports the entire tower. It's holding a large disc, and on the disc are balanced several more sub-towers. At the bottom of these sub-towers, standing on the bottom disc, are other programs, each holding their own disc, and so on. At the very tops of these sub-sub-sub-...-towers, many programs stand simply keeping the disc below them balanced but with no disc of their own.

You offer to help, but first you need to understand the structure of these towers. You ask each program to yell out their name, their weight, and (if they're holding a disc) the names of the programs immediately above them balancing on that disc. You write this information down (your puzzle input). Unfortunately, in their panic, they don't do this in an orderly fashion; by the time you're done, you're not sure which program gave which information.

For example, if your list is the following:

pbga (66)
xhth (57)
ebii (61)
havc (66)
ktlj (57)
fwft (72) -> ktlj, cntj, xhth
qoyq (66)
padx (45) -> pbga, havc, qoyq
tknk (41) -> ugml, padx, fwft
jptl (61)
ugml (68) -> gyxo, ebii, jptl
gyxo (61)
cntj (57)
...then you would be able to recreate the structure of the towers that looks like this:

                gyxo
              /     
         ugml - ebii
       /      \     
      |         jptl
      |        
      |         pbga
     /        /
tknk --- padx - havc
     \        \
      |         qoyq
      |             
      |         ktlj
       \      /     
         fwft - cntj
              \     
                xhth
In this example, tknk is at the bottom of the tower (the bottom program), and is holding up ugml, padx, and fwft. Those programs are, in turn, holding up other programs; in this example, none of those programs are holding up any other programs, and are all the tops of their own towers. (The actual tower balancing in front of you is much larger.)

Before you're ready to help them, you need to make sure your information is correct. What is the name of the bottom program? 

--- Part Two ---
The programs explain the situation: they can't get down. Rather, they could get down, if they weren't expending all of their energy trying to keep the tower balanced. Apparently, one program has the wrong weight, and until it's fixed, they're stuck here.

For any program holding a disc, each program standing on that disc forms a sub-tower. Each of those sub-towers are supposed to be the same weight, or the disc itself isn't balanced. The weight of a tower is the sum of the weights of the programs in that tower.

In the example above, this means that for ugml's disc to be balanced, gyxo, ebii, and jptl must all have the same weight, and they do: 61.

However, for tknk to be balanced, each of the programs standing on its disc and all programs above it must each match. This means that the following sums must all be the same:

ugml + (gyxo + ebii + jptl) = 68 + (61 + 61 + 61) = 251
padx + (pbga + havc + qoyq) = 45 + (66 + 66 + 66) = 243
fwft + (ktlj + cntj + xhth) = 72 + (57 + 57 + 57) = 243
As you can see, tknk's disc is unbalanced: ugml's stack is heavier than the other two. Even though the nodes above ugml are balanced, ugml itself is too heavy: it needs to be 8 units lighter for its stack to weigh 243 and keep the towers balanced. If this change were made, its weight would be 60.

Given that exactly one program is the wrong weight, what would its weight need to be to balance the entire tower?
"""

import argparse
import re

class Towers:
    def __init__(self, part, input_path):
        self.__build_tree(input_path)
        if part == 1:
            self.__solve_part1()
        else:
            self.__solve_part2()

    def __build_tree(self, input_path):
        with open(input_path) as f:
            self.__tree = {}
            parser = re.compile(r"^([a-z]+) \((\d+)\)(?: -> ([a-z ,]+))?")
            for l in f:
                m = parser.search(l)
                if not m:
                    raise RuntimeError("Unexpected line: '{}'".format(l))
                name = m.group(1)
                weight = int(m.group(2))
                children = None
                if m.group(3):
                    children = m.group(3).split(", ")
                self.__add_node(name, weight, children)
           
    def __add_node(self, name, weight, children=None):
        if name not in self.__tree:
            self.__tree[name] = Towers.Node(name, weight)
        else:
            self.__tree[name].weight = weight
        self.__tree[name].children = []
        if children is not None:
            parent = self.__tree[name]
            for c in children:
                if c not in self.__tree:
                    self.__tree[c] = Towers.Node(c, -1, parent)
                else:
                    self.__tree[c].parent = parent
                parent.children.append(self.__tree[c])

    def __rooter(self, node):
        if node.parent is None:
            return node
        return self.__rooter(node.parent)

    def __get_root(self):
        if not self.__tree:
            raise RuntimeError("Empty tree!")
        random_name = list(self.__tree)[0]
        return self.__rooter(self.__tree[random_name])

    def __solve_part1(self):
        root = self.__get_root()
        print("What is the name of the bottom program? {}".format(root.name))
    
    def __get_tower_weight(self, node):
        weight = node.weight
        for n in node.children:
            weight += self.__get_tower_weight(n)
        return weight

    def __find_correct_weight(self, node, level=0, diff=0):
        print(level*' ' + str(node))
        weights = []
        for c in node.children:
            weights.append((c, self.__get_tower_weight(c)))
        for c, w in weights:
            print(c, w)

    def __get_weight_correction(self):
        weight = 0
        root = self.__get_root()
        self.__find_correct_weight(root)
        return weight
    
    def __solve_part2(self):
        weight = self.__get_weight_correction()
        print("Given that exactly one program is the wrong weight, "
              "what would its weight need to be to balance the entire tower? {}".format(weight))
    
    class Node:
        def __init__(self, name, weight, parent=None):
            self.name = name
            self.weight = weight
            self.parent = parent

        def __str__(self):
            #return "[({}, {})->{}]".format(self.name, self.weight, self.parent)
            return "[{}, {}]".format(self.name, self.weight)


def main():
   parser = argparse.ArgumentParser(description='Solve Level 7')
   parser.add_argument("-i", "--input", help="input file", required=True)
   parser.add_argument("-p", "--part", help="solve part 1 or 2", choices=['1', '2'], default='1')
   args = parser.parse_args()
   Towers(int(args.part), args.input)


if "__main__" == __name__:
    main()
