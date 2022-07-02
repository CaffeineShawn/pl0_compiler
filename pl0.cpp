// pl0 compiler source code

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <ctime>

#include "pl0.h"
#include "set.cpp"

//////////////////////////////////////////////////////////////////////
// print error message.
void error(int n)
{
	int i;

	printf("      ");
	for (i = 1; i <= cc - 1; i++)
		printf(" ");
	printf("^\n");
	printf("Error %3d: %s\n", n, err_msg[n]);
	err++;
} // error

//////////////////////////////////////////////////////////////////////
void getch()
{
	if (cc == ll)
	{
		if (feof(infile))
		{
			printf("\nPROGRAM INCOMPLETE\n");
			exit(1);
		}
		ll = cc = 0;
		printf("%5d  ", cx);
		while ( (!feof(infile)) && ((ch = getc(infile)) != '\n'))
		{
			printf("%c", ch);
			line[++ll] = ch;
		} // while
		printf("\n");
		line[++ll] = ' ';
	}
	ch = line[++cc];
} // getch

//////////////////////////////////////////////////////////////////////
// gets a symbol from input stream.
void getsym()
{
	int i, k;
	char a[MAXIDLEN + 1];

	while (ch == ' ' || ch == '\t')
		getch();
	
	if (isalpha(ch))
	{ // symbol is a reserved word or an identifier.
		k = 0;
		do
		{
			if (k < MAXIDLEN)
				a[k++] = ch;
			getch();
		}
		while (isalpha(ch) || isdigit(ch));
		a[k] = 0;
		strcpy(id, a);
		word[0] = id;
		i = NRW;
		while (strcmp(id, word[i--]) != 0);
		if (++i)
			sym = wsym[i]; // symbol is a reserved word
		else
			sym = SYM_IDENTIFIER;   // symbol is an identifier
	}
	else if (isdigit(ch))
	{ // symbol is a number.
		k = num = 0;
		sym = SYM_NUMBER;
		do
		{
			num = num * 10 + ch - '0';
			k++;
			getch();
		}
		while (isdigit(ch));
		if (k > MAXNUMLEN)
			error(25);     // The number is too great.
	}
	else if (ch == ':')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_BECOMES; // :=
			getch();
		}
		else
		{
			sym = SYM_NULL;       // illegal?
		}
	}
	else if (ch == '>')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_GEQ;     // >=
			getch();
		}
		else
		{
			sym = SYM_GTR;     // >
		}
	}
	else if (ch == '<')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_LEQ;     // <=
			getch();
		} else {
			sym = SYM_LES;     // <
		}
	}
    else if (ch == '!')
    {
        getch();
        if (ch == '=') {
            sym = SYM_NEQ; // !=
            getch();
        } else {
            sym = SYM_NULL; // illegal
        }
    } else if (ch == '+') {
        getch();
        if (ch == '=') {
            sym = SYM_PLUSBY;
            getch();
        } else {
            sym = SYM_PLUS;
        }
    } else if (ch == '-') {
        getch();
        if (ch == '=') {
            sym = SYM_MINUSBY;
            getch();
        } else {
            sym = SYM_MINUS;
        }
    } else if (ch == '*') {
        getch();
        if (ch == '=') {
            sym = SYM_MULTIPLYBY;
			printf("保留字: SYM_MULTIPLYBY - MULTIPLYBY\n");
            getch();
        } else {
            sym = SYM_TIMES;
        }
    } else if (ch == '/') {
        getch();
        if (ch == '=') {
            sym = SYM_DIVIDEBY;
			printf("保留字: SYM_DIVIDEBY - DIVIDEBY\n");
            getch();
        } else if (ch == '*') {
            getch();
            char prev = ch;
            while (prev != '*' || ch != '/') {
                prev = ch;
                getch();
            }
            if (prev != '*' && ch != '/') {
                error(19);
            } else {
                getch();
                getsym();
            }
        } else if (ch == '/') {
            getch();
			while (cc != ll) {
				getch();
			}
			printf("保留字: SYM_NOTE - //\n");
            getsym();
        } else {
            sym = SYM_SLASH;
        }

    } else if (ch == '|') {
        getch();
        if (ch == '|') {
            sym = SYM_OR;
            getch();
        }
    } else if (ch == '&') {
        getch();
        sym = SYM_AND;
    } else if (ch == '#') {
		getch();
		error(29);
	} else { // other tokens
		i = NSYM;
		csym[0] = ch;
		while (csym[i--] != ch);
		if (++i)
		{
			sym = ssym[i];
			getch();
		}
		else
		{
			printf("Fatal Error: Unknown character.\n");
			exit(1);
		}
	}
} // getsym

//////////////////////////////////////////////////////////////////////
// generates (assembles) an instruction.
void gen(int x, int y, int z)
{
	if (cx > CXMAX)
	{
		printf("Fatal Error: Program too long.\n");
		exit(1);
	}
	code[cx].f = x;
	code[cx].l = y;
	code[cx++].a = z;
} // gen

//////////////////////////////////////////////////////////////////////
// tests if error occurs and skips all symbols that do not belongs to s1 or s2.
void test(symset s1, symset s2, int n)
{
	symset s;
	// printSet(s1, symtypeDescription);
	if (!checkIfInSet(sym, s1))
	{
		error(n);
		s = appendSet(s1, s2);
		while(!checkIfInSet(sym, s))
			getsym();
        destroySet(s);
	}
} // test

// enter object(constant, variable or procedre) into table.
void enter(int kind)
{
	mask* mk;

	tx++;
	strcpy(table[tx].name, id);
	table[tx].kind = kind;
	switch (kind)
	{
	case ID_CONSTANT:
		if (num > MAXADDRESS)
		{
			error(25); // The number is too great.
			num = 0;
		}
		table[tx].value = num;
		break;
	case ID_VARIABLE:
		mk = (mask*) &table[tx];
		mk->level = level;
		mk->address = dx++;
		break;
	case ID_PROCEDURE:
		mk = (mask*) &table[tx];
		mk->level = level;
		break;
	} // switch
} // enter

//////////////////////////////////////////////////////////////////////
// locates identifier in symbol table.
int position(char* id)
{
	int i;
	strcpy(table[0].name, id);
	i = tx + 1;
	while (strcmp(table[--i].name, id) != 0);
	return i;
} // position

//////////////////////////////////////////////////////////////////////
void constdeclaration()
{
	if (sym == SYM_IDENTIFIER)
	{
		getsym();
		if (sym == SYM_EQU || sym == SYM_BECOMES)
		{
			if (sym == SYM_BECOMES)
				error(1); // Found ':=' when expecting '='.
			getsym();
			if (sym == SYM_NUMBER)
			{
				enter(ID_CONSTANT);
				getsym();
			}
			else
			{
				error(2); // There must be a number to follow '='.
			}
		}
		else
		{
			error(3); // There must be an '=' to follow the identifier.
		}
	} else	error(4);
	 // There must be an identifier to follow 'const', 'var', or 'procedure'.
} // constdeclaration

//////////////////////////////////////////////////////////////////////
void vardeclaration()
{
	if (sym == SYM_IDENTIFIER)
	{
		enter(ID_VARIABLE);
		getsym();
	}
	else
	{
		error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
	}
} // vardeclaration

//////////////////////////////////////////////////////////////////////
void listcode(int from, int to)
{
	int i;
	
	printf("\n");
	for (i = from; i < to; i++)
	{
		printf("%5d %s\t%d\t%d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
	}
	printf("\n");
} // listcode

//////////////////////////////////////////////////////////////////////
void factor(symset fsys)
{
	void expression(symset fsys);
	int i;
	symset set;
	
	test(facbegsys, fsys, 24); // The symbol can not be as the beginning of an expression.

	while (checkIfInSet(sym, facbegsys))
	{
		if (sym == SYM_IDENTIFIER)
		{
			if ((i = position(id)) == 0)
			{
				error(11); // Undeclared identifier.
			}
			else
			{
				switch (table[i].kind)
				{
					mask* mk;
				case ID_CONSTANT:
					gen(LIT, 0, table[i].value);
					gen(LIT, 0, table[i].value);
					break;
				case ID_VARIABLE:
					mk = (mask*) &table[i];
					gen(LOD, level - mk->level, mk->address);
					break;
				case ID_PROCEDURE:
					error(21); // Procedure identifier can not be in an expression.
					break;
				} // switch
			}
			getsym();
		}
		else if (sym == SYM_NUMBER)
		{
			if (num > MAXADDRESS)
			{
				error(25); // The number is too great.
				num = 0;
			}
			gen(LIT, 0, num);
			getsym();
		}
		else if (sym == SYM_LPAREN)
		{
			getsym();
			set = appendSet(createSet(SYM_RPAREN, SYM_NULL), fsys);
			expression(set);
            destroySet(set);
			if (sym == SYM_RPAREN)
			{
				getsym();
			}
			else
			{
				error(22); // Missing ')'.
			}
		}
		test(fsys, createSet(SYM_LPAREN, SYM_NULL), 23);
	} // while
} // factor

//////////////////////////////////////////////////////////////////////
void term(symset fsys)
{
	int mulop;
	symset set;
	
	set = appendSet(fsys, createSet(SYM_TIMES, SYM_SLASH, SYM_NULL));
	factor(set);
	while (sym == SYM_TIMES || sym == SYM_SLASH)
	{
		mulop = sym;
		getsym();
		factor(set);
		if (mulop == SYM_TIMES)
		{
			gen(OPR, 0, OPR_MUL);
		}
		else
		{
			gen(OPR, 0, OPR_DIV);
		}
	} // while
    destroySet(set);
} // term

//////////////////////////////////////////////////////////////////////
void expression(symset fsys)
{
	int addop;
	symset set;

	set = appendSet(fsys, createSet(SYM_PLUS, SYM_MINUS, SYM_NULL));
	if (sym == SYM_PLUS || sym == SYM_MINUS)
	{
		addop = sym;
		getsym();
		term(set);
		if (addop == SYM_MINUS)
		{
			gen(OPR, 0, OPR_NEG);
		}
	}
	else
	{
		term(set);
	}

	while (sym == SYM_PLUS || sym == SYM_MINUS)
	{
		addop = sym;
		getsym();
		term(set);
		if (addop == SYM_PLUS)
		{
			gen(OPR, 0, OPR_ADD);
		}
		else
		{
			gen(OPR, 0, OPR_MIN);
		}
	} // while
    destroySet(set);
} // expression

//////////////////////////////////////////////////////////////////////
void condition(symset fsys)
{
	int relop;
	symset set;

	if (sym == SYM_ODD)
	{
		getsym();
		expression(fsys);
		gen(OPR, 0, 6);
	}
	else
	{
		set = appendSet(relset, fsys);
		expression(set);
        destroySet(set);
		if (!checkIfInSet(sym, relset))
		{
			error(20);
		}
		else
		{
			relop = sym;
			getsym();
			expression(fsys);
			switch (relop)
			{
			case SYM_EQU:
				gen(OPR, 0, OPR_EQU);
				break;
			case SYM_NEQ:
				gen(OPR, 0, OPR_NEQ);
				break;
			case SYM_LES:
				gen(OPR, 0, OPR_LES);
				break;
			case SYM_GEQ:
				gen(OPR, 0, OPR_GEQ);
				break;
			case SYM_GTR:
				gen(OPR, 0, OPR_GTR);
				break;
			case SYM_LEQ:
				gen(OPR, 0, OPR_LEQ);
				break;
			} // switch
		} // else
	} // else
} // condition

//////////////////////////////////////////////////////////////////////
void statement(symset fsys)
{
	int i, cx1, cx2, cx3;
	symset set1, set;
	if (sym == SYM_IDENTIFIER)
	{ // variable assignment
		mask* mk;
		if (! (i = position(id)))
		{
			error(11); // Undeclared identifier.
		}
		else if (table[i].kind != ID_VARIABLE)
		{
			error(12); // Illegal assignment.
			i = 0;
		}
		getsym();
        if (sym == SYM_BECOMES)
		{
			getsym();
            expression(fsys);
            mk = (mask*) &table[i];
            if (i) {
                gen(STO, level - mk->level, mk->address);
            }
            /*新增部分开始*/
		} else if (sym == SYM_MULTIPLYBY) { // *=
            getsym();
            mk = (mask*) &table[i];
            gen(LOD, level - mk-> level, mk->address);
            expression(fsys);
            if (i) {
                gen(OPR, 0, OPR_MUL);
                gen(STO, level - mk->level, mk->address);
            }
        } else if (sym == SYM_DIVIDEBY) { // /=
            getsym();
            mk = (mask*) &table[i];
            gen(LOD, level - mk-> level, mk->address);
            expression(fsys);
            if (i) {
                gen(OPR, 0, OPR_DIV);
                gen(STO, level - mk->level, mk->address);
            }
        } else if (sym == SYM_PLUSBY) { // +=
            getsym();
            mk = (mask*) &table[i];
            gen(LOD, level - mk-> level, mk->address);
            expression(fsys);
            if (i) {
                gen(OPR, 0, OPR_ADD);
                gen(STO, level - mk->level, mk->address);
            }
        } else if (sym == SYM_MINUSBY) { // -=
            getsym();
            mk = (mask*) &table[i];
            gen(LOD, level - mk-> level, mk->address);
            expression(fsys);
            if (i) {
                gen(OPR, 0, OPR_MIN);
                gen(STO, level - mk->level, mk->address);
            }
        } else {
			error(13); // ':=' expected.
		}

	}
	else if (sym == SYM_CALL)
	{ // procedure call
		getsym();
		if (sym != SYM_IDENTIFIER)
		{
			error(14); // There must be an identifier to follow the 'call'.
		}
		else
		{
			if (! (i = position(id)))
			{
				error(11); // Undeclared identifier.
			}
			else if (table[i].kind == ID_PROCEDURE)
			{
				mask* mk;
				mk = (mask*) &table[i];
				gen(CAL, level - mk->level, mk->address);
			}
			else
			{
				error(15); // A constant or variable can not be called. 
			}
			getsym();
		}
	} 
	else if (sym == SYM_IF)
	{ // if statement
		getsym();
		set1 = createSet(SYM_THEN, SYM_DO, SYM_ELSE, SYM_NULL);
		set = appendSet(set1, fsys);
		condition(set);
        destroySet(set1);
        destroySet(set);
		if (sym == SYM_THEN) {
			getsym();
		} else {
			error(16); // 'then' expected.
		}
		cx1 = cx;
		gen(JPC, 0, 0);
		statement(fsys);

        /**
         * 跳转到else
         */
        if (sym == SYM_ELSE) {
            getsym();
			// printf("保留字: SYM_ELSE - else\n");
            cx2 = cx;
            gen(JMP, 0,0); // 直接跳转，执行完then后面的则跳转到条件语句最后面
            code[cx1].a = cx; // 回填条件跳转，填回else语句块中第一句

            statement(fsys);
            code[cx2].a = cx; // 回填直接跳转地址
        } else {
            code[cx1].a = cx;
        }
	}
	else if (sym == SYM_BEGIN)
	{ // block
		getsym();
		set1 = createSet(SYM_SEMICOLON, SYM_END, SYM_NULL);
		set = appendSet(set1, fsys);
		statement(set);
		while (sym == SYM_SEMICOLON || checkIfInSet(sym, statbegsys))
		{
			if (sym == SYM_SEMICOLON)
			{
				getsym();
			}
			else
			{
				error(10);
			}
			statement(set);
		} // while
        destroySet(set1);
        destroySet(set);
		if (sym == SYM_END)
		{
			getsym();
		}
		else
		{
			error(17); // ';' or 'end' expected.
		}
	}
	else if (sym == SYM_WHILE)
	{ // while statement
		cx1 = cx;
		getsym();
		set1 = createSet(SYM_DO, SYM_NULL);
		set = appendSet(set1, fsys);
		condition(set);
        destroySet(set1);
        destroySet(set);
		cx2 = cx;
		gen(JPC, 0, 0);
		if (sym == SYM_DO)
		{
			getsym();
		}
		else
		{
			error(18); // 'do' expected.
		}
		statement(fsys);
		gen(JMP, 0, cx1);
		code[cx2].a = cx;
	}
    else if (sym == SYM_FOR) {
        getsym();
		// printf("保留字: SYM_FOR - for\n");
        if (sym != SYM_IDENTIFIER) {
            error(13);
        }
        i = position(id);

        if (i == 0) {
            error(11);
        } else if (table[i].kind != ID_VARIABLE) {
            error(12);
            i = 0; // 变量未声明
        }

        getsym();
        if (sym == SYM_BECOMES) {
            getsym();
        } else {
            error(13);
        }

        set1 = createSet(SYM_STEP, SYM_NULL); // 添加STEP到后跟符号集
        set = appendSet(set1, fsys);
        expression(set);
        destroySet(set1);
        destroySet(set);

        mask *mk = (mask *)&table[i];

        if (i != 0) {
            gen(STO, level - mk->level, mk->address); // 若变量已声明则存储
        }

        if (sym == SYM_STEP) {
            getsym();
			// printf("保留字: SYM_STEP - step\n");
        } else {
            error(27);
        }

        cx1 = cx;
        gen(JMP, 0, 0);
        cx3 = cx;

        set1 = createSet(SYM_UNTIL, SYM_NULL);
        set = appendSet(set1, fsys);
        expression(set); // 表达式求值
        destroySet(set1);
        destroySet(set);

        gen(LOD, level - mk->level, mk->address); // 取出变量的值到栈顶
        gen(OPR, 0, OPR_ADD); // 将STEP于次栈顶相加
        gen(STO, level - mk->level, mk->address); // 将新值存入变量

        if (sym == SYM_UNTIL) {
            getsym();
			// printf("保留字: SYM_UNTIL - until\n");
        } else {
            error(28);
        }

        code[cx1].a = cx; // 回调第一次for循环地址跳过STEP语句

        set1 = createSet(SYM_DO, SYM_NULL);
        set = appendSet(set1, fsys);
        expression(set);
        destroySet(set1);
        destroySet(set);

        gen(LOD, level - mk->level, mk->address);
        gen(OPR, 0, OPR_GEQ);
        cx2 = cx;
        gen(JPC, 0, 0);
        if (sym == SYM_DO) {
            getsym();
        } else {
            error(18);
        }

        statement(fsys);
        gen(JMP, 0, cx3);
        code[cx2].a = cx; // 回填条件跳转地址

    } else if (sym == SYM_STEP) {
        getsym();
        printf("保留字: SYM_STEP - step\n");
    } else if (sym == SYM_UNTIL) {
        getsym();
        printf("保留字: SYM_UNTIL - until\n");
    } else if (sym == SYM_DO) {
        getsym();
        printf("保留字: SYM_DO - do\n");
    } else if (sym == SYM_RETURN) {
        getsym();
        printf("保留字: SYM_RETURN - return\n");
    } else if (sym == SYM_PLUSBY) {
        getsym();
        printf("保留字: SYM_PLUSBY - plusBy\n");
    } else if (sym == SYM_MINUSBY) {
        getsym();
        printf("保留字: SYM_MINUSBY - minusBy\n");
    } else if (sym == SYM_MULTIPLYBY) {
        getsym();
        printf("保留字: SYM_MULTIPLYBY - multiplyBy\n");
    } else if (sym == SYM_DIVIDEBY) {
        getsym();
        printf("保留字: SYM_DIVIDEBY - divideBy\n");
    } else if (sym == SYM_AND) {
        getsym();
        printf("保留字: SYM_AND - and\n");
    } else if (sym == SYM_OR) {
        getsym();
        printf("保留字: SYM_OR - or\n");
    }
    test(fsys, phi, 19);
} // statement
			
//////////////////////////////////////////////////////////////////////
void block(symset fsys)
{
	int cx0; // initial code index
	mask* mk;
	int block_dx;
	int savedTx;
	symset set1, set;

	dx = 3;
	block_dx = dx;
	mk = (mask*) &table[tx];
	mk->address = cx;
	gen(JMP, 0, 0);
	if (level > MAXLEVEL)
	{
		error(32); // There are too many levels.
	}
	do
	{
		if (sym == SYM_CONST)
		{ // constant declarations
			getsym();
			do
			{
				constdeclaration();
				while (sym == SYM_COMMA)
				{
					getsym();
					constdeclaration();
				}
				if (sym == SYM_SEMICOLON)
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			}
			while (sym == SYM_IDENTIFIER);
		} // if

		if (sym == SYM_VAR)
		{ // variable declarations
			getsym();
			do
			{
				vardeclaration();
				while (sym == SYM_COMMA)
				{
					getsym();
					vardeclaration();
				}
				if (sym == SYM_SEMICOLON)
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			}
			while (sym == SYM_IDENTIFIER);
//			block = dx;
		} // if

		while (sym == SYM_PROCEDURE)
		{ // procedure declarations
			getsym();
			if (sym == SYM_IDENTIFIER)
			{
				enter(ID_PROCEDURE);
				getsym();
			}
			else
			{
				error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
			}


			if (sym == SYM_SEMICOLON)
			{
				getsym();
			}
			else
			{
				error(5); // Missing ',' or ';'.
			}

			level++;
			savedTx = tx;
			set1 = createSet(SYM_SEMICOLON, SYM_NULL);
			set = appendSet(set1, fsys);
			block(set);
            destroySet(set1);
            destroySet(set);
			tx = savedTx;
			level--;

			if (sym == SYM_SEMICOLON)
			{
				getsym();
				set1 = createSet(SYM_IDENTIFIER, SYM_PROCEDURE, SYM_NULL);
				set = appendSet(statbegsys, set1);
				test(set, fsys, 6);
                destroySet(set1);
                destroySet(set);
			}
			else
			{
				error(5); // Missing ',' or ';'.
			}
		} // while
		set1 = createSet(SYM_IDENTIFIER, SYM_NULL);
		set = appendSet(statbegsys, set1);
		test(set, declbegsys, 7);
        destroySet(set1);
        destroySet(set);
	}
	while (checkIfInSet(sym, declbegsys));

	code[mk->address].a = cx;
	mk->address = cx;
	cx0 = cx;
	gen(INT, 0, block_dx);
	set1 = createSet(SYM_SEMICOLON, SYM_END, SYM_NULL);
	set = appendSet(set1, fsys);
	statement(set);
    destroySet(set1);
    destroySet(set);
	gen(OPR, 0, OPR_RET); // return
	test(fsys, phi, 8); // test for error: Follow the statement is an incorrect symbol.
	listcode(cx0, cx);
} // block

//////////////////////////////////////////////////////////////////////
int base(const int stack[], int currentLevel, int levelDiff)
{
	int b = currentLevel;
	
	while (levelDiff--)
		b = stack[b];
	return b;
} // base

//////////////////////////////////////////////////////////////////////
// interprets and executes codes.
void interpret()
{
	int pc;        // program counter
	int stack[STACKSIZE];
	int top;       // top of stack
	int b;         // program, base, and top-stack register
	instruction i; // instruction register

	printf("Begin executing PL/0 program.\n");

	pc = 0;
	b = 1;
	top = 3;
	stack[1] = stack[2] = stack[3] = 0;
	do
	{
		i = code[pc++];
		switch (i.f)
		{
		case LIT:
			stack[++top] = i.a;
			break;
		case OPR:
			switch (i.a) // operator
			{
			case OPR_RET:
				top = b - 1;
				pc = stack[top + 3];
				b = stack[top + 2];
				break;
			case OPR_NEG:
				stack[top] = -stack[top];
				break;
			case OPR_ADD:
				top--;
				stack[top] += stack[top + 1];
				break;
			case OPR_MIN:
				top--;
				stack[top] -= stack[top + 1];
				break;
			case OPR_MUL:
				top--;
				stack[top] *= stack[top + 1];
				break;
			case OPR_DIV:
				top--;
				if (stack[top + 1] == 0)
				{
					fprintf(stderr, "Runtime Error: Divided by zero.\n");
					fprintf(stderr, "Program terminated.\n");
					continue;
				}
				stack[top] /= stack[top + 1];
				break;
			case OPR_ODD:
				stack[top] %= 2;
				break;
			case OPR_EQU:
				top--;
				stack[top] = stack[top] == stack[top + 1];
				break;
			case OPR_NEQ:
				top--;
				stack[top] = stack[top] != stack[top + 1];
                break;
			case OPR_LES:
				top--;
				stack[top] = stack[top] < stack[top + 1];
				break;
			case OPR_GEQ:
				top--;
				stack[top] = stack[top] >= stack[top + 1];
                break;
			case OPR_GTR:
				top--;
				stack[top] = stack[top] > stack[top + 1];
				break;
			case OPR_LEQ:
				top--;
				stack[top] = stack[top] <= stack[top + 1];
                break;
			} // switch
			break;
		case LOD:
			stack[++top] = stack[base(stack, b, i.l) + i.a];
			break;
		case STO:
			stack[base(stack, b, i.l) + i.a] = stack[top];
			printf("PC: %d, target: %d, top: %d, STO: %d\n", pc, base(stack, b, i.l) + i.a, top, stack[top]);
			top--;
			break;
		case CAL:
			stack[top + 1] = base(stack, b, i.l);
			// generate new block mark
			stack[top + 2] = b;
			stack[top + 3] = pc;
			b = top + 1;
			pc = i.a;
			break;
		case INT:
			top += i.a;
			break;
		case JMP:
			pc = i.a;
			break;
		case JPC:
			if (stack[top] == 0)
				pc = i.a;
			top--;
			break;
		} // switch
	}
	while (pc);

	printf("End executing PL/0 program.\n");
} // interpret

//////////////////////////////////////////////////////////////////////
int main ()
{
	FILE* hbin;
	char s[80] = "test/bad.pl0";
	int i;
	symset set, set1, set2;
    time_t t;
    struct tm *tmp_ptr = NULL;
    time(&t);
    tmp_ptr = localtime(&t);

    printf("软件工程4班3119005159肖扬\n");
    printf("开始调试时间: %d.%d.%d %d:%d:%d\n",
            (1900+tmp_ptr->tm_year),
            (1+tmp_ptr->tm_mon),
            tmp_ptr->tm_mday,
            tmp_ptr->tm_hour,
            tmp_ptr->tm_min,
            tmp_ptr->tm_sec);
	// printf("Please input source file name: "); // get file name to be compiled
	// scanf("%s", s);
	if ((infile = fopen(s, "r")) == NULL)
	{
		printf("File %s can't be opened.\n", s);
		exit(1);
	}

	phi = createSet(SYM_NULL);
	relset = createSet(SYM_EQU, SYM_NEQ, SYM_LES, SYM_LEQ, SYM_GTR, SYM_GEQ, SYM_NULL);
	
	// create begin symbol sets
	declbegsys = createSet(SYM_CONST, SYM_VAR, SYM_PROCEDURE, SYM_NULL);
	statbegsys = createSet(SYM_BEGIN, SYM_CALL, SYM_IF, SYM_ELSE, SYM_WHILE, SYM_FOR, SYM_STEP, SYM_UNTIL, SYM_DO, SYM_RETURN, SYM_NULL);
	facbegsys = createSet(SYM_IDENTIFIER, SYM_NUMBER, SYM_LPAREN, SYM_NULL);

	err = cc = cx = ll = 0; // initialize global variables
	ch = ' ';
	kk = MAXIDLEN;

	getsym();

	set1 = createSet(SYM_PERIOD, SYM_NULL);
	set2 = appendSet(declbegsys, statbegsys);
	set = appendSet(set1, set2);
	block(set);
    destroySet(set1);
    destroySet(set2);
    destroySet(set);
    destroySet(phi);
    destroySet(relset);
    destroySet(declbegsys);
    destroySet(statbegsys);
    destroySet(facbegsys);

	if (sym != SYM_PERIOD)
		error(9); // '.' expected.
	if (err == 0)
	{
		hbin = fopen("hbin.txt", "w");
		for (i = 0; i < cx; i++)
			fwrite(&code[i], sizeof(instruction), 1, hbin);
		fclose(hbin);
	}
	if (err == 0)
		interpret();
	else
		printf("There are %d error(s) in PL/0 program.\n", err);
    time(&t);
    tmp_ptr = localtime(&t);
    printf("\n结束调试时间: %d.%d.%d %d:%d:%d\n",
           (1900+tmp_ptr->tm_year),
           (1+tmp_ptr->tm_mon),
           tmp_ptr->tm_mday,
           tmp_ptr->tm_hour,
           tmp_ptr->tm_min,
           tmp_ptr->tm_sec);
//	listcode(0, cx);
} // main

//////////////////////////////////////////////////////////////////////
// eof pl0.c
