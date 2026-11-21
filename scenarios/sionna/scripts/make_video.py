# %%

"""
Makes videos from series of renders.
"""

from pathlib import Path

import cv2


def process_single(
    root_dir: Path, fps: int = 30, resolution: tuple[int, int] = (1280, 720)
):
    writer = cv2.VideoWriter(
        f"{root_dir}.mp4",
        cv2.VideoWriter_fourcc(*"mp4v"),
        fps,
        resolution,
    )

    def selector(p: Path):
        return p.is_file() and p.suffix == ".png"

    try:
        for frame_path in sorted(filter(selector, root_dir.iterdir())):
            frame = cv2.imread(frame_path)
            assert frame is not None, f"could not load frame: {frame_path}"
            writer.write(frame)
    finally:
        writer.release()


# %%

results_dir = Path.cwd().parents[0] / "results" / "limassol"
if not results_dir.is_dir():
    raise FileNotFoundError(
        f"Make sure directory {results_dir} exists in your scenario"
    )

for root, dirs, _ in results_dir.walk():
    for dir in dirs:
        process_single(root / dir)
