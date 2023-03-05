version 1.1

workflow Test {
    input {
        String name
    }

    String s1 = ""
    String s2 = ''
    String s3 = "hello"
    String s4 = "hello \"John\", how are you?"
    String s5 = 'hello'
    String s6 = 'hello \'John\', how are you?'
    String s7 = "hello 'John', how are you?"
    String s8 = 'hello "John", how are you?'
    String s9 = "hello ~{name}, how are you?"
    String s10 = "hello \"~{name}\", how are you?"
    String s11 = "hello ~{sep(" ", ['John', 'Doe'])}, how are you?"
}