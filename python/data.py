

class DataObject(object):
    """
    An object to hold data. Motivated by a desire for a mutable namedtuple with
    default values. To use, subclass, and define __slots__.

    The default default value is None. To set a default value other than None,
    set the `default_value` class variable.

    Example:
        class Jello(DataObject):
            default_value = 'no data'
            __slots__ = (
                'request_date',
                'source_id',
                'year',
                'group_id',
                'color',
                # ...
            )
    """
    __slots__ = ()
    default_value = None

    def __init__(self, *args, **kwargs):
        # Set default values
        for att in self.__slots__:
            setattr(self, att, self.default_value)

        # Set attributes passed in as arguments
        for k, v in zip(self.__slots__, args):
            setattr(self, k, v)
        for k, v in kwargs.items():
            setattr(self, k, v)

    def asdict(self):
        return dict(
            (att, getattr(self, att)) for att in self.__slots__)

    def astuple(self):
        return tuple(getattr(self, att) for att in self.__slots__)

    def __repr__(self):
        return '{}({})'.format(
            self.__class__.__name__,
            ', '.join('{}={}'.format(
                    att, repr(getattr(self, att))) for att in self.__slots__))
            
            
