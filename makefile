all:	
	gcc -o web_fork web_fork.c
	gcc -o web_select web_select.c
clean:
	rm -i web_fork web_select
