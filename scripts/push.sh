#!/usr/bin/env bash
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MulanPSL-2.0
#
# Push the current branch to every project remote (github, then gitee).
#
# Usage:
#   scripts/push.sh              # push the current branch
#   scripts/push.sh <branch>     # push the given branch
#
# Behaviour notes:
# - Each remote is pushed with an explicit refspec (remote + branch), so a
#   branch that does not exist on the remote yet is created instead of failing
#   with "The current branch ... has no upstream branch".
# - Upstream tracking is only set on the default remote (origin/gitee), so a
#   later plain "git push" keeps working.
# - Failures are retried a bounded number of times, and obviously permanent
#   failures (auth, rejected, non-fast-forward) abort immediately. The previous
#   "while true" loop retried forever, which turned any credential problem into
#   an infinite hang.
#
# Troubleshooting:
# - "could not read Username": no usable credential helper is configured.
#   Inspect with `git config --get-all credential.helper` (an empty value
#   resets the list and disables every helper configured earlier).
# - "CONNECT tunnel failed" / HTTP 502: an http(s)_proxy is intercepting the
#   connection. Unset it for these hosts if a direct connection is available.

set -uo pipefail

BRANCH="${1:-$(git rev-parse --abbrev-ref HEAD)}"
MAX_TRIES=3
RETRY_SLEEP=2

if [ "$BRANCH" = "HEAD" ]; then
    echo "error: HEAD is detached; pass a branch name explicitly" >&2
    exit 1
fi

push_with_retry() {
    local remote="$1"
    shift
    local try=1
    local out rc

    while [ "$try" -le "$MAX_TRIES" ]; do
        echo ">>> push '$BRANCH' to '$remote' (attempt $try/$MAX_TRIES)"
        out=$(git push "$@" "$remote" "$BRANCH" 2>&1)
        rc=$?
        if [ "$rc" -eq 0 ]; then
            printf '%s\n' "$out"
            return 0
        fi
        printf '%s\n' "$out" >&2

        # Do not retry on failures that will never succeed.
        case "$out" in
            *"could not read Username"* | *"unable to get password"* | \
            *"Authentication failed"* | *"Permission denied"* | \
            *"non-fast-forward"* | *"rejected"* | *"does not match any"*)
                echo "error: permanent failure pushing to '$remote', not retrying" >&2
                return "$rc"
                ;;
        esac

        try=$((try + 1))
        [ "$try" -le "$MAX_TRIES" ] && sleep "$RETRY_SLEEP"
    done

    echo "error: failed to push '$BRANCH' to '$remote' after $MAX_TRIES attempts" >&2
    return 1
}

FAILED=0

# github first, then gitee (default remote, also sets upstream tracking)
push_with_retry github || FAILED=1
push_with_retry origin -u || FAILED=1

exit "$FAILED"
