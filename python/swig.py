
import simulator as s

# move somewhere else
class PickalableSWIG:
    def __setstate__(self, state):
        self.__init__(*state['args'])

    def __getstate__(self):
        return {'args' : self.args}

class PickalableParameters(s.Parameters, PickalableSWIG):

    def __init__(self, *args):
        self.args = args
        s.Parameters.__init__(self, *args)

class PickalableSimulator(s.Simulator, PickalableSWIG):

    def __init__(self, *args):
        self.args = args
        s.Simulator.__init__(self, *args)



