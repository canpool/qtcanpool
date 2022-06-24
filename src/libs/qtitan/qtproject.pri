!isEmpty(QTPROJECT_PRI_INCLUDED):message("qtproject.pri already included")
QTPROJECT_PRI_INCLUDED = 1

isEmpty(QTPROJECT_DIR):             QTPROJECT_DIR = $$PWD
isEmpty(QTPROJECT_OUT_PWD):         QTPROJECT_OUT_PWD = $$OUT_PWD
isEmpty(QTPROJECT_PRO_FILE_PWD):    QTPROJECT_PRO_FILE_PWD = $$_PRO_FILE_PWD_
isEmpty(QTPROJECT_PRO_FILE):        QTPROJECT_PRO_FILE = $$_PRO_FILE_

defineReplace(qtLibraryTargetName) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $$member(LIBRARY_NAME, 0)_debug
              else:win32:RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}

defineReplace(qtLibraryName) {
   RET = $$qtLibraryTargetName($$1)
   win32 {
      VERSION_LIST = $$split(QTPROJECT_VERSION, .)
      RET = $$RET$$first(VERSION_LIST)
   }
   return($$RET)
}

# eg: $$qtLibraryNameVersion(qcanpool, 1)
defineReplace(qtLibraryNameVersion) {
   RET = $$qtLibraryTargetName($$1)
   win32 {
      exists($$2) {
          VERSION_LIST = $$split(QTPROJECT_VERSION, .)
          RET = $$RET$$first(VERSION_LIST)
      } else {
          RET = $$RET$$2
      }
   }
   return($$RET)
}

defineTest(minQtVersion) {
    maj = $$1
    min = $$2
    patch = $$3
    isEqual(QT_MAJOR_VERSION, $$maj) {
        isEqual(QT_MINOR_VERSION, $$min) {
            isEqual(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
            greaterThan(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
        }
        greaterThan(QT_MINOR_VERSION, $$min) {
            return(true)
        }
    }
    greaterThan(QT_MAJOR_VERSION, $$maj) {
        return(true)
    }
    return(false)
}

# config IDE_SOURCE_TREE
IDE_SOURCE_TREE = $$QTPROJECT_DIR

isEmpty(IDE_BUILD_TREE) {
    sub_dir = $$QTPROJECT_PRO_FILE_PWD
    sub_dir ~= s,^$$re_escape($$IDE_SOURCE_TREE),,
    greaterThan(QT_MAJOR_VERSION, 4) {
        IDE_BUILD_TREE = $$clean_path($$QTPROJECT_OUT_PWD) # qt5
    } else {
        IDE_BUILD_TREE = $$QTPROJECT_OUT_PWD  # qt4
    }
    IDE_BUILD_TREE ~= s,$$re_escape($$sub_dir)$,,
}
