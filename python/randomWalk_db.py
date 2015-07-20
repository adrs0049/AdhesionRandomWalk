import db

from db import *
from models import *

from sqlalchemy import exists
from sqlalchemy.sql.expression import func
from sqlalchemy.orm import with_polymorphic
import socket
import datetime
import numpy as np
import pandas as pd
import platform
import multiprocessing as mp

# can we solve this better?
cls_type = sqlalchemy.ext.declarative.api.DeclarativeMeta

class Error(Exception):
    """ Base class for exceptions in randomWalk_db module """
    pass

class InvalidEntry(Error):
    def __init__(self, cls, expression):
        self.cls = cls
        self.expression = expression
        self.tablename = self.cls.__tablename__

class CommitError(Error):
    def __init__(self, table):
        self.table = table

class RandomWalkDB(object):

    def __init__(self, databaseName='RandomWalk'):

        self.dbName = databaseName

        databaseURL = 'mysql+mysqldb://adrs0061:it4fOmen@localhost'

        # setup database
        try:
            self.engine, self.Session = setup(url=databaseURL,\
                                          databaseName=self.dbName)
        except ImportError as e:
            # use mysql connector
            databaseURL = 'mysql+mysqlconnector://adrs0061:it4fOmen@localhost'
            self.engine, self.Session = setup(url=databaseURL,\
                                            databaseName=self.dbName)
        except:
            raise

        # join the path tables
        self.data_entity = with_polymorphic(PathData, [DensityData, StateData])

    def getSession(self): return self.Session()
    def getEngine(self): return self.engine

    def commit(self):
        self.session.commit()

    def add_and_commit(self, obj):
        session = self.getSession()
        try:
            session.add(obj)
        except:
            session.rollback()
            raise
        else:
            session.commit()

    # TODO reimplement me better!!!
    def ParametersToDict(self, p):
        raise NotImplementedError()

    """ create new Parameter from dictionary """
    def createNewParameters(self, p):
        ret = Parameters(**p)
        assert ret.is_valid(), 'created Parameters is not valid!'
        return ret

    """ check that a model is valid """
    def is_valid(self, obj):
        # TODO maybe do a very general implementation here ? instead of in
        # each of the model defns?
        return obj.is_valid()

    """ Query for parameters with filters based on dictionary """
    def queryExistsParam(self, P):
        assert isinstance(P, dict), 'Argument P has to be a dictionary'

        session = self.getSession()
        q = session.query(Parameters)
        q = q.filter(exists())
        q = self.queryParamBuilder(q, **P)

        return q

    """ check if a record exists for data passed as a dictionary """
    def isParameter(self, P):
        return self.queryParam(P).scalar()

    """ Create Parameter entry in DB if it doesn't exist
        param is a dictionary containing some or all values
        of the Parameter object """
    def param_create_if_not_exist(self, param):
        q = self.queryExistsParam(param)

        if not q.scalar():
            parameters = self.createNewParameters(param)
            self.add_and_commit(parameters)
        else:
            print('Parameters already exists in the database')
            # TODO check that this works
            parameters = q.all()[0]

        return parameters

    def queryParam(self, **kwargs):
        session = self.getSession()
        q = session.query(Parameters)
        return self.queryParamBuilder(q, **kwargs)

    def queryParamBuilder(self, query, **kwargs):

        for attr, value in kwargs.items():
            query = query.filter(getattr(Parameters, attr).__eq__(value))

        return query

    """ get the right field name from Path data """
    def getDataType(self, cls):
        for field in ['population', 'density']:
            if hasattr(cls, field):
                return field

    """ Method to store a path generated by simulator in the database
        cls:         name of ORM object
        stateVector: Simulation data
        SimTime:     time of stateVector
        SimId:       simulation id has to be a valid entry in the database
        stochastic:  was it a stochastic simulation or PDE?
        version:     program version used to create this path
    """
    def storePath(self, stateVector, SimTime, simId,
                  version, steps, stochastic=True, cls=None):

        assert isinstance(stateVector, np.ndarray), "This is only tested with \
                numpy arrays"

        sim = self.getFromId(Simulation, simId)
        assert sim, "SimId:" + SimId + " not found in database!"

        metadata = PathMetaData(stochastic=stochastic,
                                time=SimTime,
                                steps=steps,
                                simulation_date = datetime.datetime.utcnow(),
                                program_version = version)

        sim.paths.append(metadata)

        if not cls:
            if issubclass(stateVector.dtype.type, np.integer):
                cls = StateData
            elif issubclass(stateVector.dtype.type, np.float):
                cls = DensityData
            else:
                assert False, "cls is not set + unknown dtype of stateVector"

        # TODO we would probably survive without storing all the
        # coordinates for all the points!! very expensive
        session = self.getSession()
        try:
            for state in stateVector:
                path_point = cls()
                setattr(path_point, self.getDataType(cls), state)

                metadata.path.append(path_point)
        except:
            session.rollback()
            raise
        else:
            session.commit()

    """ create simulation """
    def createSimulation(self, description, paramId):

        hostname = socket.gethostname()
        machine_id = self.queryMachineRecord(hostname)

        sim = Simulation(description)

        session = self.getSession()
        # Query for both the parameters and machine info to link
        p = session.query(Parameters).get(paramId)
        assert p, 'Queried parameter record does not exist. Add it!'

        m = session.query(MachineInfo).get(machine_id)
        assert m, 'Queried machine record does not exist. Add it!'

        m.simulations.append(sim)
        p.simulations.append(sim)
        session.commit()

        # return the id to the created sim object
        return sim.id

    """ Query database table by id """
    def getFromId(self, cls, id):
        assert isinstance(id, int), "id has to be an integer"
        assert isinstance(cls, cls_type), "cls has to be sqlalchemy type"
        session = self.getSession()
        ret = session.query(cls).get(id)

        if ret is None:
            raise InvalidEntry(cls, id)
        else:
            return ret

    """ Query for paramters with id """
    def getParamFromId(self, paramId):
        return self.getFromId(Parameters, paramId)

    """ Query for path with id """
    def getPathFromId(self, pathId):
        return self.getFromId(PathData, pathId)

    """ Query simulation by id """
    def getSimulationFromId(self, simId=None):
        if simId is None:
            simId = self.getMostRecentSimulation()

        return self.getFromId(Simulation, simId)

    """ Query for highest id in cls """
    def getMostRecent(self, cls):
        assert isinstance(cls, cls_type), "cls has to be sqlalchemy type"
        session = self.getSession()
        return session.query(func.max(cls.id)).scalar()

    """ get most recent randomwalk """
    def getMostRecentSimulation(self):
        return self.getMostRecent(Simulation)

    """ Create new machine info record """
    def createMachineRecord(self):
        machine = MachineInfo(socket.gethostname())
        machine.python_version = platform.python_version()
        machine.compiler = platform.python_compiler()
        machine.release = platform.release()
        machine.system = platform.system()
        machine.processor = platform.processor()
        machine.cpu_count = mp.cpu_count()
        machine.interpreter = platform.architecture()[0]

        self.add_and_commit(machine)

    """ Query machine info record """
    def queryMachineRecord(self, hostname):
        session = self.getSession()
        stmt = exists().where(MachineInfo.name == hostname)
        q = session.query(MachineInfo.name).filter(stmt)

        # if it doesn't exist, create a new record
        if not q.scalar():
            self.createMachineRecord()

        # Now that the machine record exists, return its id
        return  session.query(MachineInfo.id).filter(MachineInfo.name ==
                hostname).first()

    def get_attr(self, l):
        for attr in ['population', 'density']:
            if hasattr(l, attr): return attr

        return None

    def returnPath(self, metadataId):
        session = self.getSession()
        q = session.query(self.data_entity).\
                filter(PathData.metadata_id==metadataId) #.\

        q.add_columns(DensityData.density, StateData.population)

        res = q.all()
        attr = self.get_attr(res[0])
        ret = [getattr(x, attr) for x in res]

        # check what type the array should be
        if attr == 'population':
            return np.array(ret, dtype=np.integer)

        # else it has to be float anyways
        return np.array(ret)

    def returnPathsForSim(self, simId=None):

        if simId is None or simId == -1:
            simId = self.getMostRecentSimulation()
            print('Simulation id is not set, using most recent with id %d' \
                  % (simId))

        # check that sim exists in the database
        try:
            sim = self.getSimulationFromId(simId)
        except InvalidEntry as e:
            print('Simulation id %d does not exist in database!' % self.runId)
            return
        except:
            raise

        session = self.getSession()
        q = session.query(PathMetaData.time).\
                filter(PathMetaData.simulation_id==simId)

        time_pts = [float(x[0]) for x in q.all()]

        assert len(time_pts)>0, "More than one time point is required"

        q = session.query(PathMetaData).\
                filter(PathMetaData.simulation_id==simId)

        # list of dataframes
        ddf = dict()

        for time in time_pts:
            # dataframe for return
            rdf = pd.DataFrame()

            q = session.query(PathMetaData).\
                filter(PathMetaData.simulation_id==simId).\
                filter(PathMetaData.time==time)

            idx=0
            total_steps = 0
            for metadata in q.all():
                rdf[idx]=self.returnPath(metadata.id)
                total_steps += int(metadata.steps)
                idx+=1

            total_steps /= idx
            rdf['avg']=rdf.mean(axis=1)
            ddf[time] = rdf

        return ddf, total_steps

# debugging
if __name__ == '__main__':
    db = RandomWalkDB()

    # TODO read this from an XML file
    param = dict(DomainSize=10, DomainN=10,
                 diffusion_coeff=0.5, drift_coeff=0.0,
                 R=1.0, omega_type=1, omega_p=0.42, g_type=1,
                 u0=0.8, bcs='pp', ic_type=1, ic_p=0.1)


