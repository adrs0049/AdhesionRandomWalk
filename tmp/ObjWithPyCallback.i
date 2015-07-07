%module(directors="1") cb
%{
    #include "Callback.h"
    #include "ObjWithPyCallback.h"
%}

%feature("director") Callback;
%feature("nodirector") ObjWithPyCallback;

%feature("pythonprepend") ObjWithPyCallback::setCallback(Callback&) %{
    if not isinstance(callback, Callback) and callable(callback):
        class CallableWrapper(Callback):
            def __init__(self, f):
                super(CallableWrapper, self).__init__()
                self.f_ = f
            def call(self, obj):
                self.f_(obj)

        callback = CallableWrapper(callback)
        callback.__disown__()
    elif isinstance(callback, Callback):
        callback.__disown__()

%}

%include "Callback.h"
%include "ObjWithPyCallback.h"
