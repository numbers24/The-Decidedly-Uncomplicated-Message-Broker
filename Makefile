all:
	make client;
	make serve;

client:
	gcc -o DUMBclient DUMBclient.c

serve:
	gcc -o DUMBserve DUMBserver.c

clean:
	make clean client;
	make clean serve;
clean client:
	rm DUMBclient
clean serve:
	rm DUMBserve
