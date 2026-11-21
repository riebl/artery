# Prerequisites (installation)

To enable Sionna integration, you should:
- create virtual python environment (preferred), or have access to system site-packages installation
- install `requirements.txt`
- activate environment (if you use venv) and ensure that build process will run with correct python installation

Then, configure project as usual with `WITH_SIONNA` set to `ON`. This will:
- try to configure `Mitsuba3` and its dependencies
- enable additional projects from `extern/`
- finally, add sionna to project build

