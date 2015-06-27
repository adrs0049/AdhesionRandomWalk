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

    """ debug function to check if all values are set """
    def is_valid(self):
        for k in self.__mapper__.all_orm_descriptors.keys():
            # id will be taken care of on commit
            if k == '__mapper__' or k == 'id':
                continue
            if getattr(self, k) is None:
                print k, ' is None'
                return False

        return True

    """ Method to create a new Parameter object """
    def __init__(self, *args, **kwargs):

        print 'kwargs=', kwargs
        print 'args=', args

        keys_ok = set(Parameters.__dict__)
        for k in kwargs:  
            if k in keys_ok:
                print 'k=', k, ' ', kwargs[k]
                setattr(self, k, kwargs[k])

        # move all the time things to a different base class
        self.created_date = datetime.datetime.utcnow()
        self.last_updated_date = datetime.datetime.utcnow()

    def __repr__(self):
        return "<Parameters(Diffusion='%s', Drift='%s', DomainN='%s', R='%s',\
        DomainSize='%s'>" % (self.diffusion_coeff, self.drift_coeff, \
                self.DomainN, self.R, self.DomainSize)

class RandomWalk(Base):
    __tablename__ = 'random_walk'

    id = Column(Integer, primary_key=True, nullable=False, autoincrement=True)

    parameter_id = Column(Integer, ForeignKey('parameters.id'))
    machine_id = Column(Integer, ForeignKey('machine_info.id'))

    description = Column(String(255))

    program_version = Column(String(64), nullable=False)

    simulation_date = Column(DateTime, nullable=False)
    created_date = Column(DateTime, nullable=False)
    last_updated_date = Column(DateTime, nullable=False,
        onupdate=datetime.datetime.now)

    final_time = Column(Numeric(19, 4, asdecimal=True))

    # boolean value to distinguish between stochastic and cont sims
    # check whether this works as expected
    stochastic = Column(Boolean, unique=False)

    # define relationship
    parameter = relationship("Parameters", backref=backref('random_walk',
        order_by=id))

    machine_info = relationship("MachineInfo", backref=backref('random_walk',
        order_by=id))

    def __repr__(self):
        return "<RandomWalk(description='%s', date='%s', version='%s', \
        final_time='%s')>" \
        % (self.description, self.created_date, self.program_version, self.final_time)

    def __init__(self, desc, version, final_time, boolType=False):
        self.description = desc
        self.stochastic = boolType
        self.simulation_date = datetime.datetime.utcnow()
        self.created_date = datetime.datetime.utcnow()
        self.last_updated_date = datetime.datetime.utcnow()
        self.program_version = version
        self.time_point = final_time
        self.final_time = final_time

class PathMetaData(Base):
    __tablename__ = 'path_meta_data'

    id = Column(Integer, primary_key=True, nullable=False, autoincrement=True)

    time = Column(Numeric(19, 4, asdecimal=True), nullable=False)
    stochastic = Column(Boolean, nullable=False)

class PathData(Base):
    __tablename__ = 'path_data'

    id = Column(Integer, primary_key=True, nullable=False, autoincrement=True)
    randomwalk_id = Column(Integer, ForeignKey('random_walk.id'))
    metadata_id = Column(Integer, ForeignKey('path_meta_data.id'))

    coord_x = Column(BigInteger)
    coord_y = Column(BigInteger)
    coord_z = Column(BigInteger)

    population = Column(Numeric(10, 8, asdecimal=True), nullable=False)

    # relationships
    random_walk = relationship("RandomWalk", backref=backref('path', 
        order_by=id))

    pathMetaData = relationship("PathMetaData", order_by=id)

    #def __repr__(self):
    #    return "<DailyPrice(symbol_id='%s', price_date='%s')" %\
    #            (self.symbol_id, self.price_date)

