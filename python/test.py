import sys, os, time
from subprocess import Popen, list2cmdline

from multiprocessing import Process, Lock

def cpu_count():
    num = 1

    if sys.platform == 'win32':
        try:
            num = int(os.environ['NUMBER_OF_PROCESSORS'])
        except(ValueError, KeyError):
            pass
    elif sys.platform == 'darwin':
        try:
            num = int(os.popen('sysctl -n hw.ncpu').read())
        except ValueError:
            pass
    else:
        try:
            num = os.sysconf('SC_NPROCESSORS_ONLN')
        except(ValueError, OSError, AttributeError):
            pass

    return num

def info(title):
    print(title)
    print('module name:', __name__)
    if hasattr(os, 'getppid'):
        print('parent process:', os.getppid())
    print('process id', os.getpid())

def f(l, name):
    l.acquire()
    info('function f')
    print('hello', name)
    l.release()

if __name__ == '__main__':
    lock = Lock()

    for num in range(10):
        Process(target=f, args=(lock, num)).start()

