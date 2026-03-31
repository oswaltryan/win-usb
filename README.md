# win-usb

Windows USB device enumeration utility written in C.

## Project Layout

- `include/winusb/`: Public headers.
- `src/`: C source files.
- `tests/unit/`: Unit test area.
- `docs/`: Design and architecture notes.
- `scripts/`: PowerShell helper scripts for local workflows.

## Build

```powershell
.\scripts\build.ps1
```

Common options:

```powershell
# Clean configure + release build
.\scripts\build.ps1 -Clean

# Debug build
.\scripts\build.ps1 -Config Debug

# Use an explicit generator
.\scripts\build.ps1 -Generator "Visual Studio 18 2026" -Arch x64
```

## Unit Tests

```powershell
.\scripts\unit-test.ps1
```

Test executables are split by module:
- `winusb_unit_common`
- `winusb_unit_devnode`
- `winusb_unit_enumerate`
- `winusb_unit_json_emit`
- `winusb_unit_storage`
- `winusb_unit_topology`

Common options:

```powershell
# Clean configure + run tests
.\scripts\unit-test.ps1 -Clean

# Debug test run
.\scripts\unit-test.ps1 -Config Debug
```

## Local CI (pre-commit)

This repo uses `pre-commit` for local pipeline checks.

### Required tools

- `pre-commit` (Python package)
- `cmakelint` (Python package)
- `clang-format`
- `clang-tidy`
- `cppcheck`

Install Python tools:

```powershell
python -m pip install --user pre-commit cmakelint
```

Install native tools:

- LLVM tools (`clang-format`, `clang-tidy`)
- Cppcheck (`cppcheck`)

### Run checks

Run quick checks (format/lint stage):

```powershell
python -m pre_commit run --all-files --hook-stage pre-commit
```

Run full local pipeline:

```powershell
.\scripts\ci.ps1
```

Equivalent full pre-commit commands:

```powershell
python -m pre_commit run --all-files --hook-stage pre-commit
python -m pre_commit run --all-files --hook-stage pre-push
```

Install hooks into Git:

```powershell
python -m pre_commit install
python -m pre_commit install --hook-type pre-push
```
