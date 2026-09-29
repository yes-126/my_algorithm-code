"""results.csv / pivot.csv를 읽는 공통 코드."""
import csv
import math
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REPORT = os.path.join(ROOT, "report")

ALGOS = ["insertionSort", "quickSort", "heapSort"]
SHAPES = ["random", "sorted", "reversed", "nearly", "dups"]
SHAPE_KO = {"random": "무작위", "sorted": "정렬됨", "reversed": "역순",
            "nearly": "거의 정렬", "dups": "중복많음"}
PIVOTS = ["last", "median3", "random"]
PIVOT_KO = {"last": "마지막 원소", "median3": "중앙값(3점)", "random": "무작위"}


def read_results():
    with open(os.path.join(REPORT, "results.csv"), encoding="utf-8") as f:
        rows = list(csv.DictReader(f))
    for r in rows:
        r["n"] = int(r["n"])
        r["ms"] = float(r["ms"])
        r["compares"] = int(r["compares"])
        r["moves"] = int(r["moves"])
        r["maxDepth"] = int(r["maxDepth"])
        r["extraBytes"] = int(r["extraBytes"])
        r["stable"] = int(r["stable"])
    return rows


def read_pivot():
    with open(os.path.join(REPORT, "pivot.csv"), encoding="utf-8") as f:
        rows = list(csv.DictReader(f))
    for r in rows:
        for k in ("n", "compares", "moves", "maxDepth"):
            r[k] = int(r[k])
        r["ms"] = float(r["ms"])
    return rows


def pick(rows, **kw):
    for r in rows:
        if all(r[k] == v for k, v in kw.items()):
            return r
    raise KeyError(kw)


def slope(xs, ys):
    """로그-로그 축에서의 최소제곱 기울기 = 복잡도 지수."""
    lx = [math.log(x) for x in xs]
    ly = [math.log(y) for y in ys]
    mx, my = sum(lx) / len(lx), sum(ly) / len(ly)
    return sum((a - mx) * (b - my) for a, b in zip(lx, ly)) / sum((a - mx) ** 2 for a in lx)
