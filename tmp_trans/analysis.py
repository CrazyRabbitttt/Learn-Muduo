# 打开包含数据的文件
with open('your_data_file.txt', 'r') as file:
    data = file.read()

# 分割数据为行
lines = data.split('\n')

# 初始化总和和行数
total = 0
count = 0

# 遍历每一行
for line in lines:
    # 使用正则表达式提取xxms的数值
    import re
    match = re.search(r'mc_t=(\d+)ms', line)
    if match:
        value = int(match.group(1))
        total += value
        count += 1

# 计算平均值
if count > 0:
    average = total / count
    print(f"平均耗时是: {average} ms")
else:
    print("没有找到匹配的数据")

