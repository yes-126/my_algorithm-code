"""실행: make run-py"""

from sort import bubble_sort

if __name__ == "__main__":
    a = [6, 8, 5, 9, 10, 1, 7, 2, 4, 3]
    bubble_sort(a)
    print("sorted:", " ".join(str(x) for x in a))
