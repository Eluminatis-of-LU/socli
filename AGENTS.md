# socli — agent guidance

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug   # debug (localhost URL, leak checking)
cmake --build build                             # compile
./build/bin/socli                               # run
```

Release builds use `https://serious-oj.com`; debug builds use `http://judge_ui:8888` (see `include/client.h:8-11`).

Clean build: `rm -rf build && cmake -S . -B build && cmake --build build`.

On Linux, system packages required: `libglib2.0-dev libsecret-1-dev` (keychain). libcurl and c-logger are auto-fetched via CMake `FetchContent` if not found.

CMake configures `template/version.h.in` → `include/version.h` at configure time.

## No tests, no linting, no formatting

The project has no test framework, no linter, no formatter config. CI only validates the build compiles (`.github/workflows/pr.yml`).

## Architecture

- C11, single CMake executable target `socli`
- Global state: `g_target_url`, `curl`, `response_body` — all extern in `include/client.h`
- Nested command tree via `struct command` (name/desc/help/sub/func) using `stb_ds` dynamic arrays (`arrpush`)
- Entrypoint: `src/main.c` → parses `--url`, registers subcommands, dispatches via `print_help_and_traverse`
- Each subcommand module (`src/*.c`) exposes `init_*_command()` returning a `struct command *`
- Option parsing: header-only `ketopt.h` (getopt_long-like). Currently no sub-subcommand recursion beyond depth 2.
- JSON parsing: header-only `kson` (`lib/kson.c` + `include/kson.h`), used in contest and judge commands
- Auth: cookie-based, stored in OS keyring via `sr_keychain` (macOS Keychain, Windows Credential Manager, Linux libsecret)

## Debug mode

Defined by absence of `NDEBUG`. Enables:
- `stb_leakcheck` memory leak detection (automatic atexit report)
- Default target URL `http://judge_ui:8888`
- Verbose response body logged via printf in `client.c` write callback
- Log level controlled by env var `SOC_LOG_LEVEL` (0-5, default `LogLevel_INFO`)

## Version auto-bump & commit conventions

CI uses `ietf-tools/semver-action` to increment version before build. The VERSION in `CMakeLists.txt` is patched via `sed` in CI workflows. Do not manually bump version — CI does it.

PR titles and commit messages drive the semver bump:
- `feat:` — minor version bump
- `fix:` — patch version bump
- `BREAKING CHANGE` (in commit body or as `feat!:`/`fix!:`) — major version bump

## Adding a new command

1. Create `src/<cmd>.c` and `include/<cmd>.h` with `init_<cmd>_command(void)`
2. Register in `main.c` via `arrpush(root_command.sub, init_<cmd>_command())`
3. The `CMakeLists.txt` uses `file(GLOB_RECURSE)` — no CMake changes needed for new `.c`/`.h` files
4. Update `README.md` to document the new command

## CI workflows

| Workflow | Trigger |
|---|---|
| `pr.yml` | Push/PR to `main` — build on ubuntu + windows |
| `main.yml` | `workflow_dispatch` — build + create GitHub release |
| `opencode.yml` | PR/issue comments matching `/oc` or `/opencode` |
