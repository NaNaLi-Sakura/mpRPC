#===================================================================
# File Name: autobuild.sh
# Author: 雪与冰心丶
# main: 2414811214@qq.com
# Created Time: 2024年 09月 24日 星期二 18:00:27 CST
#===================================================================
#!/bin/bash

set -e

rm -rf ./build/
cmake -B ./build
cmake --build ./build
cp -r `pwd`/src/include `pwd`/lib
