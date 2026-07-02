CXX = g++
CXXFLAGS = -O3 -Wall -std=c++17

TARGETS = testador_completo main_hecc main_feistel cifra_arquivo hecc_rede

all: $(TARGETS)

testador_completo: testador_completo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

main_hecc: main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

main_feistel: main_feistel.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

cifra_arquivo: cifra_arquivo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

hecc_rede: hecc_rede.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS) *.o *.dat *.txt *.pub