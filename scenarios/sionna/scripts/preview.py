# %%

"""
Sampler fro camera view. Use this to position cameras in scene.
"""

from dataclasses import dataclass, field
from pathlib import Path
from typing import Optional

import mitsuba as mi
import numpy as np
import sionna.rt as rt

scenario_dir = Path.cwd().parents[0]
scene_file = scenario_dir / "limassol-scene.xml"
output_file = scenario_dir / "results" / "preview.png"

if not scenario_dir.is_dir():
    raise FileNotFoundError(f"scenario directory was not found: {scenario_dir}")

if not scene_file.is_file():
    raise FileNotFoundError(f"scene file was not found: {scene_file}")

scene = rt.load_scene(str(scene_file))

# %%

scene.preview(show_orientations=True)

# %%

# Cameras are identified as (position, rotation) tuple. You
# may provide target for look_at(), but it kinda shady and I prefer
# orientation vector instead.


@dataclass
class CameraSettings:
    origin: mi.Point3f = field(default_factory=lambda: mi.Point3f(0.0, 0.0, 150.0))
    orientation: mi.Point3f = field(default_factory=lambda: mi.Point3f(0.0, 0.0, 0.0))
    target: Optional[mi.Point3f] = field(default_factory=lambda: None)
    fov: float = 45.0


current_camera_settings = CameraSettings(
    # origin=mi.Point3f(0.0, 0.0, 0.0),
    # orientation=mi.Point3f(0.0, 0.0, 0.0),
    # target=None,
    # fov=45.0,
)
print(f"visualizing with camera settings: {current_camera_settings}")

camera = rt.Camera(
    position=current_camera_settings.origin,
    orientation=current_camera_settings.orientation,
)
if current_camera_settings.target is not None:
    camera.look_at(current_camera_settings.target)


scene.render_to_file(
    camera=camera,
    filename=str(output_file),
    fov=current_camera_settings.fov,
)
print(f"Rendered preview to {output_file}")

# %%

w = scene._preview_widget
cam = w.camera

origin = cam.position
print(f"origin: {origin}")

origin = np.array(origin, dtype=np.float32)

v = np.asarray([0.0, 0.0, -1.0], dtype=float)
q = np.asarray(cam.quaternion, dtype=float)

q_xyz = q[:3]
qw = q[3]

uv = 2.0 * np.cross(q_xyz, v)
uuv = np.cross(q_xyz, uv)
forward = v + qw * uv + uuv

forward = forward / np.linalg.norm(forward)
quat_target = origin + forward

test_cam = rt.Camera(position=mi.Point3f(origin))
test_cam.look_at(mi.Point3f(quat_target))

print(f"orientation: {test_cam.orientation}")

# %%
