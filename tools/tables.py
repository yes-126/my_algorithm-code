"""results.csv / pivot.csv 로 보고서에 붙일 마크다운 표를 찍는다. (make tables)"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from common import ALGOS, PIVOTS, PIVOT_KO, SHAPES, SHAPE_KO, pick, read_pivot, read_results, slope


def shapes_table(rows):
    print("### 입력 모양별\n")
    print("| 입력 | 알고리즘 | 시간(ms) | 비교 | 이동 | 재귀깊이 |")
    print("| --- | --- | ---: | ---: | ---: | ---: |")
    for s in SHAPES:
        for a in ALGOS:
            r = pick(rows, section="shapes", shape=s, algorithm=a)
            print(f"| {SHAPE_KO[s]} | {a} | {r['ms']:,.3f} | {r['compares']:,} | {r['moves']:,} | {r['maxDepth']} |")


def growth_table(rows, section):
    ns = sorted({r["n"] for r in rows if r["section"] == section})
    print(f"\n### {section}: 비교 횟수 (괄호는 n이 2배일 때의 배수)\n")
    print("| n | insertion | quick | heap |")
    print("| ---: | ---: | ---: | ---: |")
    prev = {}
    for n in ns:
        cells = []
        for a in ALGOS:
            c = pick(rows, section=section, n=n, algorithm=a)["compares"]
            cells.append(f"{c:,}" + (f" (×{c / prev[a]:.2f})" if a in prev else ""))
            prev[a] = c
        print(f"| {n:,} | " + " | ".join(cells) + " |")
    print(f"\n### {section}: 시간(ms)\n")
    print("| n | insertion | quick | heap |")
    print("| ---: | ---: | ---: | ---: |")
    for n in ns:
        print(f"| {n:,} | " + " | ".join(f"{pick(rows, section=section, n=n, algorithm=a)['ms']:,.3f}" for a in ALGOS) + " |")
    print("\n기울기(로그-로그): " + ", ".join(
        f"{a} 비교 {slope(ns, [pick(rows, section=section, n=n, algorithm=a)['compares'] for n in ns]):.2f}"
        f" / 시간 {slope(ns, [pick(rows, section=section, n=n, algorithm=a)['ms'] for n in ns]):.2f}" for a in ALGOS))


def pivot_table(prow):
    print("\n### 퀵 정렬 피벗 전략 (비교 횟수 / 시간ms)\n")
    print("| 입력 | " + " | ".join(PIVOT_KO[m] for m in PIVOTS) + " |")
    print("| --- | " + " | ".join("---:" for _ in PIVOTS) + " |")
    for s in SHAPES:
        cells = []
        for m in PIVOTS:
            r = pick(prow, shape=s, pivot=m)
            cells.append(f"{r['compares']:,} / {r['ms']:,.2f}")
        print(f"| {SHAPE_KO[s]} | " + " | ".join(cells) + " |")


def memory_table(rows):
    print("\n### 메모리와 안정성\n")
    print("| 알고리즘 | 추가 메모리 | 재귀 깊이 (n=32,000 무작위) | 안정성(실측, 중복많음) |")
    print("| --- | ---: | ---: | :---: |")
    for a in ALGOS:
        g = pick(rows, section="growth-random", n=32000, algorithm=a)
        d = pick(rows, section="shapes", shape="dups", algorithm=a)
        print(f"| {a} | {g['extraBytes']} B | {g['maxDepth']} | {'안정' if d['stable'] else '불안정'} |")


if __name__ == "__main__":
    rows = read_results()
    shapes_table(rows)
    growth_table(rows, "growth-random")
    growth_table(rows, "growth-sorted")
    pivot_table(read_pivot())
    memory_table(rows)
