import re
import subprocess
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
ROOT = next(
    parent
    for parent in [SCRIPT_DIR, *SCRIPT_DIR.parents]
    if (parent / "xmake.lua").is_file()
)

GENERATOR = "vsxmake2022"
VS_DIR = ROOT / GENERATOR


def main():
    print("Generating Visual Studio solution...")

    subprocess.run(
        [
            "xmake", "project",
            "-k", GENERATOR,
            "-m", "debug,release",
            "-vD"
        ],
        cwd=ROOT,
        check=True
    )

    solutions = list(VS_DIR.glob("*.sln"))

    if not solutions:
        raise FileNotFoundError(
            f"No solution found in {VS_DIR}"
        )

    source = solutions[0]
    destination = ROOT / source.name

    content = source.read_text(encoding="utf-8-sig")

    # Adjust paths relative to the project root
    content = re.sub(
        r'(Project\("[^"]+"\)\s*=\s*"[^"]+",\s*")'
        r'([^"]+\.vcxproj)'
        r'(")',
        lambda match: (
            match.group(1)
            + GENERATOR + "\\"
            + match.group(2)
            + match.group(3)
        ),
        content,
        flags=re.IGNORECASE
    )

    destination.write_text(
        content,
        encoding="utf-8-sig"
    )

    print(f"Solution generated: {destination}")


if __name__ == "__main__":
    main()
