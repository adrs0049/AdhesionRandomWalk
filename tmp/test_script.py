
import test

def f(x):
    print(x)

test.register_handler(test.THING, f)
test.test(test.THING)
