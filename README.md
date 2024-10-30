# Core Lib

![GitHub License](https://img.shields.io/github/license/razterizer/Core?color=blue)
![Static Badge](https://img.shields.io/badge/linkage-header_only-yellow)
![Static Badge](https://img.shields.io/badge/C%2B%2B-20-yellow)

[![build and test ubuntu](https://github.com/razterizer/Core/actions/workflows/build-and-test-ubuntu.yml/badge.svg)](https://github.com/razterizer/Core/actions/workflows/build-and-test-ubuntu.yml)

![Top Languages](https://img.shields.io/github/languages/top/razterizer/Core)
![GitHub repo size](https://img.shields.io/github/repo-size/razterizer/Core)
![Goto](https://img.shields.io/github/search/razterizer/Core/goto)
![Commit Activity](https://img.shields.io/github/commit-activity/t/razterizer/Core)
![Last Commit](https://img.shields.io/github/last-commit/razterizer/Core?color=blue)
![Contributors](https://img.shields.io/github/contributors/razterizer/Core?color=blue)
<!-- ![Languages](https://img.shields.io/github/languages/count/razterizer/Core) -->

This is a cross-platform, header-only library for core routines.

### DateTime.h

The code in this header file is put inside a namespace `datetime` which contain classes `Date`, `Time` and `DateTime`.

### Delay.h

The code here is in namespace `Delay` and features two functions `sleep(T us)` and `update_loop(int fps, std::function<bool(void)> update_func)`.

