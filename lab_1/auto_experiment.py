import subprocess as sp
from sys import argv


def main():
    executable = argv[1]
    start_r = int(argv[2])
    end_r = int(argv[3])
    delta = int((end_r - start_r) / 10)
    print("n,time_ms,result")
    for x in range(start_r, end_r + 1, delta):
        sp.run([executable, str(x)], shell=False)


if __name__ == '__main__':
    main()
