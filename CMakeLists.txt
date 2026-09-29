# TrailCurrent Capstan — rotary touchscreen controller firmware.
#
# One source tree, three panels. The active board is selected with
# `idf.py menuconfig` → "Capstan board"; nothing outside
# components/capstan_board and main/Kconfig.projbuild is board-specific.
cmake_minimum_required(VERSION 3.16)
set(PROJECT_VER "0.0.1")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(trailcurrent_capstan)
