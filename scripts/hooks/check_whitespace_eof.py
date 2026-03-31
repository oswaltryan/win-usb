#!/usr/bin/env python3
from __future__ import annotations

import argparse
from pathlib import Path
import subprocess
import sys


TEXT_EXTENSIONS = {
    ".c",
    ".h",
    ".cmake",
    ".txt",
    ".md",
    ".yml",
    ".yaml",
    ".ps1",
    ".py",
    ".json",
}


def should_check(path: Path) -> bool:
    if not path.is_file():
        return False
    if path.suffix.lower() in TEXT_EXTENSIONS:
        return True
    return path.name in {"CMakeLists.txt", ".gitignore", ".clang-format", ".pre-commit-config.yaml"}


def is_binary(path: Path) -> bool:
    try:
        data = path.read_bytes()
    except OSError:
        return True
    return b"\x00" in data


def check_file(path: Path) -> list[str]:
    errors: list[str] = []
    try:
        content = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        return errors
    except OSError as exc:
        errors.append(f"{path}: unable to read file ({exc})")
        return errors

    if content and not content.endswith("\n"):
        errors.append(f"{path}: missing trailing newline at EOF")

    for i, line in enumerate(content.splitlines(), start=1):
        if line.endswith(" ") or line.endswith("\t"):
            errors.append(f"{path}:{i}: trailing whitespace")

    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("files", nargs="*")
    args = parser.parse_args()

    if args.files:
        paths = [Path(p) for p in args.files]
    else:
        repo_root = Path.cwd()
        tracked = subprocess.run(
            ["git", "ls-files", "-z"],
            check=True,
            capture_output=True,
            text=False,
        ).stdout.split(b"\x00")
        untracked = subprocess.run(
            ["git", "ls-files", "--others", "--exclude-standard", "-z"],
            check=True,
            capture_output=True,
            text=False,
        ).stdout.split(b"\x00")
        rel_paths = [p for p in tracked + untracked if p]
        paths = [repo_root / p.decode("utf-8", errors="ignore") for p in rel_paths]
    errors: list[str] = []
    for path in paths:
        if not should_check(path):
            continue
        if is_binary(path):
            continue
        errors.extend(check_file(path))

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
