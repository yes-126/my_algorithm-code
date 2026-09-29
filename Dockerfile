# syntax=docker/dockerfile:1
# 실습 컨테이너. 알고리즘 예제를 돌리는 데 필요한 것만 담는다.
#
# 소스는 이미지에 굽지 않는다. compose.yml이 저장소 폴더를 /work에 바인드
# 마운트하므로, 코드는 호스트 편집기로 고치고 실행만 여기서 한다.
FROM debian:trixie-slim

# build-essential: gcc · make와 표준 라이브러리 헤더
# gdb: 포인터가 어디를 가리키는지 직접 들여다볼 때
# python3: 같은 알고리즘의 두 번째 구현
# git, less: Codespaces 터미널에서 저장소를 다루려면 컨테이너 안에도 있어야 한다
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
       build-essential gdb python3 git less ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work
