import glob
from datetime import datetime

filelists = glob.glob("data/reduction/*.graph")

cmd = "xmake r solver.elf -f {} -n {} -a {} -t {} -c {} -o {}"

cmdlist = []
t = str(datetime.now().strftime("%Y%m%d-%H%M%S"))
cmdlist.append("mkdir -p data/solution/{}".format(t.split('-')[0]))

for file in filelists:
    output_file = file.replace(".graph", "." + t.split('-')[-1] + ".sol").replace("data/reduction/", "data/solution/{}/".format(t.split('-')[0]))
    cmdlist.append(cmd.format(file, 64, 32, 16, 1800, output_file))

with open("scripts/run_solver.sh", "w+") as f:
    for cmd_i in cmdlist:
        f.write(cmd_i + "\n")
