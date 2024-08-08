#!/bin/bash

# 设置目录
DIRECTORY="./data/"  # 当前目录，可以根据需要更改

# 查找所有 .ptxt 文件
PTXT_FILES=$(find $DIRECTORY -type f -name "*.ptxt")

# 遍历每个 .ptxt 文件，并查找对应的 .graph 文件
for PTEXT_FILE in $PTXT_FILES; do
    # 获取文件名（不包括扩展名）
    FILENAME=$(basename "$PTEXT_FILE" .ptxt)
    
    # 查找对应的 .graph 文件
    GRAPH_FILE=${DIRECTORY}${FILENAME}.graph
    COMMAND="python3 scripts/ptxt2graph.py -i $PTEXT_FILE -o $GRAPH_FILE"
    echo "Executing command: $COMMAND"
    eval $COMMAND

done