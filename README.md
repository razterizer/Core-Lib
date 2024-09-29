# Core Lib

This is a header-only library.

![GitHub License](https://img.shields.io/github/license/razterizer/Core?color=blue)

[![build and test ubuntu](https://github.com/razterizer/Core/actions/workflows/build-and-test-ubuntu.yml/badge.svg)](https://github.com/razterizer/Core/actions/workflows/build-and-test-ubuntu.yml)

### DateTime.h

The code in this header file is put inside a namespace `datetime` which contain classes `Date`, `Time` and `DateTime`.

### Delay.h

The code here is in namespace `Delay` and features two functions `sleep(T us)` and `update_loop(int fps, std::function<bool(void)> update_func)`.

