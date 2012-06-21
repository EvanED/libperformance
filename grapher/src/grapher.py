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
plt.annotate("allocate (0.252019sec)", xy=(0.252019,1), xytext=(0.252019,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("allocate (0.751594sec)", xy=(0.751594,1), xytext=(0.751594,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("allocate (1.25168sec)", xy=(1.25168,1), xytext=(1.25168,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("allocate and initialize (1.75163sec)", xy=(1.75163,1), xytext=(1.75163,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("allocate and initialize (2.25168sec)", xy=(2.25168,1), xytext=(2.25168,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("allocate and initialize (2.7516sec)", xy=(2.7516,1), xytext=(2.7516,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("deallocate (3.25165sec)", xy=(3.25165,1), xytext=(3.25165,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("deallocate (3.50156sec)", xy=(3.50156,1), xytext=(3.50156,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("deallocate (3.75162sec)", xy=(3.75162,1), xytext=(3.75162,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("deallocate (4.00064sec)", xy=(4.00064,1), xytext=(4.00064,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("deallocate (4.25066sec)", xy=(4.25066,1), xytext=(4.25066,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("deallocate (4.50065sec)", xy=(4.50065,1), xytext=(4.50065,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("deallocate (4.75064sec)", xy=(4.75064,1), xytext=(4.75064,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("deallocate (5.00064sec)", xy=(5.00064,1), xytext=(5.00064,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("deallocate (5.25067sec)", xy=(5.25067,1), xytext=(5.25067,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("deallocate (5.50065sec)", xy=(5.50065,1), xytext=(5.50065,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("deallocate (5.75063sec)", xy=(5.75063,1), xytext=(5.75063,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("deallocate (6.00064sec)", xy=(6.00064,1), xytext=(6.00064,1), rotation="vertical", va="bottom", size="x-small")
plt.annotate("Program end (6.00067sec)", xy=(6.00067,1), xytext=(6.00067,1), rotation="vertical", va="bottom", size="x-small")
plt.savefig("blah.pdf", format="pdf")
