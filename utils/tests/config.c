#include <stdio.h>
#include <stdbool.h>
#include <config/config.h>
#include <cspecs/cspec.h>

context (config) {
    describe("Testing Config") {
        it("Se puede crear utils y romperlo mientras no haya memory leak") {
            const char* data[] = {"hola", "tests/mocks/test.config"};
            t_utils* utils = create_initial_config(2, data, "tests/mocks/logger.log", false, LOG_LEVEL_TRACE);
        	utils_destroy(utils);
        } end
    } end
}