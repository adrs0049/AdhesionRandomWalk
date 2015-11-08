#!/usr/bin/python

import sys, os
import scipy.io as sio
import numpy as np

from randomWalk_db import *
from models import *

print(len(sys.argv))

assert len(sys.argv)==2, 'wrong cmd line options'

print(sys.argv)
input_file = sys.argv[1]

print('Reading from ', input_file, ' ...')

mat = sio.loadmat(input_file, squeeze_me=True, struct_as_record=False)
output = mat['output']

time = output.t
y = output.y

matlab_to_sql = dict(domainlength='DomainSize',
                     IC_p = 'ic_p',
                     IC_type = 'ic_type',
                     R = 'R',
                     gridcells = 'DomainN',
                     DiffCoeff = 'diffusion_coeff',
                     alpha = 'drift_coeff',
                     g_type = 'g_type',
                     u_0 = 'u0',
                     omega_type = 'omega_type',
                     omega_p = 'omega_p',
                     BCs = 'bcs')

# parameters from simulations
p = output.params

parameters = p.__dict__['_fieldnames']

temp_data = dict()
for parameter in parameters:
    temp_data[matlab_to_sql[parameter]] = getattr(p, parameter)

# BCs is not available in the actual parameters
temp_data['bcs']='pp'
temp_data['ic_weigth']=0.09

print(temp_data)

# open database connection
db = RandomWalkDB()

# get parameter object
p2 = db.param_create_if_not_exist(temp_data)

print('parameters=', p2)
# get the rw object
sim_id = db.createSimulation('MATLAB-0.1', p2.id)

# write path data to the database
# write it for each
for path, t in zip(y, time):
    #print('y[', np.around(t+1E-3, decimals=2), ']=', path, ' sim_id=', sim_id)
    db.storePath(path, np.around(t+1E-3, decimals=2), sim_id, '0.1', 0, stochastic=False)

