#include <stdio.h>
#include <stdbool.h>
#include <page/page.h>
#include <stdlib.h>
#include <commons/string.h>
#include <cspecs/cspec.h>

context(page)
{
    describe("Testing Page"){
        it("Se serializa y se deserializa una pagina sin perdida de información"){
            t_pag *page = malloc(sizeof(*page));
    page->displacement = 0;
    page->page_number = 2;
    page->pid = 3;
    void *serialized_page = serialize_page(page);
    t_pag *deserialized_page = deserialize_page(serialized_page);
    should_int(deserialized_page->displacement) be equal to(0);
    should_int(deserialized_page->page_number) be equal to(2);
    should_int(deserialized_page->pid) be equal to(3);
    free(page);
    free(deserialized_page);
}
end
    it("Se serializa y se deserializa una pagina para el swap sin perdida de información")
{
    // t_pag_swap *page = malloc(sizeof(*page));
    // page->swap_block = 0;
    // page->page_content = "00000000000000000000000000000000";
    // page->page_info_size = strlen(page->page_content);
    t_pag_swap *page = page_for_swap(0, 32, "00000000000000000000000000000000");
    void *serialized_page = serialize_page_for_swap(page);
    t_pag_swap *deserialized_page = deserialize_page_for_swap(serialized_page);
    deserialized_page->page_content = (void *)strncat((char *)(deserialized_page->page_content), '\0', 1);
    should_int(deserialized_page->swap_block) be equal to(0);
    should_int(deserialized_page->page_info_size) be equal to(32);
    should_string(deserialized_page->page_content) be equal to("00000000000000000000000000000000");
    free(page->page_content);
    free(page);
}
end
    it("Se serializa y se deserializa una pagina para el mov_out_fs sin perdida de información")
{
    t_mov_out_fs *page = malloc(sizeof(*page));
    page->page_number = 0;
    page->displacement = 0;
    page->pid = 0;
    page->register_value = "00000000000000000000000000000000";
    page->size = strlen(page->register_value);
    void *serialized_page = serialize_page_for_mov_out_fs(page);
    t_mov_out_fs *deserialized_page = deserialize_page_for_mov_out_fs(serialized_page);
    should_int(deserialized_page->page_number) be equal to(0);
    should_int(deserialized_page->displacement) be equal to(0);
    should_int(deserialized_page->pid) be equal to(0);
    should_string(deserialized_page->register_value) be equal to("00000000000000000000000000000000");
    should_int(deserialized_page->size) be equal to(32);
    free(page->register_value);
    free(page);
}
end
}
end
}