# coding: utf-8
import operator
from functools import reduce

arr = [1, 1, 2, 2, 3, 4, 5, 6, 5, 4, 3]
print(reduce(operator.xor, arr))
