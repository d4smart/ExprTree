/*************************************************************************
  > File Name:    ExprTree.c
  > Author:       d4smart
  > Mail:         d4smart@foxmail.com
  > Created Time: 2016年06月19日 星期日 21时45分43秒
  > Version:      0.0.1
  > LastChange:   括号解决方案，浮动优先级，括号匹配（6.20）；添加注释，解决Bug（6.21）
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//二叉树节点结构体
typedef struct _node
{
    char operator;          //运算符
    int digit;              //操作数
    struct _node *left;     //左子树
    struct _node *right;    //右子树
} Node;

Node *root;                         //表达式树根节点
int base;                           //用于对优先级进行浮动变换，每进入一层括号加一

int IsOperator(char c);             //判断字符是否是运算符
char *MoveToEnd(char *str);         //将文件指针移动到字符串末尾
int GetPriority(char c);            //获取运算符的优先级
char *FriLstOpr(char *str);         //从尾部开始向前找到第一个优先级最低的运算符
int Check(char *expr);              //检查输入的表达式是否合法
Node *MakeTree(Node *node, char *str);  //根据表达式生成表达式树（二叉树）
double CalExpr(Node *node);         //根据表达式树计算表达式的值
int IsNumber(char *str);            //判断字符串是否为纯数字
void RemoveBrackets(char *str);     //去除表达式中多余的括号（包含整个式子的）

int main()
{
    char expr[200]; //用于存储算数表达式

    printf("\nEnter the expression to caculate, press ctrl+z(win)/ctrl+d(linux) to quit: ");

    while(scanf("%s", expr) != EOF) //处理主循环
    {
        base = 0;

        //表达式检查没有通过
        if(!Check(expr))
        {
            printf("Expression invalid!\n");
            printf("\nEnter the expression to caculate, press ctrl+z(win)/ctrl+d(linux) to quit: ");
            continue;
        }

        root = (Node *)malloc(sizeof(Node));    //为根节点分配内存
        RemoveBrackets(expr);           //去除表达式中多余的括号
        root = MakeTree(root, expr);    //根据表达式生成表达式树

        double result = CalExpr(root);  //计算结果
        printf("Result is %.2f.\n\n", result);
        
        printf("\nEnter the expression to caculate, press ctrl+z(win)/ctrl+d(linux) to quit: ");
    }

    printf("\n\nQuiting... Bye!\n\n");
}

//检查输入的表达式是否合法
//输入：表达式字符串首地址
//输出：判断结果，1是0否
//原理：表达式中左右括号必须对应，且顺序遍历过程中左括号个数一定>=右括号个数
//      最后左括号个数等于右括号个数
int Check(char *expr)
{
    int pos = 0;
    char *c;
    c = expr;

    while(*c)
    {
        //判断括号并记数
        if (*c == '(')
            pos++;
        if (*c == ')')
            pos--;

        //小于0则右括号个数大于左括号，表达式不合法
        if (pos < 0)
            return 0;

        c++;    //游标自增
    }

    //最后左括号个数等于右括号个数则验证通过
    if (pos != 0)
        return 0;
    else 
        return 1;
}

//去除表达式中多余的括号（对应范围为整个式子）
//输入：带多余括号的表达式
//输出：去除表达式中多余的括号
//例如：（（1+2）*3）->（1+2）*3， （1+2）*（3+4）->（1+2）*（3+4）
//原理：触底指括号个数对应相等（抵消），带多余括号的表达式
//      在顺序遍历时一定会触底且只触底一次
void RemoveBrackets(char *str)
{
    char *pos;
    pos = str;  //声明游标并将游标移到表达式开头

    //带多余括号的表达式第一个字符一定是"("，据此判断
    if (*pos != '(')
        return;

    int inner = 0, BottomTime = 0;  //抵消计数，触底计数
    while(*pos)
        {
            //对括号计数
            if (*pos == '(')
                inner++;
            if (*pos == ')')
                inner--;

            //若触底则触底计数加1
            if (inner == 0)
                BottomTime++;

            pos++;  //游标自增
        }

    if (BottomTime != 1)
        return;

    //去除括号的处理
    pos = str;
    while(*(pos+2))
    {
        *pos = *(pos+1);
        pos++;
    }
    *(pos) = '\0';
    *(pos+1) = '\0';
}

//根据表达式生成表达式树
//输入：表达式字符串
//输出：构造出的表达式树根节点
Node *MakeTree(Node *node, char *str)
{
    char lstr[100] = "", rstr[100] = "";    //运算符左右的字符串，初始全部赋为空值（若不全部为空则会保留上次的不可知结果，出现Bug 6.21）

    //若表达式为数字，则创建节点存放数字并返回节点
    if(IsNumber(str))
    {
        int num = atoi(str);
        node->digit = num;
        //node = ptr;

        return node;
    }

    char *cur = MoveToEnd(str); //移动到字符串末尾
    char *opr = FriLstOpr(cur); //找出优先级最低的运算字符
    node->operator = *opr;           //存储运算符

    strncpy(lstr, str, opr-str);    //切割左边的字符串
    RemoveBrackets(lstr);           //去除多余括号
    node->left = (Node *)malloc(sizeof(Node));
    MakeTree(node->left, lstr);     //构造左子树

    strncpy(rstr, opr+1, cur-opr);  //切割右边的字符串
    RemoveBrackets(rstr);           //去除多余括号
    node->right = (Node *)malloc(sizeof(Node));
    MakeTree(node->right, rstr);    //构造右子树

    //node = ptr;
    return node;    //返回树的根节点
}

//判断字符串是否为纯数字
//输入：待判断字符串
//输出：判断结果
int IsNumber(char *str)
{
    while(*str)
    {
        //出现运算符则不是纯数字
        if(IsOperator(*str))
            return 0;

        str++;  //游标自增
    }

    return 1;   //是纯数字
}

//判断字符是否是运算符
//输入：运算字符
//输出：判断结果
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
    char *opr;              //运算符
    int Pri = 65536, tPri;  //最低优先级，当前优先级

    while(*str)
    {
        //出现括号改变base值
        if (*str == ')')
            base++;
        if (*str == '(')
            base--;

        //出现运算符则与当前优先级最低运算符比较
        if (IsOperator(*str))
        {
            tPri = GetPriority(*str);

            if(tPri < Pri)
            {
                opr = str;
                Pri = tPri;
            }
        }
        
        str--;
    }

    return opr; //返回运算符地址
}

//根据表达式树计算表达式的值
//输入：表达式树根节点
//输出：运算结果
double CalExpr(Node *node)
{
    double result;

    //节点为数字直接返回数字
    if (node->digit)
        return node->digit;

    double LeftResult = CalExpr(node->left);    //计算左子树结果
    double RightResult = CalExpr(node->right);  //计算右子树结果

    //释放树节点内存
    free(node->left);
    free(node->right);

    //运算
    if (node->operator == '+')
    {
        result = LeftResult + RightResult;
    }
    else if (node->operator == '-')
    {
        result = LeftResult - RightResult;
    }
    else if (node->operator == '*')
    {
        result = LeftResult * RightResult;
    }
    else if (node->operator == '/')
    {
        result = LeftResult / RightResult;
    }

    return result;  //返回运算结果
}

//获取运算符的优先级
//输入：运算符，flag标记
//输出：优先级，数值越大优先级越高
int GetPriority(char c)
{
    //运算符合法，则返回浮动优先级
    if (c == '+' || c == '-')
        return base * 10 + 1;
    else if (c == '*' || c == '/')
        return base * 10 + 2;
    else if (c == '(' || c == ')')
        return base * 10 + 3;

    else    //不合法退出
    {
        fprintf(stderr, "Input char is invalid!\n");
        return -1;
    }
}

