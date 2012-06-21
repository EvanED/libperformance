import json

with open("blah.json") as input:
    run_info = json.load(input)
    data = run_info["measurements"]
    assert data[-1] == {}
    data = data[:-1]
    annotations = run_info["annotations"]


import matplotlib
matplotlib.use("PDF")
import matplotlib.pyplot as plt

measurement_keys = set()
for datum in data:
    measurement_keys |= set(datum.iterkeys())

measurement_keys -= set([u"time_offset_sec"])

elapsed = [datum["time_offset_sec"] for datum in data]

for key in measurement_keys:
    values = [datum[key] for datum in data]
    assert len(values) == len(elapsed)

    plt.plot(elapsed, values, "k")
    

for annotation in annotations:
    time = annotation["time_offset_sec"]
    text = annotation["annotation"]

    text = "%s (%s sec)" % (text, str(time))

    plt.annotate(text, xy=(time,1), xytext=(time,1), rotation="vertical", va="bottom", size="x-small")

plt.savefig("blah.pdf", format="pdf")
