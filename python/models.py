#!/usr/bin/python

import datetime
from db import *

from sqlalchemy import Column, Integer, BigInteger
from sqlalchemy import String, DateTime, Time, Numeric
from sqlalchemy import ForeignKey, Boolean
from sqlalchemy.orm import relationship, backref

class MachineInfo(Base):
    __tablename__ = 'machine_info'

    id = Column(Integer, primary_key=True, nullable=False, autoincrement=True)
    name = Column(String(64), nullable=False)
    created_date = Column(DateTime, nullable=False)
    last_updated_date = Column(DateTime, nullable=False,
        onupdate=datetime.datetime.now)

    # relationships
    simulations = relationship("Simulation", backref="MachineInfo")

    def __init__(self, name):
        self.name = name
        self.created_date = datetime.datetime.utcnow()
        self.last_updated_date = datetime.datetime.utcnow()

class Parameters(Base):
    __tablename__ = 'parameters'

    id = Column(Integer, primary_key=True, nullable=False, autoincrement=True)

    # model parameters
    diffusion_coeff = Column(Numeric(19, 4, asdecimal=True), nullable=False)
    drift_coeff = Column(Numeric(19, 4, asdecimal=True), nullable=False)
    # the sensing radius
    R = Column(Numeric(19, 4, asdecimal=True), nullable=False)

    # Domain parameters
    # DomainN stores the value per unit length
    DomainN = Column(Integer, nullable=False)
    DomainSize = Column(Numeric(19, 4, asdecimal=True), nullable=False)

    # properties of omega
    omega_type = Column(Integer, nullable=False)
    omega_p = Column(Numeric(19, 4, asdecimal=True), nullable=False)

    # volume filling threshold in g()
    g_type = Column(Integer, nullable=False)
    # u_max in volume filling term
    u0 = Column(Numeric(19, 4, asdecimal=True), nullable=False)

    # boundary conditions
    bcs = Column(String(3), nullable=False)

    # ICs
    ic_type = Column(Integer, nullable=False)
    ic_p = Column(Numeric(19, 4, asdecimal=True), nullable=False)

    # book keeping when things were created
    created_date = Column(DateTime, nullable=False)
    last_updated_date = Column(DateTime, nullable=False,
        onupdate=datetime.datetime.now)

    # relationships
    simulations = relationship("Simulation", backref="Parameters")

    """ debug function to check if all values are set """
    def is_valid(self):
        for k in self.__mapper__.all_orm_descriptors.keys():
            # id will be taken care of on commit
            if k == '__mapper__' or k == 'id':
                continue
            if getattr(self, k) is None:
                print(k, ' is None')
                return False

        return True

    """ Method to create a new Parameter object """
    def __init__(self, *args, **kwargs):

        print('kwargs=', kwargs)
        print('args=', args)

        keys_ok = set(Parameters.__dict__)
        for k in kwargs:
            if k in keys_ok:
                print('k=', k, ' ', kwargs[k])
                setattr(self, k, kwargs[k])

        # move all the time things to a different base class
        self.created_date = datetime.datetime.utcnow()
        self.last_updated_date = datetime.datetime.utcnow()

    def __repr__(self):
        return "<Parameters(Diffusion='%s', Drift='%s', DomainN='%s', R='%s',\
        DomainSize='%s'>" % (self.diffusion_coeff, self.drift_coeff, \
                self.DomainN, self.R, self.DomainSize)

class Simulation(Base):
    __tablename__ = 'simulation'

    id = Column(Integer, primary_key=True)

    parameter_id = Column(Integer, ForeignKey('parameters.id'))
    machine_id = Column(Integer, ForeignKey('machine_info.id'))

    description = Column(String(255))

    created_date = Column(DateTime, nullable=False)
    last_updated_date = Column(DateTime, nullable=False,
        onupdate=datetime.datetime.now)

    # define relationship
    paths = relationship("PathMetaData")

    def __repr__(self):
        return "<Simulation(description='%s', date='%s')>" \
        % (self.description, self.created_date)

    def __init__(self, desc):
        self.description = desc
        self.created_date = datetime.datetime.utcnow()
        self.last_updated_date = datetime.datetime.utcnow()

class PathMetaData(Base):
    __tablename__ = 'path_meta_data'

    id = Column(Integer, primary_key=True, nullable=False, autoincrement=True)
    simulation_id = Column(Integer, ForeignKey('simulation.id'))

    # path
    path = relationship("PathData")

    time = Column(Numeric(20, 9, asdecimal=True), nullable=False)
    stochastic = Column(Boolean, nullable=False)
    simulation_date = Column(DateTime, nullable=False)
    program_version = Column(String(64), nullable=False)

    def __repr__(self):
        return "<PathMetaData(id='%s', simId='%s', time='%s', stochastic='%s',\
                simulation_date='%s', program_version='%s')>" \
                % (self.id, self.simulation_id, self.time, self.stochastic, \
                   self.simulation_date, self.program_version)

# data is stored from left to right in the domain
class PathData(Base):
    __tablename__ = 'path_data'

    id = Column(Integer, primary_key=True, nullable=False, autoincrement=True)
    type = Column(String(50))

    metadata_id = Column(Integer, ForeignKey('path_meta_data.id'))

    # relationships
    #simulation = relationship("Simulation", backref=backref('path',
    #    order_by=id))

    #pathMetaData = relationship("PathMetaData", order_by=id)

    __mapper_args__ = {
        'polymorphic_identity':'path_data',
        'polymorphic_on':type
    }

    def __repr__(self):
        return "<PathData(metadataId='%s', type='%s')>" % (self.metadata_id,\
                                                           self.type)

# store data from a PDE simulation which is a decimal value
class DensityData(PathData):
    __tablename__ = 'density_data'

    id = Column(Integer, ForeignKey('path_data.id'), primary_key=True)
    density = Column(Numeric(10, 8, asdecimal=True), nullable=False)

    __mapper_args__ = {
        'polymorphic_identity':'density_data',
    }

    def __repr__(self):
        return "<DensityData(density='%s')>" % (self.density)

# store the statevector of a stochastic simulation which are integers
class StateData(PathData):
    __tablename__ = 'state_data'

    id = Column(Integer, ForeignKey('path_data.id'), primary_key=True)
    population = Column(BigInteger, nullable=False)

    __mapper_args__ = {
        'polymorphic_identity':'state_data',
    }

    def __repr__(self):
        return "<StateData(state='%s')>" % (self.population)

