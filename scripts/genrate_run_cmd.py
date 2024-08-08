import glob

filelists = glob.glob("data/reduction/*.graph")

cmd = "xmake r solver.elf -f {} -n {} -a {} -t {} -c {} -o {}"

cmdlist = []

for file in filelists:
    output_file = file.replace(".graph", ".sol").replace("data/reduction/", "data/solution/")
    cmdlist.append(cmd.format(file, 64, 32, 16, 1800, output_file))

with open("scripts/run_solver.sh", "w+") as f:
    for cmd_i in cmdlist:
        f.write(cmd_i + "\n")
