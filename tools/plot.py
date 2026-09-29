"""report/results.csv, pivot.csv 로 report/ 아래에 그래프(SVG)를 그린다."""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from common import ALGOS, PIVOTS, PIVOT_KO, REPORT, SHAPES, SHAPE_KO, pick, read_pivot, read_results, slope
from svgchart import grouped_bar, line_chart


def out(name):
    return os.path.join(REPORT, name)


def main():
    rows = read_results()
    cats = [SHAPE_KO[s] for s in SHAPES]

    # 입력 모양별 (막대): 선형 축 + 로그 축
    for key, label, fname in (("ms", "걸린 시간(ms)", "time"), ("compares", "비교 횟수", "compares")):
        series = [(a, [pick(rows, section="shapes", shape=s, algorithm=a)[key] for s in SHAPES]) for a in ALGOS]
        n = pick(rows, section="shapes", shape="random", algorithm=ALGOS[0])["n"]
        grouped_bar(out(f"input-shapes-{fname}.svg"), f"입력 모양에 따른 {label} (n={n:,}, 선형 축)",
                    cats, series, label)
        grouped_bar(out(f"input-shapes-{fname}-log.svg"), f"입력 모양에 따른 {label} (n={n:,}, 로그 축)",
                    cats, series, label, log=True)

    # n을 키우며: random / sorted
    for section, ko in (("growth-random", "무작위"), ("growth-sorted", "정렬됨")):
        ns = sorted({r["n"] for r in rows if r["section"] == section})
        for key, label, fname in (("compares", "비교 횟수", "compares"), ("ms", "걸린 시간(ms)", "time")):
            series = [(a, [pick(rows, section=section, n=n, algorithm=a)[key] for n in ns]) for a in ALGOS]
            base = "growth" if section == "growth-random" else "growth-sorted"
            if section == "growth-random" and key == "compares":
                line_chart(out(f"{base}-{fname}.svg"), f"n이 커질 때 {label} ({ko}, 선형 축)",
                           ns, series, "n", label)
            line_chart(out(f"{base}-{fname}-log.svg"), f"n이 커질 때 {label} ({ko}, 로그-로그 축)",
                       ns, series, "n", label, logx=True, logy=True)
            print(f"[{section}] {label} 기울기:",
                  ", ".join(f"{a}={slope(ns, vs):.2f}" for a, vs in series))

    # 피벗 전략
    prow = read_pivot()
    series = [(PIVOT_KO[m], [pick(prow, shape=s, pivot=m)["compares"] for s in SHAPES]) for m in PIVOTS]
    n = prow[0]["n"]
    grouped_bar(out("pivot.svg"), f"퀵 정렬의 피벗 전략별 비교 횟수 (n={n:,}, 로그 축)",
                cats, series, "비교 횟수", log=True)
    print("SVG를 report/ 에 그렸다.")


if __name__ == "__main__":
    main()
