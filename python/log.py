
import datetime, os

def log_error(dest_q, message, name):
    dest_q.put(dict(type='error',
                    name=name,
                    timestamp=datetime.datetime.now(),
                    process=os.getpid(),
                    message=message))

def log_info(dest_q, message, name):
    dest_q.put(dict(type='info',
                    name=name,
                    timestamp=datetime.datetime.now(),
                    process=os.getpid(),
                    message=message))

def log_debug(dest_q, message, name):
    dest_q.put(dict(type='debug',
                    name=name,
                    timestamp=datetime.datetime.now(),
                    process=os.getpid(),
                    message=message))


