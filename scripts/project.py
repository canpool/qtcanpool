#!/usr/bin/env python3
# Copyright (c) 2023 maminjie <canpool@163.com>
# SPDX-License-Identifier: MulanPSL-2.0

import os
import shutil
import argparse
import logging

logging.basicConfig(format='%(asctime)s %(levelname)s: %(message)s', level=logging.DEBUG)

CUR_DIR = os.path.dirname(os.path.realpath(__file__))
ROOT_DIR = os.path.abspath(os.path.join(CUR_DIR, '..'))


def rename_in_file(path, old, new):
    if not os.path.isfile(path):
        return
    with open(path, encoding="utf-8") as f:
        content = f.read()
    content = content.replace(old, new)
    with open(path, "w", encoding="utf-8", newline="") as f:
        f.write(content)


def do_create(args):
    if "." in args.name or args.name in ("template",):
        logging.error("project name is illegal")
        return
    outdir = args.outdir
    if not outdir:
        outdir = os.path.join(ROOT_DIR, "projects")
    project_dir = os.path.join(outdir, args.name)
    if os.path.isdir(project_dir):
        shutil.rmtree(project_dir)
    template_dir = os.path.join(ROOT_DIR, "projects", "template")
    shutil.copytree(template_dir, project_dir)
    os.chdir(project_dir)
    # The CMake template hardcodes its own name as the project and the target,
    # so a copy would otherwise build an executable called qxtemplate.
    for name in ("CMakeLists.txt", "main.cpp"):
        rename_in_file(name, "qxtemplate", args.name)


def do_main(args):
    print("try -h/--help for more details.")


def main():
    parser = argparse.ArgumentParser(description="qtcanpool project tool")
    parser.set_defaults(func=do_main)

    subparsers = parser.add_subparsers(help="project sub-commands")

    # create project based on projects/template
    subparser = subparsers.add_parser("create", aliases=["c", "new"],
        formatter_class=argparse.RawTextHelpFormatter,  help="create a project based on template")
    subparser.add_argument("name", type=str, metavar="NAME", help="project name")
    subparser.add_argument("-o", "--outdir", type=str, metavar="DIR",
        help="project directory, default is projects")
    subparser.set_defaults(func=do_create)

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
