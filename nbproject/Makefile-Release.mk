#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/src/BluetoothServerSocket.o \
	${OBJECTDIR}/src/BluetoothSocket.o \
	${OBJECTDIR}/src/Conditional.o \
	${OBJECTDIR}/src/EventProcessorAsync.o \
	${OBJECTDIR}/src/Keyboard.o \
	${OBJECTDIR}/src/Lockable.o \
	${OBJECTDIR}/src/Mouse.o \
	${OBJECTDIR}/src/PacketHandler.o \
	${OBJECTDIR}/src/PacketReaderAsync.o \
	${OBJECTDIR}/src/Screen.o \
	${OBJECTDIR}/src/ServiceRegistrer.o \
	${OBJECTDIR}/src/Thread.o \
	${OBJECTDIR}/src/Utils.o \
	${OBJECTDIR}/src/suinput.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bluetoothremote

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bluetoothremote: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bluetoothremote ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/src/BluetoothServerSocket.o: src/BluetoothServerSocket.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/BluetoothServerSocket.o src/BluetoothServerSocket.cpp

${OBJECTDIR}/src/BluetoothSocket.o: src/BluetoothSocket.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/BluetoothSocket.o src/BluetoothSocket.cpp

${OBJECTDIR}/src/Conditional.o: src/Conditional.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Conditional.o src/Conditional.cpp

${OBJECTDIR}/src/EventProcessorAsync.o: src/EventProcessorAsync.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/EventProcessorAsync.o src/EventProcessorAsync.cpp

${OBJECTDIR}/src/Keyboard.o: src/Keyboard.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Keyboard.o src/Keyboard.cpp

${OBJECTDIR}/src/Lockable.o: src/Lockable.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Lockable.o src/Lockable.cpp

${OBJECTDIR}/src/Mouse.o: src/Mouse.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Mouse.o src/Mouse.cpp

${OBJECTDIR}/src/PacketHandler.o: src/PacketHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/PacketHandler.o src/PacketHandler.cpp

${OBJECTDIR}/src/PacketReaderAsync.o: src/PacketReaderAsync.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/PacketReaderAsync.o src/PacketReaderAsync.cpp

${OBJECTDIR}/src/Screen.o: src/Screen.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Screen.o src/Screen.cpp

${OBJECTDIR}/src/ServiceRegistrer.o: src/ServiceRegistrer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ServiceRegistrer.o src/ServiceRegistrer.cpp

${OBJECTDIR}/src/Thread.o: src/Thread.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Thread.o src/Thread.cpp

${OBJECTDIR}/src/Utils.o: src/Utils.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Utils.o src/Utils.cpp

${OBJECTDIR}/src/suinput.o: src/suinput.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/suinput.o src/suinput.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/bluetoothremote

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
