all:
	@echo "Compiling memory..."
	make -C memory
	@echo "Memory compilation completed."
	
	@echo "Compiling filesystem..."
	make -C file-system
	@echo "Filesystem compilation completed."
	
	@echo "Compiling cpu..."
	make -C cpu
	@echo "CPU compilation completed."
	
	@echo "Compiling kernel..."
	make -C kernel
	@echo "Kernel compilation completed."
	
	@echo "Compiling utils..."
	make -C utils
	@echo "Utils compilation completed."
	
clean:
	@echo "Cleaning..."
	make clean -C memory
	make clean -C file-system
	make clean -C cpu
	make clean -C kernel
	make clean -C utils
