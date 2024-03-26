#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <initial_configuration/memory_config.h>
#include <user_memory/real_memory.h>
#include <user_memory/paging/pages.h>
#include <instruction_memory/process/process.h>
#include <cspecs/cspec.h>

context(pages) {
    // describe("Testing page fault") {
    // }end
    describe("Testing page replacement algorithms") {
        t_log* logger = log_create("./tests/tests-pag.log", "TEST", false, LOG_LEVEL_INFO);
        t_config* config = config_create("./config/memory.config");
        
        before {
            init_memory_config(config);
            init_real_memory();
            init_sorter_thread();
            active_processes = list_create();
            create_process(logger, crear_pcb(1, "1", 256, 1), 0);
            create_process(logger, crear_pcb(2, "2", 160, 1), 0);
        }end
        
        it("Testing sorting by fifo algorithm (with one process)") {
            if(string_equals_ignore_case(memory_config.algorithm, "FIFO")) {
                int page_numbers[14] = { 0, 1, 7, 2, 3 };
                t_page_entry* page;
                int i = 0;

                reference_page(1,0,logger);
                reference_page(1,1,logger);
                reference_page(1,7,logger);
                reference_page(1,2,logger);
                reference_page(1,3,logger);
                reference_page(1,2,logger);
                reference_page(1,7,logger);
                reference_page(1,1,logger);
                reference_page(1,0,logger);
                reference_page(1,3,logger);
                reference_page(1,0,logger);
                reference_page(1,2,logger);
                reference_page(1,3,logger);
                reference_page(1,1,logger);

                should_bool(list_size(pages_to_replace) == 0) be falsey;

                while(i != list_size(pages_to_replace)) {
                    page = list_get(pages_to_replace, i);
                    should_int(page->page_number) be equal to(page_numbers[i]);
                    i++;
                }

                working = 0;
                sem_post(&sort_pages);
            }
        }end
        it("Testing sorting by lru algorithm (with one process)") {
            if(string_equals_ignore_case(memory_config.algorithm, "LRU")) {
                int page_numbers[14] = { 7, 0, 2, 3, 1 };
                t_page_entry* page;
                int i = 0;

                reference_page(1,0,logger);
                reference_page(1,1,logger);
                reference_page(1,7,logger);
                reference_page(1,2,logger);
                reference_page(1,3,logger);
                reference_page(1,2,logger);
                reference_page(1,7,logger);
                reference_page(1,1,logger);
                reference_page(1,0,logger);
                reference_page(1,3,logger);
                reference_page(1,0,logger);
                reference_page(1,2,logger);
                reference_page(1,3,logger);
                reference_page(1,1,logger);

                should_bool(list_size(pages_to_replace) == 0) be falsey;

                while(i != list_size(pages_to_replace)) {
                    page = list_get(pages_to_replace, i);
                    should_int(page->page_number) be equal to(page_numbers[i]);
                    i++;
                }
                working = 0;
                sem_post(&sort_pages);
            }
        }end
    }end
}
