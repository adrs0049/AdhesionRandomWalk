import sqlalchemy

print("Using sqlalchemy version '%s'" % (sqlalchemy.__version__))

# declare a mapping
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import scoped_session, sessionmaker
Base = declarative_base()

# define global vars
engine = None
Session = None
query = None
metadata = Base.metadata
database_url = None
database = None

def setup(url="sqlite:///:memory:", databaseName=None, verbose=False):
    global engine
    global Session
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
    engine = sqlalchemy.create_engine(database_url, echo=verbose)

    try:
        engine.execute('CREATE DATABASE IF NOT EXISTS ' + database)
    except:
        print("Database already exists")

    engine.execute('USE ' + database)

    # create tables
    metadata.create_all(engine)

    # factory
    session_factory = sessionmaker(bind=engine)

    # create session
    Session = scoped_session(session_factory)

    return engine, Session

