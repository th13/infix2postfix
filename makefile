in2post: in2post.cpp stack.hpp
	g++ in2post.cpp -o in2post.x -std=c++11

test: test_stack.cpp stack.hpp
	g++ test_stack.cpp -o test_stack.x -std=c++11

test1: test_stack1.cpp stack.hpp
	g++ test_stack1.cpp -o ts.x -std=c++11

clean:
	rm *.o *.x
