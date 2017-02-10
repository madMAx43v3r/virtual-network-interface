#!/bin/bash

cd `dirname $0`

CURR_VERSION=`cat version`
GIT_VERSION=`git rev-parse HEAD`

if [ ! -e vnicppcodegen ]; then
  CURR_VERSION=""
fi

if [ "$CURR_VERSION" != "$GIT_VERSION" ]; then
  g++ -std=c++11 -g -Wfatal-errors -Icodegen/ -o vnicppcodegen codegen/cpp/vnicppcodegen.cpp
  echo $GIT_VERSION > version
fi

