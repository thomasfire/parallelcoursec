import subprocess as sp
from sys import argv


def main():
    executable = argv[1]
    start_r = int(argv[2])
    end_r = int(argv[3])
    threads = int(argv[4])
    delta = int((end_r - start_r) / 10)
    for x in range(start_r, end_r + 1, delta):
        sp.run([executable, str(x), str(threads)], shell=False)


if __name__ == '__main__':
    main()
