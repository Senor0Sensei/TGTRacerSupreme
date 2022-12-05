GITBINARY := git
PINGURL := google.com
LIBRARYREPO := simulator_libraries

ifeq ($(OS),Windows_NT)	
	SHELL := CMD
endif

all: update
ifeq ($(OS),Windows_NT)	
	@cd $(LIBRARYREPO) && mingw32-make
else
	@cd $(LIBRARYREPO) && make
endif

update:

clean:
ifeq ($(OS),Windows_NT)	
	@cd $(LIBRARYREPO) && mingw32-make clean
else
	@cd $(LIBRARYREPO) && make clean
endif