#!/usr/bin/python

import datetime
from db import *

from sqlalchemy import Column, Integer, BigInteger
from sqlalchemy import String, DateTime, Time, Numeric
from sqlalchemy import ForeignKey
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
    
    diffusion_coeff = Column(Numeric(19, 4, asdecimal=True))
    drift_coeff = Column(Numeric(19, 4, asdecimal=True))
    stepSize = Column(Numeric(19, 4, asdecimal=True))
    DomainEndL = Column(Numeric(19, 4, asdecimal=True))
    DomainEndR = Column(Numeric(19, 4, asdecimal=True))
    
    # domain subdivisions
    DomainN = Column(Integer)
    
    created_date = Column(DateTime, nullable=False)
    last_updated_date = Column(DateTime, nullable=False,
        onupdate=datetime.datetime.now)

    # define relationship
    #exchange = relationship("Exchange", backref=backref('exchange',
    #    order_by=id))

    """ Method to create a new Parameter object """
    def __init__(self, Diffusion, Drift, stepsize, DomainShape, DomainN):
        self.diffusion_coeff = Diffusion
        self.drift_coeff = Drift
        self.stepSize = stepsize
        self.DomainEndL = DomainShape[0]
        self.DomainEndR = DomainShape[1]
        self.DomainN = DomainN

        self.created_date = datetime.datetime.utcnow()
        self.last_updated_date = datetime.datetime.utcnow()

    def __repr__(self):
        return "<Parameters(Diffusion='%s', Drift='%s', StepSize='%s', DomainL='%s',\
        DomainR='%s'>" % (self.diffusion_coeff, self.drift_coeff, self.stepSize, \
        self.DomainEndL, self.DomainEndR)


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
    time_point = Column(Numeric(20, 12, asdecimal=True))

    # define relationship
    parameter = relationship("Parameters", backref=backref('random_walk',
        order_by=id))

    machine_info = relationship("MachineInfo", backref=backref('random_walk',
        order_by=id))

    def __repr__(self):
        return "<RandomWalk(description='%s', date='%s', version='%s', \
        final_time='%s')>" \
        % (self.description, self.date, self.program_version, self.final_time)

    def __init__(self, desc, version, final_time):
        self.description = desc
        self.simulation_date = datetime.datetime.utcnow()
        self.created_date = datetime.datetime.utcnow()
        self.last_updated_date = datetime.datetime.utcnow()
        self.program_version = version
        self.time_point = final_time
        self.final_time = final_time

class PathData(Base):
    __tablename__ = 'path_data'

    id = Column(Integer, primary_key=True, nullable=False, autoincrement=True)
    randomwalk_id = Column(Integer, ForeignKey('random_walk.id'))
     
    coord_x = Column(BigInteger)
    coord_y = Column(BigInteger)
    coord_z = Column(BigInteger)
    
    population = Column(BigInteger)

    # relationships
    random_walk = relationship("RandomWalk", backref=backref('path', 
        order_by=id))

    #def __repr__(self):
    #    return "<DailyPrice(symbol_id='%s', price_date='%s')" %\
    #            (self.symbol_id, self.price_date)

