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


def do_framework(args):
    outdir = args.outdir
    if not outdir:
        outdir = os.path.join(CUR_DIR, "qtcproject")
        if os.path.isdir(outdir):
            shutil.rmtree(outdir)
    os.makedirs(outdir, exist_ok=True)
    os.makedirs(os.path.join(outdir, "src", "shared"), exist_ok=True)
    files = (
        "qtproject.pri",
        "qtdata.pri",
        "src/qtlibrary.pri",
        "src/qtplugin.pri",
        "src/qttool.pri",
        "src/rpath.pri",
        "src/shared/qtproject_gui_pch.h",
        "src/shared/qtproject_pch.h"
    )
    for f in files:
        shutil.copy(os.path.join(ROOT_DIR, f), os.path.join(outdir, f))


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
    os.rename("template.pro", f"{args.name}.pro")


def do_main(args):
    print("try -h/--help for more details.")


def main():
    parser = argparse.ArgumentParser(description="qtcanpool project tool")
    parser.set_defaults(func=do_main)

    subparsers = parser.add_subparsers(help="project sub-commands")
    # framework
    subparser = subparsers.add_parser("framework", aliases=["fw"],
        formatter_class=argparse.RawTextHelpFormatter,  help="export the project framework")
    subparser.add_argument("-o", "--outdir", type=str, metavar="DIR",
        help="archive directory, default is qtcproject")
    subparser.set_defaults(func=do_framework)
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
