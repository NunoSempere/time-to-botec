SHELL := /bin/bash ## <= required to use time
run:
	venv/bin/python3 samples.py


install:
	# pipx install squigglepy==0.28
	uv venv
	source .venv/bin/activate
	uv pip install squigglepy==0.28

time:
	source .venv/bin/activate && time python3 samples.py
