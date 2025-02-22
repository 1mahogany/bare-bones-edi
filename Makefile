all:
	$(MAKE) -C lib

.PHONY: tests
tests:
	$(MAKE) -C tests

.PHONY: clean
clean:
	$(MAKE) clean -C lib
	$(MAKE) clean -C tests
