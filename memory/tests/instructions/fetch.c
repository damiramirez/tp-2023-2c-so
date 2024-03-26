#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <instruction_memory/instructions/instructions.h>
#include <instruction_memory/process/process.h>
#include <initial_configuration/memory_config.h>
#include <pcb/pcb.h>
#include <cspecs/cspec.h>

context(fetch)
{
    describe("Testing Fetch Phase") {
        t_log* logger = log_create("./tests/tests-inst.log", "TEST", false, LOG_LEVEL_INFO);
        t_config* config = config_create("./config/memory.config");
        char *test_instructions[17] = {
            "SET AX 1",
            "SET BX 1",
            "SUM AX BX",
            "SUB AX BX",
            "MOV_IN DX 0",
            "MOV_OUT 0 CX",
            "WAIT RA",
            "SLEEP 10",
            "JNZ AX 4",
            "SIGNAL RA",
            "F_OPEN ARCHIVO W",
            "F_TRUNCATE ARCHIVO 64",
            "F_SEEK ARCHIVO 8",
            "F_WRITE ARCHIVO 0",
            "F_READ ARCHIVO 0",
            "F_CLOSE ARCHIVO",
            "EXIT"
        };
        char* instruction;
        int i = 0;

        before {
            init_memory_config(config);
            init_real_memory();
            active_processes = list_create();
            create_process(logger, crear_pcb(1, "1", 0, 0), 0);
        }end

        it("All instructions were obtained successfully") {
            do {
                instruction = fetch_next_instruction(1, i, logger);
                
                if(!string_is_empty(instruction)) should_string(instruction) be equal to(test_instructions[i]);
                i++;
            } while(i < 17);
        }end
    }end
}