all:
	g++ monitor.cpp -o monitor -O3 -L/opt/vc/lib -I/usr/local/include -lraspicam -lmmal -lmmal_core -lmmal_util
