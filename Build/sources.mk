C_SRCS := 
C_UPPER_SRCS := 
CC_SRCS := 
CXX_SRCS := 
CPP_SRCS := 
S_SRCS := 
S_UPPER_SRCS :=
INTERMEDIATES := 

# Every subdirectory with source files must be described here
SUBDIRS := \
. \
common \
sparseexps \
Writers \
Wrappers/Util \
Wrappers/Encoder \
Wrappers/Decoder \
Wrappers \
Util \
Operators \
AM \
UnitTests \
compressionexps \

ifndef _CSTOREESSENCE_
SUBDIRS += \
materialexps \
TM \
plan \
plan/Nodes \
plan/UNodes \
plan/BNodes \
plan/MNodes \
plan/SNodes \
plan/Plans \
plan/Util \
parser \
parser/BExpressions \
parser/BExpressions/BBinaryLogic \
parser/BExpressions/BComparison \
parser/Expressions \
parser/Expressions/EAgg \
parser/Expressions/EArithmetics \
parser/Expressions/ENumbers \
parser/Queries \
OptPlan 
endif

