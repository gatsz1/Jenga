JENGA PROGRAMMING LANGUAGE

This is a swahili based programming language.

---------------------------------------------------------
Building and Running 
    Requirements:
        1. Gcc or Clang
        Run gcc -o Jenga Jenga.c (linux)
            gcc Jenga.c -o jenga.exe (windows)

    After compiling the source code 
    i.e You have a file name test.j (Jenga file extension)
    You basically run it as:
        On Linux
        ./Jenga test.j

        On windows
        Jenga.exe test.j

----------------------------------------------------------
Please refer to the examples below on running 
your first Jenga Based Program.

-----------------------------------------------------------
Keyword|English Equivalent|Meaning
-------|------------------|--------------------------------
anza   |{ / begin         |Start Block
maliza |} / end	          |End Block
namba  |int               |Number
safu   |array	          |Array (Row)
andika |print	          |Write/Print
wakati |while	          |While
fanya  |do	              |Do
kama   |if	              |If
sivyo  |else	          |Else (Not so)
chagua |switch	          |Choose
kisa   |case	          |Case (Incident)
kawaida|default	          |Default (Normal)
------------------------------------------------------------

Example 1:The Structure Must start with Anza end with Maliza

    anza

    andika("Habari Dunia");

    maliza

Example 2:Variables
    anza
    namba x = 10;
    namba y = 20;
    andika(x + y);
    maliza

Example 3:if/else statements

    kama (x > 10) anza
    andika("Kubwa");
    maliza sivyo anza
    andika("Ndogo");

Example 4:Loops

    namba i = 0;
    wakati (i < 5) anza
    andika(i);
    i = i + 1;
    maliza

Example 5: Switch
    
    chagua (x) anza
    kisa 1: andika("Moja");
    kisa 2: andika("Mbili");
    kawaida: andika("Nyingine");
    maliza 

Example 6: Arrays

    safu a = safu(5); 
    a[0] = 100;        
    andika(a[0]); 

Example 7: Hesabu.j

anza
    namba i = 1;
    namba sum = 0;

    andika("Kuhesabu 1 mpaka 10");

    wakati (i < 11) anza
        sum = sum + i;
        i = i + 1;
    maliza

    andika("Jumla ni:");
    andika(sum);
maliza


--------------------------------------------------
N/B: For Now Jenga Does not support some features and they will be 
added soon like functions etc

Happy Coding :)
