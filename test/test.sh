#!/bin/bash
source "common.sh"
# 定义要检查的文件夹列表
folders=("log")  # 替换为实际的文件夹名称

# 遍历文件夹并执行其中的 test.sh
for folder in "${folders[@]}"; do
    echo "Entering folder: $folder"
    cd "$folder" || { echo "Error: Failed to enter folder $folder"; exit 1; }

    # 检查是否存在 test.sh
    if [[ -f "test.sh" ]]; then
        echo "Running test.sh in $folder..."
        ./test.sh || { ERR "Error: test.sh in $folder failed"; exit 1; }
    else
        ERR "Error: test.sh not found in $folder"
        exit 1
    fi

    # 返回上级目录
    cd ..
    echo "Finished testing in $folder"
    echo
done

INFO "All tests completed successfully."
exit 0