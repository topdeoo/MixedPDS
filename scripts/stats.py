import glob
from datetime import datetime

std = {
    "case2848rte": 331,
    "case2746wop": 166,
    "case_illinois200": 20,
    "case1888rte": 227,
    "RTS-96": 6,
    "case145": 13,
    "case1951rte": 238,
    "IEEE-118": 8,
    "case2736sp": 164,
    "case2746wp": 166,
    "case3012wp": 236,
    "case2868rte": 353,
    "case6468rte": 740,
    "case3375wp": 234,
    "case2737sop": 165,
    "IEEE-14": 2,
    "IEEE-300": 30,
    "case2383wp": 208,
    "case6470rte": 756,
    "IEEE-57": 3,
    "IEEE-30": 3,
    "case118": 8,
}

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
    result["instance_name"] = file.split("/")[-1].split(".")[0]
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

stats = {}
count = len(d.keys())

for k, l in d.items():
    for l_i in l:
        if l_i["instance_name"] not in stats:
            stats[l_i["instance_name"]] = {"min": int(l_i["solution_size"]), "avg": int(l_i["solution_size"]), "max": int(l_i["solution_size"]), "diff with fss": int(l_i["solution_size"]) - std[l_i["instance_name"]]}
        else:
            stats[l_i["instance_name"]]["min"] = max(stats[l_i["instance_name"]]["min"], int(l_i["solution_size"]))
            stats[l_i["instance_name"]]["min"] = min(stats[l_i["instance_name"]]["min"], int(l_i["solution_size"]))
            stats[l_i["instance_name"]]["diff with fss"] = stats[l_i["instance_name"]]["min"] - std[l_i["instance_name"]]
            stats[l_i["instance_name"]]["avg"] += int(l_i["solution_size"])

for k, v in stats.items():
    v["avg"] = v["avg"] / count

with open("result/{}-stats.json".format(t), "w+") as f:
    f.write(json.dumps(stats, indent=4))
