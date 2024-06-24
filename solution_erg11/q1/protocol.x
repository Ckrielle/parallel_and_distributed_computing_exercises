struct text_input {
    string text<256>;
    int offset;
};

program PROTOCOL_PROG {
    version PROTOCOL_VERS {
        string UPPERCASE(text_input) = 1;
        string LOWERCASE(text_input) = 2;
        string ROT(text_input) = 3;
        string UNROT(text_input) = 4;
    } = 1;
} = 0x33113377;
