all:
	$(MAKE) -C lib

.PHONY: clean
clean:
	$(MAKE) clean -C lib
