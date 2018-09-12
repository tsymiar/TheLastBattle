# #######################################################################################
# DbgLib make file
# Author: Martin Fleisz
# #######################################################################################

# files required to compile corefx
files =	DebugFx.cpp DebugHelp.cpp MemLeakDetector.cpp Utils.cpp

ofiles = $(files:.cpp=.o)

dumpfiles = CoreDumper.c ELFCore.c LinuxThreads.c ThreadLister.c
odumpfiles = $(dumpfiles:.c=.o)


# 32-bit ANSI DEBUG build
dbglibad:	$(files) dump
			g++ -D_DEBUG -rdynamic -ggdb -c $(files)
			ar rcs ./Bin/libdbglibad.a $(ofiles) $(odumpfiles)
			mv *.o ./Debug/. -f

# 32-bit UNICODE DEBUG build
dbglibud:	$(files) dump
			g++ -D_DEBUG -D_UNICODE -rdynamic -ggdb -c $(files)
			ar rcs ./Bin/libdbglibud.a $(ofiles) $(odumpfiles)
			mv *.o ./Debug/. -f

# 32-bit ANSI RELEASE build
dbgliba:	$(files) dump
			g++ -rdynamic -O3 -c $(files)
			ar rcs ./Bin/libdbgliba.a $(ofiles) $(odumpfiles)
			mv *.o ./Release/. -f

# 32-bit UNICODE RELEASE build
dbglibu:	$(files) dump
			g++ -rdynamic -D_UNICODE -O3 -c $(files)
			ar rcs ./Bin/libdbglibu.a $(ofiles) $(odumpfiles)
			mv *.o ./Release/. -f


# core dumper files - need special compiler arguments
dump:		$(dumpfiles)
			gcc -DHAVE_CONFIG_H -I. -fno-omit-frame-pointer -g -O2 -MT CoreDumper.lo -MD -MP -MF CoreDumper.Tpo -c CoreDumper.c  -fPIC -DPIC -o CoreDumper.o 
			gcc -DHAVE_CONFIG_H -I. -fno-omit-frame-pointer -g -O2 -MT ThreadLister.lo -MD -MP -MF ThreadLister.Tpo -c ThreadLister.c  -fPIC -DPIC -o ThreadLister.o 
			gcc -DHAVE_CONFIG_H -I. -fno-omit-frame-pointer -g -O2 -MT ELFCore.lo -MD -MP -MF ELFCore.Tpo -c ELFCore.c  -fPIC -DPIC -o ELFCore.o 
			gcc -DHAVE_CONFIG_H -I. -fno-omit-frame-pointer -g -O2 -MT LinuxThreads.lo -MD -MP -MF LinuxThreads.Tpo -c LinuxThreads.c  -fPIC -DPIC -o LinuxThreads.o 
clean:
			rm ./Bin/*
			rm ./Debug/*
			rm ./Release/*
