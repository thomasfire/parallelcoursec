import subprocess as sp
from sys import argv


def main():
    executable = argv[1]
    start_r = int(argv[2])
    end_r = int(argv[3])
    threads = [1, 2, 3, 4, 5, 6, 7, 8, 128, 2048]
    delta = int((end_r - start_r) / 10)
    first_line = "n,time1,check1,time2,check2,time3,check3,time4,check4,time5,check5,time6,check6,time7,check7,time8,check8,time128,check128,time2048,check2048\n"
    lines = []
    for x in range(start_r, end_r + 1, delta):
        target = [str(x)]
        for t in threads:
            outp1 = sp.check_output([executable, str(x), str(t)], shell=False).decode("utf-8").strip().split(",")
            thr1, n1, deltams1, checksum1 = int(outp1[0]), int(outp1[1]), int(outp1[2]), float(outp1[3])
            if len(argv) == 6 and argv[5] == "--reduced":
                target.append(str(int(deltams1)))
                target.append("{:.6f}".format(checksum1))
            else:
                outp2 = sp.check_output([executable, str(x), str(t)], shell=False).decode("utf-8").strip().split(",")
                thr2, n2, deltams2, checksum2 = int(outp2[0]), int(outp2[1]), int(outp2[2]), float(outp2[3])
                outp3 = sp.check_output([executable, str(x), str(t)], shell=False).decode("utf-8").strip().split(",")
                thr3, n3, deltams3, checksum3 = int(outp3[0]), int(outp3[1]), int(outp3[2]), float(outp3[3])
                target.append(str(int((deltams1 + deltams2 + deltams3) / 3)))
                target.append("{:.6f}".format((checksum1 + checksum2 + checksum3) / 3))
        lines.append(",".join(target))

    with open(argv[4], "w") as f:
        f.write(first_line)
        for x in lines:
            f.write("{}\n".format(x))


if __name__ == '__main__':
    main()
