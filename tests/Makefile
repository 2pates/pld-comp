PYTHON=python3
PROGRAM=ifcc-test.py
FOLDER=testfiles
OUTPUT=ifcc-test-output
TMP_OUTPUT_FILE=output.txt

all:
	$(PYTHON) $(PROGRAM) $(FOLDER)

check:
	rm -rf $(OUTPUT)
	$(PYTHON) $(PROGRAM) $(FOLDER) > $(TMP_OUTPUT_FILE)
	@if grep -q "FAIL" $(TMP_OUTPUT_FILE) ; \
	then \
		echo "Some tests failed" ; \
		exit 1 ; \
	else \
		echo "All tests passed" ; \
		exit 0 ; \
	fi
	rm $(TMP_OUTPUT_FILE)

clean:
	rm -rf $(OUTPUT)




