#This makefile builds the UIMACPP documentation
#Requires Doxygen 1.3.6 and Graphviz 1.8.10 installed and 
#PATH environment variable must include
#doxygen/bin;graphviz/bin;graphviz/bin/tools
#Must be run from the uimacpp/build subdirectory

#use these to run in WIN environment
#RM=RD /s /q
#DEL=del
#MDFILES=..\docs\html\*.md5
#MAPFILES=..\docs\html\*.map
#DOCDIR=..\docs

#use these to run in LINUX environment
RM=rm -rf
DEL=rm
MDFILES=../docs/html/*.md5
MAPFILES=../docs/html/*.map
DOCDIR=../docs

DOXYGENCMD=doxygen

build : ../docs/html/index.html
	
../docs/html/index.html : uimacpp.dox
	@echo building docs....  
	$(DOXYGENCMD) uimacpp.dox
	$(DEL) $(MDFILES)
	$(DEL) $(MAPFILES)

rebuild : clean build

clean : 
	@echo deleting $(DOCDIR)/html 
	$(RM) $(DOCDIR)/html
	@echo deleting tags file
	$(RM) $(DOCDIR)/uimadoxytags.tag



