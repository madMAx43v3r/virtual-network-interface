#!/bin/bash

TARGET=/usr/local/bin

if [ "$#" -gt 0 ]; then
  TARGET=$1
fi

cp vnicppcodegen $TARGET/

