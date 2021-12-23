CC  = g++
src = specfetch.cc
arg = -s
out = specfetch

build: $(src)
	$(CC) $(src) $(arg) -o $(out)