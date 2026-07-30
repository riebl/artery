# %%
"""
Compute transform vectors for Sionna and SUMO coordinates mapping.
"""

import numpy as np
import numpy.typing as npt


# Returns (rotation, translation) vector
def fit(
    source_point: np.ndarray, target_point: np.ndarray
) -> tuple[npt.NDArray, npt.NDArray]:
    if (
        source_point.shape != target_point.shape
        or source_point.ndim != 2
        or target_point.shape[1] != 2
    ):
        raise ValueError("Expected two arrays with shape (N, 2)")
    n = source_point.shape[0]
    if n < 2:
        raise ValueError("Need at least 2 correspondences.")

    a_mean = source_point.mean(axis=0)
    b_mean = target_point.mean(axis=0)
    aa = source_point - a_mean
    bb = target_point - b_mean

    # 2D Kabsch (rotation only)
    h = aa.T @ bb
    u, _, vt = np.linalg.svd(h)
    r = vt.T @ u.T
    if np.linalg.det(r) < 0:
        vt[-1, :] *= -1.0
        r = vt.T @ u.T

    t = b_mean - r @ a_mean
    return r, t


class Transform:
    @staticmethod
    def from_sumo(t: npt.NDArray, r: npt.NDArray, point: npt.ArrayLike) -> npt.NDArray:
        return r @ np.asarray(point, dtype=np.float32) + t

    @staticmethod
    def from_sionna(
        t: npt.NDArray, r: npt.NDArray, point: npt.ArrayLike
    ) -> npt.NDArray:
        return r.T @ (np.asarray(point, dtype=np.float32) - t)


# %%

# SUMO coordinates, then Sionna coords
points = [
    # (100.0, 200.0, 10.0, -30.0),
    # (300.0, 250.0, 210.0, 20.0),
    # (1103, 1277, -594, 538),
    (1075, 758, -282, 616),
    (998, 690, -211, 539),
]

if len(points) < 2:
    raise ValueError("Add at least two tie points in points before running fit")

arr = np.asarray(points, dtype=float)
r, t = fit(arr[:, :2], arr[:, 2:])

print(f"rotation: {r}")
print(f"translation: {t}")

# %%

# SUMO coordinates
sample = (
    # 0.0, 0.0
    966,
    671,
)

converted = Transform.from_sumo(t, r, sample)
back = Transform.from_sionna(t, r, converted)

assert np.allclose(sample, back)

print(f"translated: {converted}")
print(f"translated back: {back}")
