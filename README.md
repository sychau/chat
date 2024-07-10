# s-talk
CMPT300 Assignment 2 2024-02-23
CMPT 300 X1 Groups
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
Henry Chau, syc54
Ryan Taban, rat2

Use GNU17 instead of ISO C (C99, C11, C17) to access some function e.g. gethostname
Avoid using ctrl+C or ctrl+D interrupt to allow the program clean up properly
Undefine behaviour if buffer overflow(input too long)

Usage: ./s-talk [portNum] [remote machine name] [remote port number]
Quit: enter "!"

Build: make
Clean: make clean

# Test locally
Terminal A: ./s-talk 44444 127.0.0.1 55555
Terminak B: ./s-talk 55555 127.0.0.1 44444

# Memory test locally
Terminal A: valgrind --leak-check=full --show-leak-kinds=all ./s-talk 44444 127.0.0.1 55555
Terminal B: valgrind --leak-check=full --show-leak-kinds=all ./s-talk 55555 127.0.0.1 44444

# Test long string:
We produce about two million dollars for each hour we work.  Thefifty hours is one conservative estimate for how long it we taketo get any etext selected, entered, proofread, edited, copyrightsearched and analyzed, the copyright letters written, etc.  Thisprojected audience is one hundred million readers.  If our valueper text is nominally estimated at one dollar, then we produce 2million dollars per hour this year we, will have to do four textWe produce about two million dollars for each hour we work.  Thefifty hours is one conservative estimate for how long it we taketo get any etext selected, entered, proofread, edited, copyrightsearched and analyzed, the copyright letters written, etc.  Thisprojected audience is one hundred million readers.  If our valueper text is nominally estimated at one dollar, then we produce 2million dollars per hour this year we, will have to do four text