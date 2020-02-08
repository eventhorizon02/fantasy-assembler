assembler:assembler.o file_io.o utills.o hash.o globals.o error_handling.o first_pass.o second_pass.o
	gcc -g globals.o assembler.o error_handling.o first_pass.o second_pass.o file_io.o utills.o hash.o -Wall -ansi -pedantic -std=c99 -o assembler

assembler.o:assembler.c
	gcc -c -g assembler.c -ansi -Wall -pedantic -std=c99

file_io.o:file_io.c file_io.h
	gcc -c -g file_io.c -ansi -Wall -pedantic -std=c99

utills.o:utills.c utills.h
	gcc -c -g utills.c -ansi -Wall -pedantic -std=c99

hash.o: hash.c hash.h
	gcc -c -g hash.c -ansi -Wall -pedantic -std=c99

error_handling.o: error_handling.c error_handling.h
	gcc -c -g error_handling.c -ansi -Wall -pedantic -std=c99

first_pass.o: first_pass.c first_pass.h
	gcc -c -g first_pass.c -ansi -Wall -pedantic -std=c99

second_pass.o: second_pass.c second_pass.h
	gcc -c -g second_pass.c -ansi -Wall -pedantic -std=c99

globals.o: globals.c globals.h
	gcc -c -g globals.c -ansi -Wall -pedantic -std=c99

clean:
	rm *.o *.ob *.ent *.ext

	


