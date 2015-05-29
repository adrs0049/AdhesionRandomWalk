import db

from db import *
from models import *

if __name__ == '__main__':
    # setup database
    session = setup(url='mysql+mysqldb://adrs0061:it4fOmen@localhost',
            databaseName='RandomWalk')
