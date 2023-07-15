#include <bits/stdc++.h>
using namespace std;
//N是数组最大容纳的个数。
//要求该程序的输入最大长度不可超过N,且右括号的数量最多不超过M
const int N = 1e5;
const int M = 1e3;

//第一个栈，这是以串为单位存放的栈。容器是一个结构体数组e[M]，栈顶指针为etop，初始化为-1表示栈空。
struct zz {
	char k[N];
} e[M];
int etop = -1;

//第二个栈，用于存放运算符号+-*/。容器是一个char数组，栈顶指针为atop，初始化-表示栈空
char atom[N];
int atop = -1;

//函数merges。当主程序读入一个')'时，从第二个栈取栈顶运算符，将第一个栈的栈顶的字符串和次于栈顶的字符串用该运算符连接，同时加括号。
//当第一个栈中不足两个元素代表函数输入的错误，打印error input!并退出程序。
void merges(char c) {
	//参数se_top，即second top，表示次于栈顶的字符串。
	int se_top = etop - 1;
	if (se_top < 0) {
		printf("error input!\n");
		exit(1);
	}
	//临时字符串数组。
	char tmp[N];
	tmp[0] = '(';
	tmp[1] = '\0';

	//临时字符串数组，用于向tmp中写入+-*/
	char _c[2];
	_c[0] = c;
	_c[1] = '\0';

	strcat(tmp, e[se_top].k);
	strcat(tmp, _c);
	strcat(tmp, e[etop].k);

	//写入')'
	_c[0] = ')';
	strcat(tmp, _c);

	//tmp中字符串存入栈1
	strcpy(e[se_top].k, tmp);
	//栈顶减小
	etop--;
}

//将写入的整数转换为字符型存放在栈1中，其中k的高位存放在低地址
void insert_int(int k) {
	//临时数组和其长度
	char tmp[N];
	int i = -1;
	//写入k，低位放在低地址
	while (k) {
		tmp[++i] = k % 10 + '0';
		k /= 10;
	}
	//补上\0
	tmp[i + 1] = '\0';
	//倒转数组
	for (int j = 0; j <= i - j; j++) {
		swap(tmp[j], tmp[i - j]);//这是交换两数的一个c++函数
	}
	//写入栈1
	strcpy(e[++etop].k, tmp);
}

int main() {
	//输入一个以#为结尾的省略左括弧的中缀表达式
	int c = getchar();
	while (c != '#') {
		int k = 0;
		//isdigit为c语言自带的函数，当字符c是数字时返回1，否则返回0
		if (isdigit(c)) {
			while (isdigit(c)) {
				k = c - '0' + 10 * k;
				c = getchar();
			}
			//将k插入栈1
			insert_int(k);
			if (c == '#')
				break;
		}
		//将+-*/运算符压入栈2
		if (c == '+' || c == '-' || c == '*' || c == '/') {
			atom[++atop] = c;
		}
		//读入')'时将栈顶和次顶字串合并，中间加运算符，且两侧加括号
		if (')' == c) {
			merges(atom[atop]);
			atop--;
		}
		c = getchar();
	}
	//打印
	for (int i = 0; i <= etop; i++) {
		printf("%s", e[i].k);
	}
	printf("\n");
	return 0;
}