
import simulator as s
from randomWalk_db import *
from models import *
from adhesion_1d import *

import numpy as np

if __name__ == '__main__':

    # TODO read this from an XML file
    param = dict(DomainSize=10, DomainN=32,
                 diffusion_coeff=1.0, drift_coeff=20.0,
                 R=1.0, omega_type=s.OMEGA_TYPE_UNIFORM,
                 omega_p=0.42, g_type=1,
                 u0=0.8, bcs='pp', ic_type=s.IC_TYPE_DELTA, ic_p=0.1,
                 rw_type=s.RANDOMWALK_TYPE_ADHESION,
                 space_type=s.SPACE_TYPE_ALWAYS_FREE,
                 adhesivity_type=s.ADHESIVITY_TYPE_SIMPLE)

    player = Player(param)

    player.runSimulations(np.arange(0.00001,1.0,0.25), 10)

    player.setNewDB()

    player.doHistogram(Compare=True)


