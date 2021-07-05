#include <iostream>
#include<string>
#include<fstream>
#include <cassert>
#include<vector>
#include <sstream>  
#include <iostream>
#include "hecong.h"

using namespace std;

struct Quotation_marks {
	string q_type;
	string op;
	string res1;
	string res2;
	string des;
};


struct op
{
	TYPEL* type_p;
	string CAT;
	int   varpos;
	int   constvalue;
};

int temval_num = 0;

TYPEL type_int;
TYPEL type_float;
TYPEL type_char;



vector<string> temval_stack;
vector<TYPEL*> type_stack;
vector<Token>  waitingToken_stack;
vector<op>       op_stack;
vector<Quotation_marks> Quo_array;

int serch_in_syn(Token tk);

void output_quo() {
	for (auto iter = Quo_array.begin(); iter != Quo_array.cend(); iter++) {
		cout << "(" << iter->op << "," << iter->res1 << "," << iter->res2 << "," << iter->des << ")";
		cout << endl;
	}
}

void push_op() {
	Token tk = waitingToken_stack.back();
	waitingToken_stack.pop_back();
	op newop;
	if (tk.type == "I") {
		newop.CAT = "VAR";
		newop.varpos = serch_in_syn(tk);
		newop.type_p = mySYNBL[newop.varpos].TYPE;
		newop.constvalue = 0;
	}
	else if (tk.type == "C1" || tk.type == "C2") {
		newop.CAT = "CONST";
		newop.varpos = -1;
		newop.type_p = NULL;
		if (tk.type == "C1")
		newop.constvalue = str_to_double(C1[tk.number - 1]);
		else
		newop.constvalue = str_to_double(C2[tk.number - 1]);
	}
	op_stack.push_back(newop);
}

void init_sym() {
	char a = 'I';
	type_int.TVAL = a;
	type_int.TPOINT1 = NULL;
	type_int.TPOINT2 = NULL;

	a = 'F';
	type_float.TVAL = a;
	type_float.TPOINT1 = NULL;
	type_float.TPOINT2 = NULL;

	a = 'C';
	type_char.TVAL = a;
	type_char.TPOINT1 = NULL;
	type_char.TPOINT2 = NULL;
};

int serch_in_syn(Token tk) {
	string name;
	if (tk.type == "I") {
		name = I[tk.number-1];
	}
	for (auto p = mySYNBL.begin(); p != mySYNBL.end(); p++) {
		if (p->NAME == name)
			return distance(mySYNBL.begin(),p);
	}
	cout << "在符号表中未查询到:"<<name;
	return -1;
};

bool is_firstshow(op a) {
	if (mySYNBL[a.varpos].CAT == "") {
		return true;
	}
	return false;
};

void Gen_FunQuo() {
	Quotation_marks now;
	now.q_type = "Fun";
	Token tk = waitingToken_stack.back();

	now.op = "FunDefBegin";
	now.res1 = I[tk.number - 1];
	now.res2 = "";
	now.des = "";

	Quo_array.push_back(now);
};

void Gen_FunQuo_rtn() {
	Quotation_marks now;
	now.q_type = "Fun";
	Token tk = waitingToken_stack.back();

	now.op = "FunReturn";
	if (tk.type == "C1" || tk.type == "C2") {
		op newop;
		newop.type_p = NULL;
		//newop.CAY="";
		newop.constvalue = myCONSL[tk.number - 1].length;
	}
	else if (tk.type == "I") {
		op newop;
		int pos = serch_in_syn(tk);
		//错误检查  函数定义的返回值类型 和 真实返回值类型是否相符
		if (mySYNBL[pos].TYPE != type_stack.back()) {
			cout << "返回值与函数返回类型不匹配！";
			return;
		}
		newop.type_p = mySYNBL[pos].TYPE;
		newop.CAT = "VAR";
		newop.varpos = pos;
	}
}


void Gen_AssQuo() {
	Quotation_marks now;
	now.q_type = "Ass";

	op firop = op_stack.back();
	op_stack.pop_back();
	op secop = op_stack.back();
	op_stack.pop_back();

	now.op = "=";
	if (firop.CAT == "TEM") {
		now.res1 = "t"+to_string(temval_num);
	}
	else if(firop.CAT=="VAR")
	{
		now.res1 = mySYNBL[firop.varpos].NAME;
	}
	else {
		now.res1 = to_string(firop.constvalue);
	}
	now.res2 = "";

    if (secop.CAT == "VAR")
	{
		if (is_firstshow(secop)) {
			cout << "为未定义变量赋值";
			return;
		}
		now.des = mySYNBL[secop.varpos].NAME;
	}else if(secop.CAT == "CONST"){
		cout << "赋值语句左端不能是常数";
	}else {

	}
	Quo_array.push_back(now);
};


void Gen_FuQuo() {
	op firop = op_stack.back();
	op_stack.pop_back();

	Quotation_marks now;
	now.q_type = "Fu";

	if (firop.CAT == "VAR")
	{
		now.res1 = mySYNBL[firop.varpos].NAME;
	}
	now.op = "-";
	now.res2 = "";
	temval_num++;

	now.des = "t" + to_string(temval_num);
	temval_stack.push_back(now.des);

	op newop;
	newop.CAT = "TEM";
	newop.varpos = temval_num;
	op_stack.push_back(newop);
	Quo_array.push_back(now);
}

void Gen_CuQuo() {
	Quotation_marks now;
	now.q_type = "Cu";

	Token tk = waitingToken_stack.back();
	waitingToken_stack.pop_back();
	string fuhao;
	if (tk.type == "P")
		fuhao = P[tk.number - 1];
	else {
		cout << "未知错误";
	}

	op firop = op_stack.back();
	op_stack.pop_back();
	op secop = op_stack.back();
	op_stack.pop_back();

	now.op = fuhao;

	if (firop.CAT == "TEM") {
		now.res1 = "t" + to_string(temval_num);
	}
	else if (firop.CAT == "VAR")
	{
		now.res1 = mySYNBL[firop.varpos].NAME;
	}
	else {
		now.res1 = to_string(firop.constvalue);
	}

	if (secop.CAT == "TEM") {
		now.res2 = "t" + to_string(temval_num);
	}
	else if (secop.CAT == "VAR")
	{
		now.res2 = mySYNBL[secop.varpos].NAME;
	}
	else {
		now.res2 = to_string(secop.constvalue);
	}


	temval_num++;

	now.des = "t" + to_string(temval_num);
	temval_stack.push_back(now.des);

	op newop;
	newop.CAT = "TEM";
	newop.varpos = temval_num;
	op_stack.push_back(newop);
	Quo_array.push_back(now);
}

void fill_fun_sym() {
	Token tk = waitingToken_stack.back();
	int pos = serch_in_syn(tk);
	mySYNBL[pos].CAT = "FUN";
	mySYNBL[pos].TYPE = type_stack.back();
};

void fill_var_sym() {
	Token tk = waitingToken_stack.back();
	int pos = serch_in_syn(tk);
	//错误检查
	if (mySYNBL[pos].CAT != "" || mySYNBL[pos].TYPE != NULL) {
		if (mySYNBL[pos].CAT != "")
			cout << mySYNBL[pos].CAT;
		cout << "重复声明变量";
		return;
	}	
	mySYNBL[pos].CAT = "VAR";
	mySYNBL[pos].TYPE = type_stack.back();
};

class grammer
{
private:
	Token w;
	void NEXTW();
	bool program();
	bool ExtDefList();
	bool ExtDef();
	bool Specifier();
	bool FunDec();
	bool CompSt();
	bool TYPE();
	bool VarList();
	bool DefList();
	bool StmList();
	bool Def();
	bool DecList();
	bool Dec();
	bool VarDec();
	bool Stmt();
	bool Exp();
	bool exp2();
public:
	void mainprogram();
};

int main()
{
	grammer G;
	input = readTxt("C:/Users/l/source/repos/SemanticAnalysis/data.txt");
	G.mainprogram();
	output_quo();
	return 0;
}
void grammer::NEXTW()
{
	w = getToken();
}
void grammer::mainprogram()
{
	init_sym();
	NEXTW();
	program();
	if (w.type == "#")
	{
		cout << endl << "正确";
	}
	else
	{
		cout << endl << "错误";
		exit(0);
	}
}
bool grammer::program()
{
	if (ExtDefList())
	{
		return true;
	}
	return false;
}
bool grammer::ExtDefList()
{
	if (ExtDef())
	{
		if (ExtDefList())
		{
			return true;
		}
		return false;
	}
	else return true;
}
bool grammer::ExtDef()
{
	if (Specifier())
	{
		if (FunDec())
		{
			if (CompSt())
			{
				return true;
			}
		}
	}
	return false;
}
bool grammer::Specifier()
{
	if (TYPE())
	{
		return true;
	}
	return false;
}
bool grammer::TYPE()
{
	if (w.type == "K" && (w.number == 1 || w.number == 4))
	{
		if (w.type == "K" && w.number == 1)
		{
			auto p = &type_int;
			type_stack.push_back(p);
		}
		else {
			auto p = &type_float;
			type_stack.push_back(p);
		}
		NEXTW();
		return true;
	}
	return false;
}
bool grammer::FunDec()
{
	if (w.type == "I" || (w.type == "K" && w.number == 15))
	{
		if (w.type == "I") {
			waitingToken_stack.push_back(w);
			Gen_FunQuo();
			fill_fun_sym();
		}
		else {

		}
		NEXTW();
		if (w.type == "P" && w.number == 3)
		{
			NEXTW();
			if (w.type == "P" && w.number == 4)
			{
				NEXTW();
				return true;
			}
			/*
			else
			{
				VarList();
				if (w.type == "P" && w.number == 4)
				{
					NEXTW();
					return true;
				}
				return false;
			}
			*/
		}
	}
	return false;
}
bool grammer::CompSt()
{
	if (w.type == "P" && w.number == 15)
	{
		NEXTW();
		if (DefList())
		{
			if (StmList())
			{
				if (w.type == "P" && w.number == 16)
				{
					type_stack.pop_back();
					//gen
					NEXTW();
					return true;
				}
			}
		}
	}
	return false;
}
bool grammer::DefList()
{
	if (Def())
	{
		if (DefList())
		{
			return true;
		}
		return false;
	}
	return true;
}
bool grammer::Def()
{
	if (Specifier())
	{
		if (DecList())
		{
			if (w.type == "P" && w.number == 13)
			{
				type_stack.pop_back();
				NEXTW();
				return true;
			}
		}
	}
	return false;
}
bool grammer::DecList()
{
	if (Dec())
	{
		if (w.type == "P" && w.number == 12)
		{
			NEXTW();
			if (DecList())
			{
				return true;
			}
			return false;
		}
		return true;
	}
	return false;
}
bool grammer::Dec()
{
	if (VarDec())
	{
		//fill_var_sym();
		if (w.type == "P" && w.number == 11)
		{
			NEXTW();
			if (Exp())
			{
				Gen_AssQuo();
				return true;
			}
			return false;
		}
		return true;
	}
	return false;
}
bool grammer::VarDec()
{
	if (w.type == "I")
	{
		waitingToken_stack.push_back(w);
		fill_var_sym();
		push_op();
		NEXTW();
		return true;
	}
	return false;
}
bool grammer::StmList()
{
	if (Stmt())
	{
		if (StmList())
		{
			return true;
		}
		return false;
	}
	return true;
}
bool grammer::Stmt()
{
	if (w.type == "K" && w.number == 11)
	{
		NEXTW();
		if (Exp())
		{
			if (w.type == "P" && w.number == 13)
			{
				NEXTW();
				return true;
			}
		}
	}
	if (Exp())
	{
		if (w.type == "P" && w.number == 13)
		{
			NEXTW();
			return true;
		}
	}
	return false;

}
bool grammer::Exp()
{
	if (w.type == "I")
	{
		waitingToken_stack.push_back(w);
		push_op();
		NEXTW();
		if (w.type == "P" && w.number == 11)
		{
			NEXTW();
			if (Exp())
			{
				Gen_AssQuo();
				if (exp2())
				{
					return true;
				}
			}
		}
		if (exp2())
		{
			return true;
		}
		return false;
	}
	if (w.type == "P" && w.number == 3)
	{
		NEXTW();
		if (Exp())
		{
			if (w.type == "P" && w.number == 4)
			{
				NEXTW();
				if (exp2())
				{
					return true;
				}
			}
		}
		return false;
	}
	if (w.type == "P" && w.number == 1)
	{
		NEXTW();
		if (Exp())
		{
			Gen_FuQuo();
			if (exp2())
			{
				return true;
			}
		}
		return false;
	}
	if (w.type == "C1" || w.type == "C2")
	{
		waitingToken_stack.push_back(w);
		push_op();
		NEXTW();
		if (exp2())
		{
			return true;
		}
		return false;
	}
	return false;
}
bool grammer::exp2()
{
	if (w.type == "P" && (w.number == 8 || w.number == 1 || w.number == 9 || w.number == 2))
	{
		waitingToken_stack.push_back(w);
		NEXTW();
		if (Exp())
		{
			Gen_CuQuo();
			if (exp2())
			{
				return true;
			}
		}
		return false;
	}
	return true;
}