To compile this program, run:

    gcc -std=gnu99 -pthread -o line_processor main.c mem.h mem.c

In the directory. Then you can run it by executing either

    1.  ./line_processor
    2.  ./line_processor > output.txt
    3.  ./line_processor < input.txt
    4.  ./line_processor > output.txt < input.txt

Everything should work flawlessly. Thank you!