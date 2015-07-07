#include "ObjWithPyCallback.h"
#include "Callback.h"

#include <iostream>

ObjWithPyCallback::ObjWithPyCallback() : callback_(nullptr) {}

ObjWithPyCallback::~ObjWithPyCallback() {}

void ObjWithPyCallback::setCallback(Callback& callback)
{
    callback_ = &callback;
}

void ObjWithPyCallback::call()
{
    if ( ! callback_ )
        std::cerr << "No callback is set!" << std::endl;
    else
        callback_->call(*this);

}
