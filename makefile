
LIB = gopts/libgopts.a

.PHONY: $(LIB)
$(LIB):
	$(MAKE) -C gopts

.PHONY: test
test:
	$(MAKE) -C test

.PHONY: clean
clean:
	$(MAKE) clean -C gopts
	$(MAKE) clean -C test

