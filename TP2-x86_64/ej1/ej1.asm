; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0

section .data

section .text

global string_proc_list_create_asm
global string_proc_node_create_asm
global string_proc_list_add_node_asm
global string_proc_list_concat_asm

; FUNCIONES auxiliares que pueden llegar a necesitar:
extern malloc
extern free
extern str_concat


string_proc_list_create_asm:
    ; Step 1: Allocate 16 bytes for string_proc_list
    mov rdi, 16          ; Set rdi to 16 (size of the structure: 8 for first, 8 for last)
    call malloc          ; Call malloc; pointer to allocated memory is returned in rax

    ; Step 2: Check if allocation failed
    test rax, rax        ; Test if rax is 0 (NULL)
    jz .malloc_failed    ; If rax is 0, jump to failure handling

    ; Step 3: Initialize the structure fields to NULL
    mov qword [rax], 0   ; Set first (offset 0) to 0 (NULL)
    mov qword [rax + 8], 0 ; Set last (offset 8) to 0 (NULL)

    ; Step 4: Return the pointer (already in rax)
    ret

.malloc_failed:
    ; Handle allocation failure by returning NULL
    xor rax, rax         ; Set rax to 0 (NULL)
    ret

string_proc_node_create_asm:

string_proc_list_add_node_asm:

string_proc_list_concat_asm:

