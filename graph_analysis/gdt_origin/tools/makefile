# ------------
# user targets
# ------------


all: s t d b c o 

s: simpletest
	(cd simpletest; $(MAKE)  HOST=$(HOST))

t: test
	(cd test; $(MAKE)  HOST=$(HOST))

d: designgraphconv
	(cd designgraphconv; $(MAKE)  HOST=$(HOST))
	
b: blag
	(cd blag; $(MAKE)  HOST=$(HOST))

o: orthobox
	(cd orthobox; $(MAKE) HOST=$(HOST) )
c:

	(cd gdtcommline; $(MAKE)  HOST=$(HOST))

ed:  
	(cd GraphEditor; ./compile.sh)


clean:
	(cd simpletest; $(MAKE) clean HOST=$(HOST))
	(cd test; $(MAKE) clean HOST=$(HOST))
	(cd designgraphconv; $(MAKE) clean HOST=$(HOST))
	(cd blag; $(MAKE) clean HOST=$(HOST))
	(cd gdtcommline; $(MAKE) clean HOST=$(HOST))
	(cd orthobox; $(MAKE) clean HOST=$(HOST))
	(cd GraphEditor; ./clean.sh)
	#(cd WDrawer; $(MAKE) clean HOST=$(HOST))
delete: 
	(cd simpletest; $(MAKE) delete HOST=$(HOST))
	(cd test; $(MAKE) delete HOST=$(HOST))
	(cd designgraphconv; $(MAKE) delete HOST=$(HOST))
	(cd blag; $(MAKE) delete HOST=$(HOST))
	(cd gdtcommline; $(MAKE) delete HOST=$(HOST))
	(cd orthobox; $(MAKE) delete HOST=$(HOST))
	(cd GraphEditor; clean.sh )
	#(cd WDrawer; $(MAKE) delete HOST=$(HOST))
	
