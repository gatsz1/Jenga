JENGA PROGRAMMING LANGUAGE

This is a Swahili based programming language.

---------------------------------------------------------
Building and Running 
    Requirements:
        1. GCC or Clang
        
    Compiling:
        Linux:   gcc Jenga.c -o Jenga
        Windows: gcc Jenga.c -o Jenga.exe

    Running:
    After compiling, if you have a file named test.j:
        Linux:   ./Jenga test.j
        Windows: Jenga.exe test.j

----------------------------------------------------------
Please refer to the examples below on running 
your first Jenga Based Program.

NOTE: Every Jenga program MUST start with 'anza' and end 
with 'maliza'. If you miss this, you will get a Syntax Error.

-----------------------------------------------------------
Keyword | English Equivalent | Meaning
------- | ------------------ | --------------------------------
anza    | { / begin          | Start Block
maliza  | } / end            | End Block
namba   | int                | Number
safu    | array              | Array (Row)
andika  | print              | Write/Print
wakati  | while              | While
fanya   | do                 | Do
kama    | if                 | If
sivyo   | else               | Else (Not so)
chagua  | switch             | Choose
kisa    | case               | Case (Incident)
kawaida | default            | Default (Normal)
kwa     | for                | For Loop
kweli   | true               | Boolean True
sikweli | false              | Boolean False
------------------------------------------------------------

Comments:
    // single-line comment
    /* multi-line
       comment */

Example 1: The Structure (Must start with Anza, end with Maliza)

    anza
        andika("Habari Dunia");
    maliza

Example 2: Variables

    anza
        namba my_number = 10;
        namba other_number = 20;
        andika(my_number + other_number);
    maliza

Example 3: If/Else Statements
    (Notice we declare 'x' inside the block first)

    anza
        namba x = 50;

        kama (x > 10) anza
            andika("Kubwa");
        maliza sivyo anza
            andika("Ndogo");
        maliza
    maliza

Example 4: Loops

    anza
        namba i = 0;
        
        wakati (i < 5) anza
            andika(i);
            i = i + 1;
        maliza
    maliza

Example 4b: For Loop

    anza
        kwa (namba i = 0; i < 5; i = i + 1) anza
            andika(i);
        maliza
    maliza

Example 5: Switch

    anza
        namba x = 2;

        chagua (x) anza
            kisa 1: andika("Moja");
            kisa 2: andika("Mbili");
            kawaida: andika("Nyingine");
        maliza 
    maliza

Example 6: Arrays

    anza
        safu a = safu(5); 
        a[0] = 10;
        a[1] = 20;
        
        andika(a[0] + a[1]);
    maliza

Example 7: String Concatenation

    anza
        andika("Habari " + "Dunia");
    maliza

Example 8: Booleans

    anza
        namba x = 1;
        kama (kweli) anza
            andika("True Branch");
        maliza sivyo anza
            andika("False Branch");
        maliza
    maliza
