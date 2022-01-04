tmd: render.c tmd.c
	@cc -o tmd ./render.c ./tmd.c

clean:
	@rm tmd
