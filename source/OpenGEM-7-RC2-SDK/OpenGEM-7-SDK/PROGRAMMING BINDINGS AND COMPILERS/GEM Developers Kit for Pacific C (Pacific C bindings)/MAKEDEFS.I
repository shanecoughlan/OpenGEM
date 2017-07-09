#
# Where is Pacific installed on your system?
#
# (nb: some programs require the forward slashes, and others require
#     the backward sort).
#
CCDIR=D:\TOOLS\C
CCDIRF=D:/TOOLS/C
#
# Location of PPD include files. This is necessary if the PPD directory has
# more than one component in it; as it is on mine, being D:/TOOLS/C. PACC 
# would incorrectly generate a path of D:/TOOLS/INCLUDE rather than 
# D:/TOOLS/C/INCLUDE
#
STDINC=-I$(CCDIRF)/INCLUDE
STDLIB=-L$(CCDIRF)/LIB
