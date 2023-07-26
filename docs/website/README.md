# Artery's Website using mkdocs
The website is located at [artery.v2x-research.eu](http://artery.v2x-research.eu).

## Getting Started
### Installing Dependencies
As [PEP 668](https://peps.python.org/pep-0668/) the recommended way to install dependencies is to use a local virtual Python Environment.
To create a virtual Python Environment:
```console
cd docs/website
python3 -m venv venv
```
Then install Artery's Website's dependencies.
You can either use the [`pyproject.toml`](pyproject.toml) (1.) or install every dependency manually (2.).  

1. using [`pyproject.toml`](pyproject.toml)  

Note: This feature might be a Python >= 3.11 feature.
In this case [Poetry](https://python-poetry.org) might be your tool of choice.
```console
venv/bin/pip install .
```

2. using manually install
```console
venv/bin/pip install mkdocs
venv/bin/pip install mkdocs-material
venv/bin/pip install mkdocs-git-revision-date-localized-plugin
```

### Build or Serve
Change the directory to the parent directory:
```
cd ..
```
You should be now in `artery/docs`.

Then do one of the following either

Use to build:
```console
website/venv/bin/mkdocs build
```

Use to serve:
```console
website/venv/bin/mkdocs serve
```