import glob
from datetime import datetime

t = datetime.now().strftime('%Y%m%d')

solution_files = glob.glob('data/solution/{}/*.sol'.format(t))

d = dict()

for file in solution_files:
    cat = file.split(".")[-2]
    if cat not in d:
        d[cat] = []
    with open(file, 'r') as f:
        lines = f.readlines()
    result = {}
    timeline = lines[0].strip().split(" ")
    if timeline[0] == 'r':
        result["solve time"] = timeline[1] + " us"
    else:
        result["solve time"] = timeline[0] + " us"
    result["solution_size"] = lines[1].strip()

    d[cat].append(result)

import json

with open("result/{}.json".format(t), "w+") as f:
    f.write(json.dumps(d, indent=4))