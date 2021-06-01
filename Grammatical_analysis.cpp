#include <iostream>
#include <cstring>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <fstream>
#include <sstream>
using namespace std;
//����ʽ 
struct Estring {
	string left;	//����ʽ��
	vector<string> right;	//����ʽ�Ҳ�
	Estring(string l, vector<string> r) {
		left = l;
		right = r;
	}
};
//�ķ�
struct Grammar {
	vector<Estring> expressions;		//����ʽ����
	string S;							//��ʼ�ַ�
	set<string> Vn;						//���ս����
	set<string> Vt;						//�ս����
	map<string, set<string> > firstSet;		//First��
	map<string, set<string> > followSet;    //Follow��
	vector<set<string> > selectSet; //select��
	vector<vector<int> > table;  //Ԥ�������
}G;
int ll1=1;
vector<string> tokens;
void init();
void initVt();
void inputEX(string rule);
void calFisrt();
void calFollow();
void calSelect();
int get_column(string target);
int get_row(string target);
void create_table();
void print_table();
vector<string> split(const string& str, const string& delim);
void deal_tokens();
//ת������ 
void init() {
	fstream fp("./2.txt", ios::in);
	if (!fp) {
		cout << "�ļ���ʧ��" << endl;
		exit(-1);
	}
	string rule;
	G.S = "S";
	initVt();
	while (!fp.eof()) {
		getline(fp, rule);
		inputEX(rule);
	}
}
void initVt() {
	G.Vt.insert("+");//operators
	G.Vt.insert("-");
	G.Vt.insert("*");
	G.Vt.insert("/");
	G.Vt.insert(">");
	G.Vt.insert(">=");
	G.Vt.insert("<");
	G.Vt.insert("<=");
	G.Vt.insert("=");
	G.Vt.insert("==");
	G.Vt.insert("!=");
	G.Vt.insert("$");//limiter
	G.Vt.insert(";");
	G.Vt.insert(",");
	G.Vt.insert("(");
	G.Vt.insert(")");
	G.Vt.insert("{");
	G.Vt.insert("}");
	G.Vt.insert("char");//type
	G.Vt.insert("int");
	G.Vt.insert("void");
	G.Vt.insert("float");
	G.Vt.insert("number");
	G.Vt.insert("identifier");
	G.Vt.insert("or");
	G.Vt.insert("and");
	G.Vt.insert("not");
	G.Vt.insert("if");
	G.Vt.insert("else");
	G.Vt.insert("while");
	G.Vt.insert("for");
	G.Vt.insert("return");
	G.Vt.insert("print");
}
void inputEX(string rule) {//����2���ķ� 
	int pre = rule.find('-');
	int last = rule.find('>') + 1;
	string left = rule.substr(0, pre);
	G.Vn.insert(left);
	vector<string> right;
	int pos = rule.find('>');
	for (int i = last; i < rule.length(); i++) {
		if (rule[i] == ' ') {
			pos = i;
			string cur = rule.substr(last, i - last);
			right.push_back(cur);
			if (G.Vn.find(cur) == G.Vn.end() && G.Vt.find(cur) == G.Vt.end()) {
				G.Vn.insert(cur);
			}
			last = i + 1;
		}
	}
	string cur = rule.substr(pos + 1, rule.length());
	right.push_back(cur);
	if (cur == "$") {
		right.clear();
	}
	if (G.Vn.find(cur) == G.Vn.end() && G.Vt.find(cur) == G.Vt.end()) {
		G.Vn.insert(cur);
	}

	G.expressions.push_back(Estring(left, right));
}
void calFisrt() {
	set<string> vn;
	int esize, flag = 1;
	for (int i = 0; i < G.expressions.size(); i++) {    //����ʽ������
		vn.insert(G.expressions[i].left);            //vn��������в���ʽ��˵ķ��ս��  ʹ��setʹ��Ψһ
	}
	while (flag) {
		flag = 0;
		for (set<string>::iterator it = vn.begin(); it != vn.end(); it++) {    //�������з��ս��
			esize = G.firstSet[*it].size();  //��ǰ�ս����first���ϴ�С
			for (int i = 0; i < G.expressions.size(); i++) {   //��ÿһ�����ս���������в���ʽ
				if (G.expressions[i].left == *it) {// �����Ŀ��ƥ��
					if (G.expressions[i].right.size() == 0) {  // right��$ʱ �ս��ֱ�Ӽ���
						G.firstSet[*it].insert("$");
					}
					else {// ���ս���������ַ���⣬ֱ�������ս��������ȫ���ܹ��Ƴ���
						for (int j = 0; j < G.expressions[i].right.size(); j++) {
							string str = G.expressions[i].right[j];
							if (G.Vt.find(str) != G.Vt.end()) {  //����ұߵĵ�һ�����Ƿ��ս�� ����ֱ�Ӳ���������ս�� Ȼ������ѭ��
								G.firstSet[*it].insert(str);
								break;
							}
							else if (G.Vn.find(str) != G.Vn.end()) { // �������Ƿ��ս��
								if (G.firstSet[str].find("$") == G.firstSet[str].end()) {  //�������ս����first�����Ƿ��п�$
									G.firstSet[*it].insert(G.firstSet[str].begin(), G.firstSet[str].end());//��������п� �ͰѸ÷��ս����first�����뵱ǰ���ڴ���ķ��ս����first��
									break;
								}
								else {  //������пգ��ͰѸ÷��ս������$������first����Ԫ�ؼ���*it�е�first�����У����ﲢ������ѭ��
									for (set<string>::iterator init = G.firstSet[str].begin(); init != G.firstSet[str].end(); init++) {
										if (*init != "$") {
											G.firstSet[*it].insert(*init);
										}
									}
								}

							}
						}
					}
				}
			}
			if (esize != G.firstSet[*it].size())
				flag = 1;  //�����һ�����ս����first���ϵĴ�С�����仯�� �����±��������ս��
		}
	}
}
void calFollow() {
	set<string> vn;
	int esize, flag = 1;
	G.followSet[G.S].insert("#");    //��ʼ����ʼ����follow���к���# 
	for (int i = 0; i < G.expressions.size(); i++) {    //����ʽ������
		vn.insert(G.expressions[i].left);            //vn��������в���ʽ��˵ķ��ս��  ʹ��setʹ��Ψһ
	}
	while (flag) {
		flag = 0;
		for (int i = 0; i < G.expressions.size(); i++) {
			string left = G.expressions[i].left;
			for (int j = 0; j < G.expressions[i].right.size(); j++) {
				string right = G.expressions[i].right[j]; //����ʽ�ұߵ�j��Ԫ��
				if (vn.find(right) != vn.end()) {       //�������ʽ�ұ����Ԫ���Ƿ��ս��
					string bright;//׼���洢right�����Ԫ��
					esize = G.followSet[right].size();
					if (j < G.expressions[i].right.size() - 1) {	// Follow(rj)+=First(brj)  rightԪ�غ��滹��Ԫ��
						bright = G.expressions[i].right[j + 1];  //�洢right�����Ԫ��
						if (G.Vt.find(bright) != G.Vt.end()) { //������Ǹ��ս�� ��ֱ�Ӽ��뵽follow����
							G.followSet[right].insert(bright);
						}
						else {    //else ���Ǹ����ս��
							int emptyFlag = 0;  //����brightԪ�ص�first�����Ƿ�Ϊ��
							set<string>::iterator bright_iter;
							for (bright_iter = G.firstSet[bright].begin(); bright_iter != G.firstSet[bright].end(); bright_iter++) {
								if (*bright_iter != "$") //��brightԪ�ص�first�����еķǿ�Ԫ�ؼ���right��follow������
									G.followSet[right].insert(*bright_iter);
								else
									emptyFlag = 1;//��ʾbrightԪ�ص�first�����к��п�Ԫ��$
							}
							if (emptyFlag == 1) {		// A->aBCD��C�����Ƴ��ţ�Follow(B)+=Follow(C){
								for (bright_iter = G.followSet[bright].begin(); bright_iter != G.followSet[bright].end(); bright_iter++) {
									G.followSet[right].insert(*bright_iter);
								}
							}
						}
					}
					else {    //    rightԪ�غ���û��Ԫ��  Follow(rj)+=Follow(left){
						set<string>::iterator left_iter;
						for (left_iter = G.followSet[left].begin(); left_iter != G.followSet[left].end(); left_iter++) {
							G.followSet[right].insert(*left_iter);
						}
					}
					if (esize != G.followSet[right].size())
						flag = 1;  //�����һ�����ս����first���ϵĴ�С�����仯�� �����±��������ս��
				}
			}
		}
	}
}
void calSelect() {
	for (int i = 0; i < G.expressions.size(); i++) { //�������в���ʽ
		int countEmpty = 0;			// �Ҷ�ȫ�����Ƴ�$����ʹ�� Select(A-a) = First(a)-$ �� Follow(A)
		set<string> select;
		for (int j = 0; j < G.expressions[i].right.size(); j++) { //������������ʽ�ұߵ�����Ԫ��
			string right = G.expressions[i].right[j];
			if (G.Vt.find(right) != G.Vt.end()) { // �����ս��
				if (right != "$")	// ���ǿմ�����ֱ�Ӽ��벢����
					select.insert(right);
				else if (right == "$")
					countEmpty++;
				break;   //�����ս�� ��������ս���ǲ��ǿգ� ��������ʽ��select���Ͼ��Ѿ�ȷ��
			}
			else {// �������ս��
				set<string>::iterator value_iter;   // ����First�����з�$������
				for (value_iter = G.firstSet[right].begin(); value_iter != G.firstSet[right].end(); value_iter++) {
					if (*value_iter != "$")
						select.insert(*value_iter);
				}
				if (G.firstSet[right].find("$") != G.firstSet[right].end()) {
					countEmpty++;
				}
				else {
					break;
				}
			}
		}
		// ��Follow���ϲ���
		if (countEmpty == G.expressions[i].right.size()) {
			set<string>::iterator value_iter;
			for (value_iter = G.followSet[G.expressions[i].left].begin(); value_iter != G.followSet[G.expressions[i].left].end(); value_iter++) {
				select.insert(*value_iter);
			}
		}
		G.selectSet.push_back(select);
	}
}
int get_column(string target) { // ���ս����Ԥ��������е��б�
	int i;
	set<string>::iterator t_iter;
	for (t_iter = G.Vt.begin(), i = 0; t_iter != G.Vt.end(); t_iter++, i++) {
		if (*t_iter == target)
			return i;
	}
	return 0;
}

int get_row(string target) {// ����ս����Ԥ��������е��б�
	int i;
	set<string>::iterator nt_iter;
	for (nt_iter = G.Vn.begin(), i = 0; nt_iter != G.Vn.end(); nt_iter++, i++) {
		if (*nt_iter == target)
			return i;
	}
	return 0;
}
void create_table() {
	vector<int> x;
//	cout << G.Vn.size() << " " << G.Vt.size() << endl;
	for (int i = 0; i < G.Vn.size(); i++) {
		G.table.push_back(x);
		for (int j = 0; j < G.Vt.size(); j++) {
			G.table[i].push_back(-1);
		}
	}
//	cout << G.table.size() << " " << G.table[2].size() << endl;
	for (int i = 0; i < G.expressions.size(); i++) {
		int row = get_row(G.expressions[i].left);  //��ȡ����ʽ��ߵķ��ս����ڼ��� 
		set<string>::iterator s_iter;
		for (s_iter = G.selectSet[i].begin(); s_iter != G.selectSet[i].end(); s_iter++) {
//			if(*s_iter=="i")
//				cout<<"YES"<<endl;
			int column = get_column(*s_iter); //��ȡselect���е�Ԫ���ڵڼ��� 
			if (G.table[row][column] != -1) {
				ll1=0; 
				return;
			}
			else {
				G.table[row][column] = i;
			}
		}
	}
}
//void print_table() {
//	int i, j;
//	cout << "-------------Ԥ�������---------------" << endl << endl;
//	set<string>::iterator it;
//	vector<string>::iterator it1;
//	for (it = G.Vt.begin(); it != G.Vt.end(); it++) {
//		if (*it == "$")
//			cout << "\t" << '#';
//		else
//			cout << "\t" << *it;
//	}
//	cout << endl;
//	for (it = G.Vn.begin(), i = 0; it != G.Vn.end(); it++, i++) {
//		cout << *it;
//		for (j = 0; j < G.Vt.size(); j++) {
//			if (G.table[i][j] != -1) {
//				cout << "\t";
//				cout << G.expressions[G.table[i][j]].left << "->";
//				for (it1 = G.expressions[G.table[i][j]].right.begin(); it1 != G.expressions[G.table[i][j]].right.end(); it1++) {
//					cout << *it1 << " ";
//				}
//			}
//			else {
//				cout << "\t";
//			}
//		}
//		cout << endl;
//	}
//	cout << endl;
//}
vector<string> split(const string& str, const string& delim) {
	vector<string> res;
	if ("" == str) return res;
	//�Ƚ�Ҫ�и���ַ�����string����ת��Ϊchar*����  
	char* strs = new char[str.length() + 1]; //��Ҫ����  
	strcpy(strs, str.c_str());

	char* d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char* p = strtok(strs, d);
	while (p) {
		string s = p; //�ָ�õ����ַ���ת��Ϊstring����  
		res.push_back(s); //����������  
		p = strtok(NULL, d);
	}
	delete strs;
	delete d; 
	return res;
}
void deal_tokens() {
	fstream fp("./ans.txt", ios::in);
	if (!fp) {
		cout << "�ļ���ʧ��" << endl;
		exit(-1);
	}
	string str;
	while (!fp.eof()) {         //����token 
		getline(fp, str);
		if (str.length() == 0)
			break;
		vector<string> temp = split(str, " ");
		string token;
		if (temp[1] == "number") {
			token = temp[1];
		}
		else if (temp[1] == "operator") {
			token = temp[2];
		}
		else if (temp[1] == "limiter" || temp[1] == "keyword") {
			token = temp[2];
		}
		else if (temp[1] == "identifier") {
			token = temp[1];
		}

		tokens.push_back(token);
	}
}
int main() {
	init();
	calFisrt();
	calFollow();
	calSelect();
	create_table();
	deal_tokens();
	vector<string>::iterator it;
	int k=1;
	for(it=tokens.begin();it!=tokens.end();it++){
		cout<<k<<":"<<(*it)<<endl;
		k++;
	}
	int i, j = 0;
	if (ll1) {
		vector<string> para; 
		for(int x=0;x<tokens.size();x++){
			para.push_back(tokens[x]);
		} 
		i=para.size();
		if (para[i - 1] != "#")
			para.push_back("#");
		for(vector<string>::iterator itv=para.begin();itv!=para.end();itv++){
			cout<<*itv<<"  ";
		} 
		// ��������ջ����#�Ϳ�ʼ������ջ
		stack<string> s;
		s.push("#");
		s.push(G.S);  // #S 
		// �����ͷ 
		string readhead = para[j++];
		while (s.size() != 1) {  //ջ����Զ��# 
			string top = s.top();
			s.pop();
			if (G.Vt.find(top) != G.Vt.end()) {  //�����ջ��Ԫ�����ս�� 
				if (top != "$") {  //���ջ�������Ԫ�ز��ǿ�$ 
					if (top == readhead)     //������Ԫ�������������ͷ��Ԫ����ͬ 
						readhead = para[j++];  //���¶� 
					else {   //������������������ʹ���˲���ʽ�Ƶ����������  �����󲻴���Դ���� 
						cout<<"ʹ���˴���Ĳ���ʽ���´���"<<endl;
						system("pause");
						return 0;
					}
				}
			}
			else {   //�����ջ��Ԫ���Ƿ��ս�� 
				int row = get_row(top);
				int column = get_column(readhead);
				int index = G.table[row][column];    //table ��ʾԤ��������е�row�еķ��ս��������column�е��ս��ʱʹ�õ��ǵڶ��ٺŲ���ʽ 
				if (index == -1) { //Ԥ����������Ҳ�����Ӧ���õĲ���ʽ  �Ƶ�����ȥ 
					cout<<"Ԥ����������Ҳ�����Ӧ���õĲ���ʽ"<<endl;
					system("pause");
					return 0;
				}
				else {
					int k;
					for (k = G.expressions[index].right.size() - 1; k >= 0; k--) {
						s.push(G.expressions[index].right[k]); //index�Ų���ʽ�ұߵ�Ԫ����ջ �Ӻ���ǰ�� 
					}
				}
			}
		}   //ֻ��ջ��Ԫ�ر�ƥ������ ��S-> һ������ 
		if (s.top() == readhead) { //�����ʱ����� ���� ����#������Ԫ�� ����û�б��Ƶ��� 
			cout<<"�﷨�����ɹ���"<<endl;
			system("pause");
			return 0;
		}
	}
	cout<<"���ķ�����ll(1)�ķ���"<<endl;
	system("pause");
	return 0;
}
