#!/bin/sh

set -e

REGIME=${1}
if [ -z "$REGIME" ]; then
  REGIME=dbg
fi

SRC_DIR=$(pwd)
BUILD_DIR=build/linux-x86_64/${REGIME}

[ -d ${BUILD_DIR} ] || mkdir -p ${BUILD_DIR}

cd ${BUILD_DIR}

if [ "$REGIME" = "dbg" ]; then
  CMAKE_TYPE=Debug
else
  CMAKE_TYPE=Release
fi

$WRAPPER cmake -DCMAKE_BUILD_TYPE=${CMAKE_TYPE} -DCMAKE_INSTALL_PREFIX=dist ${SRC_DIR}
$WRAPPER make
make install
