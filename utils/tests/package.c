#include <stdio.h>
#include <stdbool.h>
#include <package/package.h>
#include <cspecs/cspec.h>

context (package) {
    describe("Testing Package") {
        it("Se puede crear un package y romperlo mientras no haya memory leak") {
            int op_code = 1;
        	t_package* package = create_empty_package(op_code);
            should_int(package->op_code) be equal to (op_code);
            should_int(package->size) be equal to (0);
            should_ptr(package->buffer) be equal to (NULL);
            destroy_package(package);
        } end
    } end
}