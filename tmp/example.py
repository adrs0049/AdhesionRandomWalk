import cb

class CB(cb.Callback):
    def __init__(self):
        super(CB, self).__init__()
    def call(self, x):
        print("Hello from CB")
        print(x)

def foo(x):
    print("hello from foo")
    print(x)

class Bar:
    def __call__(self, x):
        print("Hello from bar!")
        print(x)

o = cb.ObjWithPyCallback()
mycb = CB()
o.setCallback(mycb)
o.call()

o.setCallback(foo)
o.call()

o.setCallback(Bar())
o.call()
