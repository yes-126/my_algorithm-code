"""표준 모듈만으로 SVG 그래프(막대 · 선)를 그린다. matplotlib이 없어도 된다."""
import math

COLORS = ["#4e79a7", "#e15759", "#59a14f", "#f28e2b", "#b07aa1"]
FONT = "'Noto Sans KR','Malgun Gothic','Apple SD Gothic Neo',sans-serif"
W, H = 720, 420
ML, MR, MT, MB = 78, 24, 64, 64


def _esc(s):
    return str(s).replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


def _fmt(v):
    if v == 0:
        return "0"
    for lim, suf in ((1e9, "G"), (1e6, "M"), (1e3, "k")):
        if abs(v) >= lim:
            return f"{v / lim:.3g}{suf}"
    return f"{v:.3g}"


def _nice_ticks(vmax, count=5):
    if vmax <= 0:
        return [0, 1]
    raw = vmax / count
    mag = 10 ** math.floor(math.log10(raw))
    step = 10 * mag
    for m in (1, 2, 2.5, 5, 10):
        if raw <= m * mag:
            step = m * mag
            break
    ticks, v = [], 0.0
    while v < vmax + step * 0.999:
        ticks.append(v)
        v += step
        if ticks[-1] >= vmax:
            break
    return ticks


class _Canvas:
    def __init__(self, title, xlabel="", ylabel=""):
        self.parts = []
        self.pw, self.ph = W - ML - MR, H - MT - MB
        self.add(f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {W} {H}" width="{W}" height="{H}" '
                 f'font-family="{FONT}">')
        self.add(f'<rect width="{W}" height="{H}" fill="#ffffff"/>')
        self.add(f'<text x="{W / 2}" y="26" text-anchor="middle" font-size="16" font-weight="bold" '
                 f'fill="#222">{_esc(title)}</text>')
        if xlabel:
            self.add(f'<text x="{ML + self.pw / 2}" y="{H - 12}" text-anchor="middle" font-size="12" '
                     f'fill="#444">{_esc(xlabel)}</text>')
        if ylabel:
            self.add(f'<text transform="translate(16 {MT + self.ph / 2}) rotate(-90)" text-anchor="middle" '
                     f'font-size="12" fill="#444">{_esc(ylabel)}</text>')

    def add(self, s):
        self.parts.append(s)

    def legend(self, names):
        x = ML
        for i, name in enumerate(names):
            self.add(f'<rect x="{x}" y="38" width="12" height="12" fill="{COLORS[i % len(COLORS)]}"/>')
            self.add(f'<text x="{x + 17}" y="49" font-size="12" fill="#333">{_esc(name)}</text>')
            x += 26 + 7.2 * len(name)

    def hgrid(self, y, label):
        self.add(f'<line x1="{ML}" x2="{ML + self.pw}" y1="{y:.1f}" y2="{y:.1f}" stroke="#e3e3e3"/>')
        self.add(f'<text x="{ML - 8}" y="{y + 4:.1f}" text-anchor="end" font-size="11" fill="#555">{label}</text>')

    def frame(self):
        self.add(f'<line x1="{ML}" x2="{ML}" y1="{MT}" y2="{MT + self.ph}" stroke="#888"/>')
        self.add(f'<line x1="{ML}" x2="{ML + self.pw}" y1="{MT + self.ph}" y2="{MT + self.ph}" stroke="#888"/>')

    def save(self, path):
        self.add("</svg>")
        with open(path, "w", encoding="utf-8") as f:
            f.write("\n".join(self.parts))


def _scale(log, vmin, vmax):
    """값 -> 0(아래)~1(위) 비율을 돌려주는 함수와 눈금 목록을 만든다."""
    if log:
        lo = 10 ** math.floor(math.log10(vmin))
        hi = 10 ** math.ceil(math.log10(vmax))
        if hi == lo:
            hi = lo * 10
        a, b = math.log10(lo), math.log10(hi)
        ticks = [10 ** e for e in range(int(a), int(b) + 1)]
        return (lambda v: (math.log10(v) - a) / (b - a)), ticks, lo
    ticks = _nice_ticks(vmax)
    top = ticks[-1]
    return (lambda v: v / top), ticks, 0


def grouped_bar(path, title, categories, series, ylabel="", log=False):
    """categories: 묶음 이름들, series: [(이름, [묶음마다 값])]"""
    cv = _Canvas(title, ylabel=ylabel)
    vals = [v for _, vs in series for v in vs]
    pos = [v for v in vals if v > 0]
    f, ticks, base = _scale(log, min(pos) if log else 0, max(vals))
    cv.legend([n for n, _ in series])
    for t in ticks:
        if log or t >= 0:
            y = MT + cv.ph * (1 - f(t if (not log or t > 0) else 1))
            cv.hgrid(y, _fmt(t))
    cv.frame()
    gw = cv.pw / len(categories)
    bw = gw * 0.72 / len(series)
    for gi, cat in enumerate(categories):
        gx = ML + gi * gw + gw * 0.14
        cv.add(f'<text x="{ML + gi * gw + gw / 2:.1f}" y="{MT + cv.ph + 18}" text-anchor="middle" '
               f'font-size="12" fill="#333">{_esc(cat)}</text>')
        for si, (name, vs) in enumerate(series):
            v = vs[gi]
            x = gx + si * bw
            if v <= 0:
                cv.add(f'<text x="{x + bw / 2:.1f}" y="{MT + cv.ph - 4}" text-anchor="middle" '
                       f'font-size="9" fill="#777">0</text>')
                continue
            top = MT + cv.ph * (1 - f(v))
            cv.add(f'<rect x="{x:.1f}" y="{top:.1f}" width="{bw - 2:.1f}" height="{MT + cv.ph - top:.1f}" '
                   f'fill="{COLORS[si % len(COLORS)]}"/>')
            cv.add(f'<text x="{x + bw / 2 - 1:.1f}" y="{top - 3:.1f}" text-anchor="middle" '
                   f'font-size="9" fill="#333">{_fmt(v)}</text>')
    if log:
        cv.add(f'<text x="{W - MR}" y="{H - 12}" text-anchor="end" font-size="11" fill="#888">'
               f'로그 축 (눈금 한 칸 = 10배)</text>')
    cv.save(path)


def line_chart(path, title, xs, series, xlabel="", ylabel="", logx=False, logy=False):
    """xs: x 값들, series: [(이름, [xs마다 y 값])]"""
    cv = _Canvas(title, xlabel, ylabel)
    ys = [v for _, vs in series for v in vs if v > 0]
    fy, yticks, _ = _scale(logy, min(ys) if logy else 0, max(ys))
    cv.legend([n for n, _ in series])
    for t in yticks:
        cv.hgrid(MT + cv.ph * (1 - fy(t if (not logy or t > 0) else 1)), _fmt(t))
    cv.frame()
    if logx:
        a, b = math.log2(min(xs)), math.log2(max(xs))
        fx = lambda x: (math.log2(x) - a) / (b - a) if b > a else 0.5
    else:
        lo, hi = min(xs), max(xs)
        fx = lambda x: (x - lo) / (hi - lo) if hi > lo else 0.5
    px = lambda x: ML + 12 + (cv.pw - 24) * fx(x)
    for x in xs:
        cv.add(f'<line x1="{px(x):.1f}" x2="{px(x):.1f}" y1="{MT}" y2="{MT + cv.ph}" stroke="#f0f0f0"/>')
        cv.add(f'<text x="{px(x):.1f}" y="{MT + cv.ph + 18}" text-anchor="middle" font-size="11" '
               f'fill="#555">{_fmt(x)}</text>')
    for si, (name, vs) in enumerate(series):
        pts = [(px(x), MT + cv.ph * (1 - fy(v))) for x, v in zip(xs, vs) if v > 0]
        color = COLORS[si % len(COLORS)]
        cv.add(f'<polyline fill="none" stroke="{color}" stroke-width="2.2" '
               f'points="{" ".join(f"{x:.1f},{y:.1f}" for x, y in pts)}"/>')
        for x, y in pts:
            cv.add(f'<circle cx="{x:.1f}" cy="{y:.1f}" r="3.4" fill="{color}"/>')
    if logy or logx:
        cv.add(f'<text x="{W - MR}" y="{H - 12}" text-anchor="end" font-size="11" fill="#888">'
               f'{"로그-로그 축" if (logx and logy) else "로그 축"}</text>')
    cv.save(path)
