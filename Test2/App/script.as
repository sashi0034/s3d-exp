﻿
namespace A{
    funcdef bool CALLBACK(int, int);
}

class A
{
    bool Cmp(int a, int b)
    {
        count++;
        return a > b;
    }
    int count = 0;
}

void main()
{
    A a;
    
    bool bb = true;
    int dfdf = bb ? 99 : 11;

    Print << dfdf;

    // Create the delegate for the A::Cmp class method
    A::CALLBACK @func = A::CALLBACK(a.Cmp);

    // Call the delegate normally as if it was a global function
    if( func(1,2) )
    {
        Print << ("The function returned true\n");
    }
    else
    {
        Print << ("The function returned false\n");
    }

    Print << ("The number of comparisons performed is ") << a.count << "\n";
}