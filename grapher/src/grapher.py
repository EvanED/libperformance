import json
import sys

try:
    [prog, dash_o, out_pdf, in_json] = sys.argv
    if dash_o != "-o":
        [a,b] = [1]
except ValueError:
    print "Usage:", sys.argv[0], " -o out.pdf in.json"
    print "(Yes, it's specific for now)"
    sys.exit(1)


_next_color = ["r", "b", "g"]
def next_color():
    global _next_color
    try:
        ret = _next_color[0]
        _next_color = _next_color[1:]
        return ret
    except IndexError:
        return "k"

with open(in_json) as input:
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

    plt.plot(elapsed, values, next_color())
    

for annotation in annotations:
    time = annotation["time_offset_sec"]
    text = annotation["annotation"]

    text = "%s (%s sec)" % (text, str(time))

    plt.annotate(text, xy=(time,1), xytext=(time,1), rotation="vertical", va="bottom", size="x-small")

plt.savefig(out_pdf, format="pdf")
