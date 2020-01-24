"""
load tests for join_server (c++ OTUS Lesson 25)
"""

from random import randint
import sys

INSERT_COUNT = 5
NAMES_COUNT = 10
NAMES = ["one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"]
ins_idx = 0

def generate_insert(table_name):
    i = 0
    while i < INSERT_COUNT:
        yield "INSERT " + table_name + " " + str(i) + " " + NAMES[randint(0, NAMES_COUNT-1)]
        i += 1

def generate_intersection():
    return "INTERSECTION"

def generate_sym_diff():
    return "SYMMETRIC_DIFFERENCE"


if __name__ == "__main__":
    if len(sys.argv) > 1:
        INSERT_COUNT = int(sys.argv[1])

    ins_gen_a = generate_insert("A")
    ins_gen_b = generate_insert("B")
    for req in ins_gen_a:
        print(req)
    for req in ins_gen_b:
        print(req)
    #print(generate_intersection())

