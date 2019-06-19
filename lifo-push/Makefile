PGMS = lifo-push lifo-push-uintptr

all: $(PGMS)

lifo-push: lifo-push.cpp lifo-stress.hpp
	g++ -g -std=c++11 -o lifo-push lifo-push.cpp -lpthread

lifo-push-uintptr: lifo-push-uintptr.cpp lifo-stress.hpp
	g++ -g -std=c++11 -o lifo-push-uintptr lifo-push-uintptr.cpp -lpthread
