import os
import shutil
import sys
import re

# 設定 Python 輸出 UTF-8，因爲有中文檔名
sys.stdout.reconfigure(encoding='utf-8')

# 根據規則提取適當的文件夾名稱
def extract_base_name(filename):
    name, _ = os.path.splitext(filename)  # 去除副檔名
    
    if "_" in name:  # 情況 1：有 `_`，則取 `_` 前的部分
        return name.split("_")[0]

    match = re.match(r"([a-zA-Z]+)(\d+)", name)  # 情況 2：檢查是否有字母+數字的模式
    if match:
        return match.group(1)  # 取數字前的字母部分

    return name  # 情況 3：沒有 `_`、沒有數字，直接用整個名稱

def organize_files(directory):
    if not os.path.exists(directory):
        print("Directory does not exist!")
        return
    
    for filename in os.listdir(directory):
        file_path = os.path.join(directory, filename)
         # 忽略子文件夾
        if not os.path.isfile(file_path):
            continue 
        # 刪除包含 # 的文件
        if "#" in filename:
            print(f"Deleting: {filename}")
            os.remove(file_path)  
            continue
        
        base_name = extract_base_name(filename)
        target_folder = os.path.join(directory, base_name)
        
        # 如果 target_folder 已經是一個文件
        if os.path.exists(target_folder) and not os.path.isdir(target_folder):
            print(f"Warning: {target_folder} exists as a file. Skipping.")
            continue
        
        # 確保 target_folder 存在
        os.makedirs(target_folder, exist_ok=True)
        # 移動文件
        target_path = os.path.join(target_folder, filename)
        shutil.move(file_path, target_path)  
    
    print("Files organized successfully!")

"""指定要整理文件夾的目錄地址"""
organize_files(r"C:\Users\tjx20\Downloads\SoulKnight5.0.0_asset_v1\Sprite")