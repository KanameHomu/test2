#include <bits/stdc++.h>
using namespace std;
//N������������ɵĸ�����
//Ҫ��ó����������󳤶Ȳ��ɳ���N,�������ŵ�������಻����M
const int N = 1e5;
const int M = 1e3;

//��һ��ջ�������Դ�Ϊ��λ��ŵ�ջ��������һ���ṹ������e[M]��ջ��ָ��Ϊetop����ʼ��Ϊ-1��ʾջ�ա�
struct zz {
	char k[N];
} e[M];
int etop = -1;

//�ڶ���ջ�����ڴ���������+-*/��������һ��char���飬ջ��ָ��Ϊatop����ʼ��-��ʾջ��
char atom[N];
int atop = -1;

//����merges�������������һ��')'ʱ���ӵڶ���ջȡջ�������������һ��ջ��ջ�����ַ����ʹ���ջ�����ַ����ø���������ӣ�ͬʱ�����š�
//����һ��ջ�в�������Ԫ�ش���������Ĵ��󣬴�ӡerror input!���˳�����
void merges(char c) {
	//����se_top����second top����ʾ����ջ�����ַ�����
	int se_top = etop - 1;
	if (se_top < 0) {
		printf("error input!\n");
		exit(1);
	}
	//��ʱ�ַ������顣
	char tmp[N];
	tmp[0] = '(';
	tmp[1] = '\0';

	//��ʱ�ַ������飬������tmp��д��+-*/
	char _c[2];
	_c[0] = c;
	_c[1] = '\0';

	strcat(tmp, e[se_top].k);
	strcat(tmp, _c);
	strcat(tmp, e[etop].k);

	//д��')'
	_c[0] = ')';
	strcat(tmp, _c);

	//tmp���ַ�������ջ1
	strcpy(e[se_top].k, tmp);
	//ջ����С
	etop--;
}

//��д�������ת��Ϊ�ַ��ʹ����ջ1�У�����k�ĸ�λ����ڵ͵�ַ
void insert_int(int k) {
	//��ʱ������䳤��
	char tmp[N];
	int i = -1;
	//д��k����λ���ڵ͵�ַ
	while (k) {
		tmp[++i] = k % 10 + '0';
		k /= 10;
	}
	//����\0
	tmp[i + 1] = '\0';
	//��ת����
	for (int j = 0; j <= i - j; j++) {
		swap(tmp[j], tmp[i - j]);//���ǽ���������һ��c++����
	}
	//д��ջ1
	strcpy(e[++etop].k, tmp);
}

int main() {
	//����һ����#Ϊ��β��ʡ������������׺���ʽ
	int c = getchar();
	while (c != '#') {
		int k = 0;
		//isdigitΪc�����Դ��ĺ��������ַ�c������ʱ����1�����򷵻�0
		if (isdigit(c)) {
			while (isdigit(c)) {
				k = c - '0' + 10 * k;
				c = getchar();
			}
			//��k����ջ1
			insert_int(k);
			if (c == '#')
				break;
		}
		//��+-*/�����ѹ��ջ2
		if (c == '+' || c == '-' || c == '*' || c == '/') {
			atom[++atop] = c;
		}
		//����')'ʱ��ջ���ʹζ��ִ��ϲ����м��������������������
		if (')' == c) {
			merges(atom[atop]);
			atop--;
		}
		c = getchar();
	}
	//��ӡ
	for (int i = 0; i <= etop; i++) {
		printf("%s", e[i].k);
	}
	printf("\n");
	return 0;
}