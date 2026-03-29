CC=gcc

C_FILES=main.c parser.c vector.c header.c response.c utils.c
C_OUTPUT=output

JS_DIR=./benchmark/js
PORT=3000

build:
	$(CC) $(C_FILES) -o $(C_OUTPUT)

car:
	$(CC) $(C_FILES) -o $(C_OUTPUT) && make run_c

install_js:
	cd $(JS_DIR) && npm install express

run_c:
	./$(C_OUTPUT)

run_js:
	cd $(JS_DIR) && node main.js

benchmark_c:
	@echo "Starting C server..."
	./$(C_OUTPUT) & \
	C_PID=$$!; \
	sleep 1; \
	echo "Running C benchmark (50k req, 200 concurrency)..."; \
	ab -n 50000 -c 200 http://localhost:$(PORT)/; \
	kill $$C_PID

benchmark_js:
	@echo "Starting JS server..."
	cd $(JS_DIR) && node main.js & \
	JS_PID=$$!; \
	sleep 2; \
	echo "Running JS benchmark (50k req, 200 concurrency)..."; \
	ab -n 50000 -c 200 http://localhost:4000/; \
	kill $$JS_PID

benchmark: build install_js
	@echo "===== C SERVER TEST ====="
	$(MAKE) benchmark_c
	@echo "\n===== JS SERVER TEST ====="
	$(MAKE) benchmark_js
