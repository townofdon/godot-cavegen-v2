#!/bin/bash

source "./utils.sh"

#
# SCRIPT
#

assertFileExists "../build/webgl/index.html"
rm -rf "./webgl"
mkdir -p "./webgl"
cp -a "../build/webgl/." "./webgl"

docker-compose up --build --force-recreate
