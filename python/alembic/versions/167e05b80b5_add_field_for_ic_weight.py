"""add field for ic_weight

Revision ID: 167e05b80b5
Revises: 19089cd961e8
Create Date: 2015-10-27 11:15:52.621578

"""

# revision identifiers, used by Alembic.
revision = '167e05b80b5'
down_revision = '19089cd961e8'
branch_labels = None
depends_on = None

from alembic import op
import sqlalchemy as sa


def upgrade():
    op.add_column('parameters', sa.Column('ic_weigth', sa.Numeric(19,7,
                                                                  asdecimal=True)))

def downgrade():
    op.drop_column('parameters', 'ic_weigth')
