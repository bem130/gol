random:
	g++ random.cpp -o random -O3
	./random
win:
	g++ randomwinapi.cpp -lgdi32 -o randomwinapi -O3
	./randomwinapi