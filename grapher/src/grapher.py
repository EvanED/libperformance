import json

with open("blah.json") as input:
    run_info = json.load(input)
    data = run_info["measurements"]
    annotations = run_info["annotations"]


data = [
    [0.251426, 19, 1,19, 1],
    [0.501448, 20, 1,20, 1],
    [0.751445, 20, 1,20, 1],
    [1.00136, 21, 1,21, 1],
    [1.25146, 21, 1,21, 1],
    [1.50145, 22, 1,22, 1],
    [1.75145, 22, 1,22, 1],
    [2.00146, 23, 2,23, 2],
    [2.25146, 23, 2,23, 2],
    [2.50144, 24, 3,24, 3],
    [2.75143, 24, 3,24, 3],
    [3.00144, 25, 4,25, 4],
    [3.25145, 25, 4,25, 4],
    [3.50138, 24, 4,25, 4],
    [3.75144, 24, 4,25, 4],
    [4.00044, 23, 4,25, 4],
    [4.25044, 23, 4,25, 4],
    [4.50045, 22, 4,25, 4],
    [4.75044, 22, 4,25, 4],
    [5.00045, 21, 3,25, 4],
    [5.25046, 21, 3,25, 4],
    [5.50045, 20, 2,25, 4],
    [5.75045, 20, 2,25, 4],
    [6.00043, 19, 1,25, 4],
]

data_t = zip(*data)
[elapsed, vm_bytes, resident_bytes, vm_bytes_peak, vm_resident_bytes_peak] = data_t

import matplotlib
matplotlib.use("PDF")
import matplotlib.pyplot as plt
plt.plot(elapsed, vm_bytes_peak, "k",
         elapsed, vm_resident_bytes_peak, "k",
         elapsed, vm_bytes, "r",
         elapsed, resident_bytes, "b")

for annotation in annotations:
    time = annotation["time_offset_sec"]
    text = annotation["annotation"]

    text = "%s (%s sec)" % (text, str(time))

    plt.annotate(text, xy=(time,1), xytext=(time,1), rotation="vertical", va="bottom", size="x-small")

plt.savefig("blah.pdf", format="pdf")
