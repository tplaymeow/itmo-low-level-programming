section .text
 
 
; Принимает код возврата и завершает текущий процесс
exit: 
    mov rax, 60
    syscall

; Принимает указатель на нуль-терминированную строку, возвращает её длину
string_length:
    xor rax, rax
    .loop:
        cmp byte [rdi+rax], 0
        je .loop_end
        inc rax
        jmp .loop
    .loop_end:
        ret

; Принимает указатель на строку и длину строки, выводит её в stdout
print_string_with_length:
    mov rdx, rsi
    mov rsi, rdi
    mov rax, 1
    mov rdi, 1
    syscall
    ret

; Принимает указатель на нуль-терминированную строку, выводит её в stdout
print_string:
    push rdi
    call string_length
    pop rdi
    mov rsi, rax
    jmp print_string_with_length

; Принимает код символа и выводит его в stdout
print_char:
    push rdi
    mov rdx, 1
    mov rax, 1
    mov rsi, rsp
    mov rdi, 1
    syscall
    pop rdi
    ret

; Переводит строку (выводит символ с кодом 0xA)
print_newline:
    mov rdi, 0xA
    jmp print_char

; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
    push rbx
    xor rcx, rcx
    mov rbx, 10
    mov rax, rdi

    .loop:
        xor rdx, rdx
        div rbx

        add dl, '0'

        dec rsp
        mov [rsp], dl
        inc rcx

        test rax, rax
        jnz .loop
    
    .loop_end:
        mov rdi, rsp
        mov rsi, rcx
        push rcx
        call print_string_with_length
        pop rcx
        add rsp, rcx
        pop rbx
        ret

; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
    cmp rdi, 0
    jge print_uint
    
    push rdi
    mov rdi, '-'
    call print_char
    pop rdi
    neg rdi
    jmp print_uint

; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
string_equals:
    ; string_length  с сохранением регистров
    push rdi
    push rsi
    call string_length
    pop rsi
    pop rdi

    ; увеличиваем размер массива на 1, чтобы сравнить и нуль-терминаторы,
    ; чтоб проверить, что строки имеют одинакувую днину
    inc rax
    mov rcx, rax
    repe cmpsb
    jnz .not_equal

    mov rax, 1          ; если стороки равны
    ret

    .not_equal:         ; если строки не равны
        xor rax, rax
        ret

; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
    push 0
    xor rax, rax
    mov rdi, 0
    mov rsi, rsp
    mov rdx, 1
    syscall
    pop rax
    ret 

; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор
read_word:
    xor rdx, rdx
    .main_loop:
        ; read_char с сохранением регистров
        push rdi
        push rsi
        push rdx
        call read_char
        pop rdx
        pop rsi
        pop rdi

        ; проверка на пробельные символы
        cmp rax, 0x20
        je .on_whitespace
        cmp rax, 0x9
        je .on_whitespace
        cmp rax, 0xA
        je .on_whitespace

        ; проверка на ошибку переполнения буффера
        cmp rdx, rsi
        je .on_error
        ; проверка на конец ввода
        test rax, rax
        je .end

        mov byte [rdi + rdx], al
        inc rdx
        jmp .main_loop
    
    .on_whitespace:     ; обработка пробельных символов
        cmp rdx, 0      ; если символы в начале строки,
        je .main_loop   ; переходим на следующую итерацию
        jmp .end        ; в обратном случае, заканчиваем чтение

    .on_error:
        xor rax, rax
        ret

    .end:
        mov rax, rdi
        mov byte [rdi + rdx], 0
        ret

; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
    xor rdx, rdx
    xor rax, rax
    xor rcx, rcx
    mov r9, 10
    .loop:
        mov cl, byte [rdi + rdx]
        ; берем первый символ строки
        ; проверяем что символ является числом
        cmp cl, '0'
        jl .end
        cmp cl, '9'
        jg .end
        ; переводим символ в число
        sub cl, '0'
    
        ; добавляем цифру в конец итогового числа:
        ; res = (res * 10) + digit
        push rdx
        mul r9
        pop rdx
        add rax, rcx

        inc rdx
        jmp .loop

    .end:
        ret

; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
    mov cl, byte [rdi]
    cmp cl, '-'
    je .negative
    cmp cl, '+'
    je .positive

    jmp parse_uint

    .negative:
        inc rdi
        call parse_uint
        cmp rdx, 0
        je .exit
        neg rax
        inc rdx
    .exit:
        ret
    
    .positive:
        inc rdi
        call parse_uint
        inc rdx
        ret

; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
    call string_length      ; Проверяем помещается ли строка в буффер
    cmp rdx, rax
    jge string_copy_unsafe  ; если да, переходим к `string_copy_unsafe`
    xor rax, rax            ; если нет, возвращаем 0
    ret

; Принимает указатель на строку, указатель на буффер, длину строки
; Копирует строку в буффер
; Длина строки и буффера не проверяется
string_copy_unsafe:
    push rdi
    push rsi
    pop rdi
    pop rsi
    inc rdx
    mov rcx, rdx
    rep movsb
    ret
