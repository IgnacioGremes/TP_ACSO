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
    ; Arguments: rdi = type (uint8_t), rsi = hash (char*)

    ; Step 1: Allocate memory for string_proc_node (32 bytes due to alignment)
    push rdi              ; Save type
    push rsi              ; Save hash
    mov rdi, 32           ; Size of string_proc_node
    call malloc           ; Call malloc; pointer returned in rax
    pop rsi               ; Restore hash
    pop rdi               ; Restore type

    ; Step 2: Check if allocation failed
    test rax, rax         ; Test if rax is 0 (NULL)
    jz .malloc_failed     ; Jump to failure handling if NULL

    ; Step 3: Initialize the structure fields
    mov qword [rax], 0    ; next = NULL (offset 0)
    mov qword [rax + 8], 0 ; previous = NULL (offset 8)
    mov byte [rax + 16], dil ; type = argument type (dil = lower 8 bits of rdi)
    mov qword [rax + 24], rsi ; hash = argument hash (rsi)

    ; Step 4: Return the pointer (already in rax)
    ret

.malloc_failed:
    xor rax, rax          ; Set rax to 0 (NULL)
    ret

string_proc_list_add_node_asm:
    ; Arguments: rdi = list, rsi = type (uint8_t), rdx = hash (char*)

    ; Step 1: Call string_proc_node_create_asm(type, hash)
    push rdi              ; Save list pointer
    mov rdi, rsi          ; Move type to first argument (rdi)
    mov rsi, rdx          ; Move hash to second argument (rsi)
    call string_proc_node_create_asm ; Returns new node in rax
    pop rdi               ; Restore list pointer

    ; Step 2: Check if new_node is NULL
    test rax, rax         ; Test if rax is 0 (NULL)
    jz .return            ; If NULL, return

    ; Step 3: Check if list->first is NULL (empty list)
    mov r8, rax           ; Save new_node pointer in r8
    mov r9, [rdi]         ; Load list->first into r9
    test r9, r9           ; Test if list->first is NULL
    jz .empty_list        ; If NULL, handle empty list

    ; Step 4: Non-empty list case
    mov r10, [rdi + 8]    ; Load list->last into r10
    mov [r8 + 8], r10     ; new_node->previous = list->last
    mov [r10], r8         ; list->last->next = new_node
    mov [rdi + 8], r8     ; list->last = new_node
    jmp .return

.empty_list:
    ; Step 5: Empty list case
    mov [rdi], r8         ; list->first = new_node
    mov [rdi + 8], r8     ; list->last = new_node

.return:
    ret

string_proc_list_concat_asm:
    ; Arguments: rdi = list, rsi = type (uint8_t), rdx = hash (char*)

    ; Step 1: Initialize result and current
    mov r8, rdx           ; result = hash (r8 holds result)
    mov r9, [rdi]         ; current = list->first (r9 holds current)
    mov r10, rsi          ; Save type in r10
    mov r11, rdx          ; Save original hash in r11 for comparison

.loop:
    ; Step 2: Check if current is NULL
    test r9, r9           ; Test if current is NULL
    jz .done              ; If NULL, exit loop

    ; Step 3: Compare current->type with type
    movzx rax, byte [r9 + 16] ; Load current->type (1 byte) into rax (zero-extended)
    cmp rax, r10          ; Compare with type
    jne .next             ; If not equal, skip to next node

    ; Step 4: Call str_concat(result, current->hash)
    mov rdi, r8           ; First arg: result
    mov rsi, [r9 + 24]    ; Second arg: current->hash
    push r8               ; Save result
    push r9               ; Save current
    push r10              ; Save type
    push r11              ; Save original hash
    call str_concat       ; Returns new string in rax
    pop r11               ; Restore original hash
    pop r10               ; Restore type
    pop r9                ; Restore current
    pop r8                ; Restore result

    ; Step 5: Free intermediate result if necessary
    cmp r8, r11           ; Compare result with original hash
    je .skip_free         ; Skip free if result == hash
    mov rdi, r8           ; Arg for free: result
    push rax              ; Save new result
    push r9               ; Save current
    push r10              ; Save type
    push r11              ; Save original hash
    call free             ; Free old result
    pop r11               ; Restore original hash
    pop r10               ; Restore type
    pop r9                ; Restore current
    pop rax               ; Restore new result

.skip_free:
    mov r8, rax           ; Update result with new string

.next:
    ; Step 6: Move to next node
    mov r9, [r9]          ; current = current->next
    jmp .loop

.done:
    ; Step 7: Return result
    mov rax, r8           ; Return value in rax
    ret

