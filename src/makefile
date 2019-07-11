BUILD_DIR := ../Build

debug:  forceMake cleanForceMake
	cd $(BUILD_DIR); make -k debug; cp cstoreqp ../src/cstoreqptest

#stop if error	
debug2:  forceMake cleanForceMake
	cd $(BUILD_DIR); make debug; cp cstoreqp ../src/cstoreqptest

clean:  forceMake cleanForceMake
	rm cstoreqptest; cd $(BUILD_DIR); make -k clean 


data:	forceMake cleanForceMake
	cd ../data; ./getData
  
opt:  forceMake cleanForceMake
	export __CSTORE_OPTIMIZED__=1; \
	cd $(BUILD_DIR); make debug; cp cstoreqp ../src/cstoreqptestOpt

gprof:  forceMake cleanForceMake
	export __CSTORE_GPROF__=1; \
	cd $(BUILD_DIR); make debug; cp cstoreqp ../src/cstoreqptestGprof

papi: forceMake cleanForceMake
	export __CSTORE_PAPI__=1; \
	export __CSTORE_OPTIMIZED__=1; \
	cd $(BUILD_DIR); make debug; cp cstoreqp ../src/cstoreqptestPAPI

essence: forceMake cleanForceMake
	export _CSTOREESSENCE_=1; \
	cd $(BUILD_DIR); make debug; cp cstoreqp ../src/cstoreqptest

forceMake:
	touch forceMake

cleanForceMake:
	rm forceMake
