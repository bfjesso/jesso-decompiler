int sub(int a, int b) 
{
	return a - b;
}

int __attribute__((stdcall)) multiply(int a, int b)
{
	return a * b;
}

int __attribute__((fastcall)) divide(int a, int b)
{
	return a / b;
}

int addAndMultiply(int a, int b)
{
	return a * (a + b);
}

int returnCall(int a, int b) 
{
	return addAndMultiply(a, b) * 9;
}

int foo(int a, int b, int c)
{
	int d = (a * b) + (b + c);

	if (d < 10)
	{
		int e = 12 - b;
		return e + c;
	}
	else 
	{
		int f = c * 4;
		return f - a;
	}
}

float types()
{
	float f = 3.14;
	double d = 420.69;

	char c = 1;
	short s = 2;
	int i = 3;
	long long l = 4;

	unsigned int ui = 5;

	int* p = &i;

	return f;
}

short bar(float b, short* s) 
{
	char c = 79;
	float d = b + 0.5;
	return *s + c;
}

struct MyStruct 
{
	int i;
	float f;
	char c;
};

char structFunc(struct MyStruct* myStruct) 
{
	myStruct->i = 420;
	myStruct->f = 3.14;
	myStruct->c = 69;
	return 1;
}

int condition(int a, int b)
{
	if (a > b)
	{
		return a * 3;
	}
	else
	{
		return b + 27;
	}
}

int condition2(float f, char c) 
{
	if (f > 7.0) 
	{
		return c + 1;
	}
	else if (f > 5.0) 
	{
		return c + 2;
	}
	else if (f > 3.0) 
	{
		return c + 3;
	}
	else 
	{
		return c + 4;
	}
}

short condition3(char c) 
{
	if (c > 2 && c < 5) 
	{
		return 1;
	}
	else 
	{
		return 2;
	}
}

short condition4(char c)
{
	if (c == 1 || c == 3)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

short condition5(char c)
{
	if (c != 6 && c > 2 && c < 10)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

short condition6(char c)
{
	if (c == 6 || c == 9 || c == 10)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

short condition7(char c)
{
	if (c > 2)
	{
		if (c != 5) 
		{
			return 1;
		}
		else 
		{
			return 2;
		}
	}
	else
	{
		return 3;
	}
}

int main() 
{
	int a = 14;
	int b = 7;
	short c = 9;

	int diff = sub(a, b);
	int product = multiply(a, b);
	int quotient = divide(a, b);
	int sumProd = addAndMultiply(a, b);
	int called = returnCall(a, b);
	int food = foo(a, b, 3);
	types();
	short bard = bar(23.1, &c);

	struct MyStruct myStruct;
	structFunc(&myStruct);

	int cond = condition(a, b);
	int cond2 = condition2(3.14, c);
	short cond3 = condition3(c);
	short cond4 = condition4(c);
	short cond5 = condition5(c);
	short cond6 = condition6(c);
	short cond7 = condition7(c);
	
	return 0;
}
