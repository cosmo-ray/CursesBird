NAME= "Curses Bird"



build:	main.o
	gcc -o ${NAME} main.o -lcurses

clean:
	rm main.o

fclean: clean
	${NAME}
