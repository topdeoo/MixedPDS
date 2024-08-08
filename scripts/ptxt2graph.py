def convert_graph_format(input_data):
    lines = input_data.strip().split('\n')
    vertex_count = int(lines[1])
    edges = []
    
    i = 3
    while i < len(lines):
        node = int(lines[i].split()[-1]) + 1
        edge_count = int(lines[i + 1])
        edge_list = lines[i + 3 : i + 3 + edge_count]
        for edge in edge_list:
            edges.append((node, int(edge) + 1))
        i += (3 + edge_count)
    
    edge_count = len(edges)
    result = f"{vertex_count} {edge_count}\n"
    for edge in edges:
        result += f"{edge[0]} {edge[1]}\n"
    
    return result.strip()

import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Transform ptxt format into graph format")
    parser.add_argument('-i', '--input', help='输入文件名')
    parser.add_argument('-o', '--output', help='输出文件名')
    args = parser.parse_args()


# 从文件中读取输入数据
input_file_path = args.input
with open(input_file_path, 'r') as file:
    input_data = file.read()

# 转换格式并输出结果
output_data = convert_graph_format(input_data)

output_file_path = args.output
with open(output_file_path, 'w') as file:
    file.write(output_data)