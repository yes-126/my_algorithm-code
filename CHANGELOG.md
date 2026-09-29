# Changelog

이 저장소(실습 환경 template)의 변경 기록. 형식은
[Keep a Changelog](https://keepachangelog.com/ko/1.1.0/)를 따르고,
버전은 [유의적 버전](https://semver.org/lang/ko/)을 따른다.

## [1.0.3] - 2026-08-25

### Added

- **Code Runner로 파일 하나 실행.** 편집기 오른쪽 위 ▶ 버튼으로 열려 있는
  C·Python 파일을 그대로 돌린다. v1.0.2에서 C/C++ 확장의 ▶ 버튼을 끄면서
  "그냥 한 번 돌려보기" 수단이 사라졌던 것을 대신한다.
- `Makefile`에 `%.out` 패턴 규칙. `%.debug.out`과 짝을 이룬다.

### Fixed

- Code Runner 기본 설정의 두 문제를 `.vscode/settings.json`에서 고쳤다.
    - C가 `gcc main.c -o main`으로 파일 하나만 컴파일해
      `undefined reference`로 실패하던 것. 이제 `Makefile`을 거친다
    - Python이 이미지에 없는 `python`을 찾던 것. `python3`로 바꿨다
- `runInTerminal`을 켰다. 출력 패널은 입력을 받지 못해 `scanf`·`input()`이
  멈춘다.

### Changed

- `CFLAGS`에서 고정 `-Isrc`를 뺐다. 패턴 규칙이 대상 파일의 폴더를 붙인다.

## [1.0.2] - 2026-08-25

### Added

- **`C 디버그 (현재 파일)` 구성.** 열려 있는 `.c` 파일을 그대로 디버깅한다.
  주제 폴더가 늘어나도 구성을 새로 만들 필요가 없다. Python도 같은 구성을 뒀다.
- `Makefile`에 패턴 규칙 `%.debug.out: %.c`. 어느 폴더의 파일이든
  `make <경로>/<이름>.debug.out`으로 빌드된다. 같은 폴더의 `.c`를 함께
  링크하므로 구현이 옆 파일에 있어도 된다. 대신 한 폴더에 `main`은 하나만 둔다.

### Changed

- **편집기 오른쪽 위의 ▶ 버튼을 껐다**(`C_Cpp.debugShortcut`). 그 버튼은 파일
  하나만 컴파일해 링크가 실패하는데, `F5`의 구성 목록에서도 맨 위에 기본으로
  잡혀 잘못 고르기 쉬웠다. 이제 목록에 우리 구성만 남는다.
- `DEBUGFLAGS`에서 `-Isrc`를 뺐다. 패턴 규칙이 대상 파일의 폴더를 붙인다.

## [1.0.1] - 2026-08-25

### Fixed

- **VS Code에서 F5·빌드가 링크 오류로 실패하던 문제.** 확장이 기본으로 만드는
  "C/C++: gcc 활성 파일 빌드"는 열려 있는 파일 하나만 컴파일한다. 이 프로젝트는
  `main.c`와 `sort.c`가 나뉘어 있어 `undefined reference to bubbleSort`로
  링크가 실패했다. `.vscode/tasks.json`과 `launch.json`을 넣어 `Makefile`을
  거치도록 했다. 그 기본 작업은 출력 파일 이름도 `main`(확장자 없음)이라
  `.gitignore`에 걸리지 않는 문제가 함께 있었다.

### Added

- `.vscode/` 설정: 빌드(`Cmd/Ctrl + Shift + B`), 테스트, C·Python 디버그(`F5`),
  IntelliSense의 헤더 경로, Testing 패널에서 Python 테스트 실행
- `make debug`: 최적화를 끄고 심볼을 남겨 빌드한다. `F5`가 이 결과물을 쓴다
- devcontainer에 `ms-python.debugpy` 확장 추가

### Changed

- 편집기 설정의 단일 원본을 `.vscode/settings.json`으로 옮겼다. devcontainer에는
  컨테이너에서만 의미가 있는 것만 남긴다

## [1.0.0] - 2026-08-25

첫 릴리스. 2026-2 고급알고리즘의 실습 환경 template이다.
`Use this template`으로 사본을 만들어 과제와 개인프로젝트를 시작한다.

### Added

- **실습 컨테이너**: `debian:trixie-slim` 기반 이미지에 gcc 14.2, gdb, make,
  Python 3.13, git을 담았다. 서비스는 `lab` 하나이고, 저장소 폴더를 컨테이너의
  `/work`에 바인드 마운트한다. 이미지에 소스를 굽지 않으므로 호스트에서 고친
  코드가 바로 반영된다.
- **Codespaces 지원**: `.devcontainer/devcontainer.json`이 `compose.yml`의
  `lab` 서비스를 그대로 쓴다. 브라우저에서 열든 로컬에서 `docker compose`로
  띄우든 같은 컨테이너다.
- **`src`/`tests` 뼈대**: 버블 정렬을 C와 Python으로 같은 함수 이름으로 구현하고,
  각각에 유닛 테스트를 붙였다. C 테스트는 외부 프레임워크 없이 표준 C로만 쓴다.
- **`Makefile`**: `make run`으로 두 구현을 실행하고 `make test`로 두 테스트를
  돌린다. 테스트가 실패하면 0이 아닌 코드로 끝난다.
- **`.gitignore`**: 실행 파일은 `*.out`으로 만드는 규약을 두고 그것만 걸러낸다.
  컨테이너에서 컴파일한 Linux 바이너리가 커밋에 섞이지 않는다.

### Notes

- 이 저장소에는 **강의 예제 코드를 넣지 않는다.** 주제별 예제는
  [algorithm-code](https://github.com/lec-algorithm/algorithm-code)에 있고,
  교안과 슬라이드는 [lecture](https://github.com/lec-algorithm/lecture)에 있다.
- `src`의 버블 정렬은 환경이 도는지 보여 주는 예제다. 사본을 만든 뒤에는 지우고
  자기 코드를 넣으면 된다.

[1.0.3]: https://github.com/lec-algorithm/algorithm-env/releases/tag/v1.0.3
[1.0.2]: https://github.com/lec-algorithm/algorithm-env/releases/tag/v1.0.2
[1.0.1]: https://github.com/lec-algorithm/algorithm-env/releases/tag/v1.0.1
[1.0.0]: https://github.com/lec-algorithm/algorithm-env/releases/tag/v1.0.0
