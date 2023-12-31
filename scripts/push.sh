#!/usr/bin/env bash
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MulanPSL-2.0

while true; do
    # push code to github
    git push github
    if [ $? -eq 0 ]; then
        break
    fi
    sleep 1
done

# push code to gitee (default)
git push
