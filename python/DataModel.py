from peewee import *
from datetime import *

db = SqliteDatabase('myapp.db')

class BaseModel(Model):
    class Meta:
        database = db

class Parameters(BaseModel):
    parameterId = PrimaryKeyField()
    DiffusionP = FloatField()
    AdhesionP = FloatField()
    SensingR = FloatField()
    DomainL = FloatField()
    StepSize = FloatField()
    TimeStep = FloatField()

# Todo add number of steps var
class Runs(BaseModel):
    runId = PrimaryKeyField()
    NumberOfSteps = IntegerField()
    created_date = DateTimeField(default=datetime.now)
    parameterId = ForeignKeyField(Parameters, related_name='Runs')
    
class FinalPos(BaseModel):
    runId = ForeignKeyField(Runs, related_name='FinalPos')
    cellId = IntegerField()
    pos = IntegerField()
    
class PathData(BaseModel):
    runId = ForeignKeyField(Runs, related_name='PathData')
    cellId = IntegerField()
    step = IntegerField()
    

    