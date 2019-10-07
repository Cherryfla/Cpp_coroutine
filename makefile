cc = g++
CFLAGS = -std=c++11 -g -Wall

obj = coroutine.o sample.o
tar = sample

$(tar):$(obj) 
	$(cc) $(CFLAGS) $(obj) -o $(tar)

%.o:%.cpp
	$(cc) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -rf $(tar) $(obj)

