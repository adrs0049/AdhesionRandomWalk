import sqlalchemy

print "Using sqlalchemy version '%s'" % (sqlalchemy.__version__)

# declare a mapping
from sqlalchemy.ext.declarative import declarative_base
Base = declarative_base()

# define global vars
engine = None
session = None
query = None
metadata = Base.metadata
database_url = None
database = None

def setup(url="sqlite:///:memory:", databaseName=None):
    global engine
    global session
    global query
    global metadata
    global database
    global database_url

    if databaseName is None:
        print('Error database name has to be set')
        exit
    
    database = databaseName

    # let my models register them selfs
    # and fill Base.metadata
    import models

    database_url = url
    engine = sqlalchemy.create_engine(database_url, echo=True)

    engine.execute('CREATE DATABASE IF NOT EXISTS ' + database)
    engine.execute('USE ' + database)

    # create tables
    metadata.create_all(engine)

    # create session
    Session = sqlalchemy.orm.sessionmaker(bind=engine)

    # create and save session object to Session
    session = Session()
    query = session.query

    return session

