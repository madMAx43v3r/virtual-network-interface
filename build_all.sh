#!/bin/bash

if [ ! -e vnicppcodegen ]; then
  g++ -std=c++11 -g -Wfatal-errors -Icodegen/ -o vnicppcodegen codegen/cpp/vnicppcodegen.cpp
fi

