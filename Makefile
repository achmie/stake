IDIR=.
CXX=g++
CXFLAGS=-I$(IDIR) -O2

DEPS = crypto.h aes_locl.h

OBJ = aes_128.o aes_core.o arth.o secp192r1.o ecp.o ecc.o


%.o: %.cpp $(DEPS)
	$(CXX) $(CXFLAGS) -c $< -o $@

stake: main_stake.o $(OBJ)
	$(CXX) -o $@ $^ $(CXFLAGS) $(LIBS)

pki: main_pki.o $(OBJ)
	$(CXX) -o $@ $^ $(CXFLAGS) $(LIBS)

all: stake pki

.PHONY: clean

clean:
	rm -f *.o
