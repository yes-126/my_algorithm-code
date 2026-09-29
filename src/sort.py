"""버블 정렬 — 이 저장소가 도는지 확인하는 예제이자, 새 프로젝트의 출발점."""


def bubble_sort(a):
    """a를 제자리에서 오름차순으로 정렬한다."""
    n = len(a)
    for i in range(n - 1):
        swapped = False
        # 한 번 훑을 때마다 가장 큰 값이 뒤로 밀려 자리를 잡는다.
        for j in range(n - 1 - i):
            if a[j] > a[j + 1]:
                a[j], a[j + 1] = a[j + 1], a[j]
                swapped = True
        # 한 바퀴 동안 교환이 없었다면 이미 정렬된 것이다.
        if not swapped:
            return a
    return a
