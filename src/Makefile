CC=gcc
CFLAGS=-Wall -Werror -Wextra -std=c11 #-fsanitize=address -g
VALGRIND=valgrind --leak-check=full --leak-resolution=med -s
OS:= $(shell uname -s)
LINFLAGS=-lcheck -pthread -lrt -lm -lsubunit
#TC_FLAGS += $(shell pkg-config --cflags check) $(shell pkg-config --libs check)
SRCFILE = s21_convertation.c s21_arithmetic.c s21_comparison.c s21_other.c
OBJECTS = s21_convertation.o s21_arithmetic.o s21_comparison.o s21_other.o
TESTS =  s21_test.c

all: clean test

s21_decimal.a:
	$(CC) -c $(SRCFILE)
	@ar rcs s21_decimal.a $(OBJECTS)
	ranlib s21_decimal.a

test: s21_decimal.a
ifeq ($(OS), Darwin)
	gcc $(TESTS) s21_decimal.a  -o test $(shell pkg-config --cflags --libs check) -lcheck -lpthread #-fsanitize=address
	./test
	rm -rf *.o *.a
else
	gcc $(TESTS) s21_decimal.a -o test $(LINFLAGS)
	./test
	rm -rf *.o *.a
endif

gcov_report: clean
ifeq ($(OS), Darwin)
	gcc --coverage $(TESTS) -o test $(SRCFILE) $(shell pkg-config --cflags --libs check) -lcheck
	./test
	gcov -f s21_decimal.gcda
	lcov -t "s21_test" -o s21_test.info -c -d .   
	genhtml -o report s21_test.info
	open ./report/index.html
else
	# sudo apt-get install xdg-utils
	gcc --coverage $(TESTS) -o test $(SRCFILE) $(LINFLAGS)
	./test
	gcov -f s21_decimal.gcda
	lcov -t "s21_test" -o s21_test.info -c -d .  
	genhtml -o report s21_test.info
	xdg-open ./report/index.html
endif

check: clean cppcheck style leaks

cppcheck: test
	cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

# cpplint:
# 	cp ../materials/linters/CPPLINT.cfg CPPLINT.cfg
# 	python3 ../materials/linters/cpplint.py --extensions=c *.c *.h
# 	rm CPPLINT.cfg

style: test
	cp ../materials/linters/.clang-format .clang-format 
	# clang-format -style=google -i *.c *.h
	clang-format -style=google -n *.c *.h
	rm .clang-format 

leaks: test
ifeq ($(OS), Darwin)
	CK_FORK=no leaks --atExit -- ./test
	rm -rf s21_decimal.o
else
	# sudo apt install -y valgrind
	$(VALGRIND) --leak-check=full --show-leak-kinds=all ./test
	rm -rf s21_decimal.o
endif

clean:
	-rm -rf *.o && rm -rf *.gcno && rm -rf *.exec
	-rm -rf *.a && rm -rf *.gcda
	-rm -rf *.info && rm -rf *.gcov
	-rm -rf ./gcov_report
	-rm -rf ./run_tests ./test 
	-rm -rf ./report/
