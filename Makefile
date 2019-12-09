all:
	make client;
	make serve;

client:
	gcc -o DUMBclient DUMBclient.c

serve:
	gcc -o DUMBserve DUMBserver.c

clean:
	rm DUMBclient;
	rm DUMBserve;
