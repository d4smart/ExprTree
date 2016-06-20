/*************************************************************************
  > File Name:    ExprTree.c
  > Author:       d4smart
  > Mail:         d4smart@foxmail.com
  > Created Time: 2016年06月19日 星期日 21时45分43秒
  > Version:      0.0.1
  > LastChange:   括号解决方案，浮动优先级，括号匹配（6.20）
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node
{
    char ele;
    struct _node *left;
    struct _node *right;
} Node;

Node *root;             //表达式树根节点
int isroot = 1;         //记录二叉树根节点
int base = 0;           //用于对优先级进行浮动变换，每进入一层括号加一

int IsOperator(char c);             //判断字符是否是运算符
char *MoveToEnd(char *str);         //将文件指针移动到字符串末尾
int GetPriority(char c, int flag);  //获取运算符的优先级
char *FriLstOpr(char *str);         //从尾部开始向前找到第一个优先级最低的运算符
int Check(char *expr);              //检查输入的表达式是否合法
Node *MakeTree(Node *node, char *str);  //根据表达式生成表达式树（二叉树）
double CalExpr(Node *node);            //根据表达式树计算表达式的值
int IsNumber(char *str);            //判断字符串是否为纯数字
void RemoveBrackets(char *str);     //去除表达式中多余的括号（包含整个式子的）

int main()
{
    char expr[200];
    //Node *head;
    //char *cur, *opr;

    printf("Enter the expression to caculate, press ctrl+c to quit: ");

    while(scanf("%s", expr) != EOF)
    {
        if(!Check(expr))
        {
            printf("Expression invalid!\n");
            printf("Enter the expression to caculate, press ctrl+c to quit: ");
            continue;
        }

        root = (Node *)malloc(sizeof(Node));
        root = MakeTree(root, expr);

        int result = CalExpr(root);
        printf("Result is %d.\n", result);
        
        printf("Enter the expression to caculate, press ctrl+c to quit: ");
    }

    printf("\nQuiting... Bye!\n");
}

int Check(char *expr)
{
    int pos = 0;
    char *c;
    c = expr;

    while(*c)
    {
        if (*c == '(')
            pos++;
        if (*c == ')')
            pos--;
        if (pos < 0)
            return 0;
        c++;
    }

    if (pos != 0)
        return 0;
    else 
        return 1;
}

void RemoveBrackets(char *str)
{
    char *pos;
    pos = str;

    if (*pos != '(')
        return;

    int inner = 0, BottomTime = 0;
    while(*pos)
        {
            if (*pos == '(')
                inner++;
            if (*pos == ')')
                inner--;

            if (inner == 0)
                BottomTime++;
            pos++;
        }

    if (BottomTime != 1)
        return;

    pos = str;
    while(*(pos+2))
    {
        *pos = *(pos+1);
        pos++;
    }
    *(pos) = '\0';
    *(pos+1) = '\0';
}

Node *MakeTree(Node *node, char *str)
{
    char lstr[100], rstr[100];
    //node = (Node *)malloc(sizeof(Node));
    if(IsNumber(str))
    {
        int num = atoi(str);
        node->ele = num;
        //node = ptr;

        return node;
    }

    char *cur = MoveToEnd(str);
    char *opr = FriLstOpr(cur);
    node->ele = *opr;

    //CutExpr(lstr, str, opr);
    strncpy(lstr, str, opr-str);
    node->left = (Node *)malloc(sizeof(Node));
    MakeTree(node->left, lstr);

    //CutExpr(rstr, opr+1, cur+1);
    strncpy(rstr, opr+1, cur-opr);
    node->right = (Node *)malloc(sizeof(Node));
    MakeTree(node->right, rstr);

    //node = ptr;
    return node;
}

int IsNumber(char *str)
{
    while(*str)
    {
        if(IsOperator(*str))
            return 0;
        str++;
    }

    return 1;
}

//判断字符是否是运算符
//输入：运算字符
//输出：Bool值，1是0非
int IsOperator(char c)
{
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')')
            return 1;
        else
            return 0;

}

//将文件指针移动到字符串末尾
//输入：字符串首地址
//返回：字符串末尾地址
char *MoveToEnd(char *str)
{
    while(*str)
    {
        str++;
    }
    str--;

    return str;
}

//从尾部开始向前找到第一个优先级最低的运算符
//输入：运算符尾部地址
//输出：运算符地址
char *FriLstOpr(char *str)
{
    char *opr;
    int Pri = 65536, tPri;

    while(*str)
    {
        if(IsOperator(*str))
        {
            tPri = GetPriority(*str, 1);

            if(tPri < Pri)
            {
                opr = str;
                Pri = tPri;
            }

            str--;
        }
        else
        {
            str--;
        }
    }

    return opr;
}

double CalExpr(Node *node)
{
    double result;

    if (!IsOperator(node->ele))
        return node->ele;

    double LeftResult = CalExpr(node->left);
    double RightResult = CalExpr(node->right);

    if (node->ele == '+')
    {
        result = LeftResult + RightResult;
    }
    else if (node->ele == '-')
    {
        result = LeftResult - RightResult;
    }
    else if (node->ele == '*')
    {
        result = LeftResult * RightResult;
    }
    else if (node->ele == '/')
    {
        result = LeftResult / RightResult;
    }

    return result;
}

//获取运算符的优先级
//输入：运算符，flag标记
//输出：优先级，数值越大优先级越高
int GetPriority(char c, int flag)
{
    if (c == '+' || c == '-')
        return base * 10 + 1;

    else if (c == '*' || c == '/')
        return base * 10 + 2;

    else
    {
        fprintf(stderr, "Input char is invalid!\n");
        return -1;
    }
}

