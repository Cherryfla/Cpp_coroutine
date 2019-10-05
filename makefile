cc = g++
CFLAGS = -std=c++11 -g -Wall

tar:sample
obj:coroutine.o sample.o

$(tar):$(obj) 
	$(cc) $(CFLAGS) $(obj) -o $(tar) $(lib)

%.o:%.cpp
	$(cc) $(CFLAGS) -c $< -o $@

.PHONY:clean
	rm $(tar) $(obj)

