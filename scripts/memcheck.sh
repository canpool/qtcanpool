#!/usr/bin/env bash
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MulanPSL-2.0

# on openeuler:
#   dnf install valgrind glibc-debuginfo
# example:
#   bash memcheck.sh ./PROGRAM 2>&1 | tee memcheck.log
valgrind --tool=memcheck --leak-check=full --show-reachable=yes --trace-children=yes --smc-check=all --track-origins=yes $@
