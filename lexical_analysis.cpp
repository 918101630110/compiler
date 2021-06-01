#include <iostream>
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
/* TOKEN���� */
typedef enum {
    /*״̬ */
    ENDL,ERROR,
    /*�ؼ��� */
    IF,ELSE,INT,CHAR,FLOAT,RETURN,VOID,WHILE,FOR,AND,OR,NOT,PRINT,
    /*��ʶ�� */
    ID,NUMA,NUMB,NUMC,NUMD,NUME,NUMF,
    /*�����*/
    /*+ - * / < <= > >= == != = ; , ( ) [ ] { } # :*/
    ADD,SUB,MUL,DIV,LT,LE,GT,GE,EE,EQ,NE,SEMI,COMMA,LPAR,RPAR,LBRA,RBRA,LCUR,RCUR,TAG,COLON
} TokenType;
/* DFA״̬����*/
typedef enum {          //A.BDeCFE    �Ը������ж���A��ʼ 
    START,SID,SNUMA,SNUMB,SNUMC,SNUMD,SNUME,SNUMF,SNUMG,
    SGT,SLT,SEQ,SNE,SEE,DONE
} StateType;
map<string,TokenType> keyWords;
map<string,StateType> keyState;
map<string,string> category;
struct Ans {
	int line;			//�ʷ�������
	int column;			//�ʷ�������
	string classify;	//�ʷ�token����
	string value;		//�ʷ�����
	string error;		//��������
	Ans(int l,int c,string cl,string v,string e) {  //���캯��
		line=l;
		column=c;
		classify=cl;
		value=v;
		error=e;
	}
};
struct Node {
	char pre,next;   //��ǰ״̬����һ״̬��ת������
	string data;
	Node(char p,string d,char n) {  //���캯��
		pre=p;
		data=d;
		next=n;
	}
};
struct FA {
	int edgeNum;			// ����ͳ��
	int nodeNum;			// �ڵ�ͳ��
	string state;		//����״̬��
	string alphabet;		//������ĸ��
	vector<Node>tranSet;	//ת�������������СΪtranSet[nodeNum][nodeNum]
	string startNode;		//�ǿճ�̬��
	vector<char> endNode;	//��̬��
} DFA,NFA;
//�����ķ��ṹ��
struct EX {
	set<char> VN;			//���ս��
	string VT;				//�ս��
	string S;				//��ʼ��
	vector<Node>P;			//����ʽ
} express;
void preInit();
void init();
void inputEX(string rule);
set<char> Closure(FA nfa, char st);
set<char> unionSet(set<char>s1,set<char>s2);
set<char> e_move(FA nfa,char st, char ch);
bool equalSet(set<char> s1, set<char> s2);
void createNFA(string express);
FA createDFA(FA nfa);
bool isLetter(char ch);
bool isDigit(char ch);
map<char,char> getTrans(char nowNode);
Ans runDFA(string str,int pos,int line);
void preInit() {
	keyWords["if"]=IF;
	keyWords["else"]=ELSE;
	keyWords["int"]=INT;
	keyWords["char"]=CHAR;
	keyWords["float"]=FLOAT;
	keyWords["return"]=RETURN;
	keyWords["void"]=VOID;
	keyWords["while"]=WHILE;
	keyWords["for"]=FOR;
	keyWords["and"]=AND;
	keyWords["or"]=OR;
	keyWords["not"]=NOT;
	keyWords["print"]=PRINT;
	keyState["START"]=START;
	keyState["ID"]=SID;
	keyState["NUMA"]=SNUMA;
	keyState["NUMB"]=SNUMB;
	keyState["NUMC"]=SNUMC;
	keyState["NUMD"]=SNUMD;
	keyState["NUME"]=SNUME;
	keyState["NUMF"]=SNUMF;
	keyState["NUMG"]=SNUMG;
	keyState["GT"]=SGT;
	keyState["LT"]=SLT;
	keyState["EQ"]=SEQ;
	keyState["NE"]=SNE;
	keyState["EE"]=SEE;
	keyState["DONE"]=DONE;
}
void init() {
	fstream fp("./3.txt",ios::in);
	if(!fp) {
		cout<<"�ļ���ʧ��"<<endl;
		exit(-1);
	}
	string rule;
	express.S=keyWords["START"]+'A';
	while(!fp.eof()) {
		fp>>rule;
		inputEX(rule);
	}
}
void inputEX(string rule) {       //�洢�����ķ��������ݽṹ
	int pre=rule.find('-');
	int last=rule.find('>')+1;
	string vn1=rule.substr(0,pre);//���ս�� ��ߵ�
	string vt=rule.substr(last,1);  //�ս�� ���ڵ�
	string vn2=rule.substr(last+1,rule.length()-last-1); //���ս�� �ս���ұߵ�
	express.VN.insert(keyState[vn1]+'A');
	express.VN.insert(keyState[vn2]+'A');
	if(express.VT.find(vt)==string::npos&&(vt[0]!='$')) { //�Ҳ���������ս�����Ҹ÷��ս����Ϊ$
		express.VT+=vt;
	}
	express.P.push_back(Node((keyState[vn1]+'A'),vt,(keyState[vn2]+'A')));
}
// ����һ���ڵ�Ħűհ�
set<char> Closure(FA nfa, char st) {
	int pos;
	set<char> result;
	stack<char> s;
	s.push(st);
	while (!s.empty()) {
		char ch =s.top();
		s.pop();
		result.insert(ch);  //�հ������Լ�

		vector<int> pos;
		for(int i=0; i<nfa.tranSet.size(); i++) { //ͨ��pos��λ��i��ת����
			if(nfa.tranSet[i].pre==ch) {  //chΪ���
				pos.push_back(i);         //�����chΪ�����ת����ϵ�ͼ�¼
			}
		}
		for(int i=0; i<pos.size(); i++) {
			if(nfa.tranSet[pos[i]].data[0]=='$') {  //�������ת����ϵ��ת�������ǿ�$
				s.push(nfa.tranSet[pos[i]].next);//ͨ��e�ܵ���
			}
		}
	}
	return result;
}
//�ϲ���������
set<char> unionSet(set<char>s1,set<char>s2) {
	s1.insert(s2.begin(),s2.end());
	return s1;
}
//����һ���ڵ�st����ת���ַ�chת���õ��Ľ�����űհ������
set<char> e_moveSet(FA nfa,char st, char ch) {
	set<char> result;
	vector<int> pos;
	for(int i=0; i<nfa.tranSet.size(); i++) {
		if(nfa.tranSet[i].pre==st) {
			pos.push_back(i);
		}
	}
	for(int i=0; i<pos.size(); i++) { //move(T,a)
		if(nfa.tranSet[pos[i]].data[0]==ch) {
			result.insert(nfa.tranSet[pos[i]].next);
		}
	}
	for(set<char>::iterator it=result.begin(); it!=result.end(); it++) {
		// �����-�հ�������
		result=unionSet(result,Closure(nfa,*it));  //ʹ��set ���õ���Ԫ���ظ� 
	}
	return result;
}
// �ж����������Ƿ���ͬ
bool equalSet(set<char> s1, set<char> s2) {
	// ��С��һ���ļ��ϱ�Ȼ��ͬ
	if (s1.size()!=s2.size()) {
		return false;
	}
	set<char> temp=unionSet(s1,s2);

	if (temp.size()!=s1.size()) {
		return false;
	} else {
		return true;
	}
}
void createNFA(EX express) {
	NFA.nodeNum=express.VN.size(); //�ж��ٸ����ս�����ж��ٵ�
	NFA.edgeNum=express.P.size(); //������3���ķ� һ������ʽ��Ӧһ����
	NFA.alphabet=express.VT;//��ĸ��
	NFA.startNode=express.S;
	NFA.tranSet=express.P;//����ʽ
	for(set<char >::iterator it=express.VN.begin(); it!=express.VN.end(); it++) {
		NFA.state+=*(it);       //��һ�����ս�����ǳ�̬
	}//15״̬��A~O��
	int flag;
	//������ֹ�ڵ�
	for(int i=0; i<NFA.state.size(); i++) {
		flag=0;  //ÿ�ν�������0 ��ÿ��״̬���ж��ǲ����ս�̬
		for(int j=0; j<NFA.tranSet.size(); j++) {
			if(NFA.tranSet[j].pre==NFA.state[i]) {
				flag=1;  //�ҵ�ƥ��Ĳ���ʽ  ��״̬�������ս�״̬
				break;
			}
		}
		if(!flag) {             //flagδ����1��û�ж�Ӧ�Ĳ���ʽ����Ϊ�ս�̬
			NFA.endNode.push_back(NFA.state[i]);
			flag=0;
		}
	}
	//д�ļ�
	ofstream out("./NFA.txt");
	if(out.fail()) {
		cout<<"���������";
	}
	for(int i=0; i<NFA.tranSet.size(); i++) {
		out<<NFA.tranSet[i].pre<<"##"<<NFA.tranSet[i].data<<"##"<<NFA.tranSet[i].next<<endl;
	}
	out.close();

}
FA createDFA(FA nfa) {
	FA dfa;
	vector<set<char> > Q;//����DFA�ڵ㼯��
	queue<set<char> > s;//�����Ӽ��仯���������ѭ��    s��ͱ�������DFA�Ľڵ�ֱ���ڵ����������� 

	s.push(Closure(nfa,nfa.startNode[0]));//�����ʼ״̬�ıհ� 
	Q.push_back(Closure(nfa,nfa.startNode[0]));

	queue<set<char> > transEdge;//����ת���������Qһһ��Ӧ

	while(!s.empty()) {
		set<char> top=s.front();//�õ�����Ԫ��һ��set
		s.pop();		//����Ԫ�س���

		//��ÿһ��ת�����ַ�����ת�����
		for(int i=0; i<nfa.alphabet.size(); i++) {
			set<char>temp;
			int flag=0;

			//��move(top,i)
			for(set<char>::iterator it=top.begin(); it!=top.end(); it++) {
				temp=unionSet(temp,e_moveSet(nfa,*it,nfa.alphabet[i]));
			}       //tempΪtop������Ԫ��move�󾭹�eclosure����ɵ�һ��set   DFA��һ���ڵ� 

			// �����ǰ�ڵ㾭��ת���ַ�ת�����ռ��������һ����ʾ�ռ��ķ���
			if(temp.size()==0) {
				temp.insert('$');
			} 
			else { //����Q,��δ����Ӽ�����ջ��
				for(vector<set<char> >::iterator Qit=Q.begin(); Qit!=Q.end(); Qit++) {
					if(equalSet(*Qit,temp)) {
						flag=1;
					}
				}
				if(flag==0) {     //���Ǹ��µĽڵ�
					s.push(temp);
					Q.push_back(temp);
				}
			}
			transEdge.push(temp); //ת����������  �����ǲ����½ڵ㶼Ҫ���뵽transEdge�� ����ת���Ľ�� 
		}
	}
	//�����µ�DFA
	int i,j;
	dfa.nodeNum=Q.size();
	dfa.alphabet=nfa.alphabet;//������ĸ��

	//����ڵ㣨״̬��
	for(i=0; i<dfa.nodeNum; i++) {
		dfa.state+=i+'A';  //Q�е�һ�����ǳ�̬
	}

	//��ÿһ��ת�����ַ�������ת����ϵ
	i=0;
	while(!transEdge.empty()) {    //transEge������ÿ��״̬Qͨ������alphabet�ߵ���״̬Q 
		for(int p=0; p<dfa.alphabet.size(); p++) {
			set<char>temp=transEdge.front();//tempΪtransEgde����setԪ��  һ��eclosure(move(x,x))  һ��DFA�ڵ� 
			transEdge.pop();
			if(temp.size()==1&&*temp.begin()=='$') {  //�������ڵ�������ǿ�$
				continue;
			} else {
				for(j=0; j<Q.size(); j++) { //�ҳ�λ��j,ת�ƽ���ı��   �ж��temp��Qͨ��alphabetת��������
					if(equalSet(temp,Q[j])) {  //�ҵ���ǰ��״̬Q[i]ͨ��alphbetת�������Ǹ�״̬Q[j] 
						break;
					}
				}
				string str;  //�洢node.data 
				str.push_back(dfa.alphabet[p]);
				dfa.tranSet.push_back(Node(i+'A',str,j+'A'));
			}
		}
		i++;  //������һ��alphbet�ʹ���������˴ӵ�һ��״̬ͨ��alphbet���������״̬���� ��ʼ������һ��״̬
	}

	dfa.edgeNum=dfa.tranSet.size(); //��������ת�����ٸ�Q��transEdge
	//�����̬
	dfa.startNode=dfa.state[0];  //245�� Q�е�һ�����ǳ�̬

	//������̬
	int flag;
	//������ֹ�ڵ�
	//��״̬T����ԭ����̬������״̬Ϊ��̬
	for(i=0; i<Q.size(); i++) {
		for(int j=0; j<nfa.endNode.size(); j++) {
			set<char>::iterator it=Q[i].find(nfa.endNode[j]);  //dfa������״̬�м�Q�а�����ԭnfa�е��ս�̬��Ϊ�ս�̬Q[i]Ϊһ��״̬
			if(it!=Q[i].end()) {     //���findû�ҵ��ˣ��ʹ�������״̬Q����nfa���ս����״̬ 
				dfa.endNode.push_back(dfa.state[i]); //Q������̬ 
			}
		}
	}
	//ʵ�����ս��
	for(int i=0; i<dfa.tranSet.size(); i++) {
		if(dfa.tranSet[i].data[0]=='l') {
			dfa.tranSet[i].data.clear();          //ת���������������l��������ĸ
			dfa.tranSet[i].data.insert(0,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
		} else if(dfa.tranSet[i].data[0]=='d') {
			dfa.tranSet[i].data.clear();     //ת���������������d ����������
			dfa.tranSet[i].data.insert(0,"0123456789");
		}
	}
	//д�ļ�
	ofstream out("./DFA.txt");
	if(out.fail()) {
		cout<<"���������";
	}
	for(int i=0; i<dfa.tranSet.size(); i++) {
		out<<dfa.tranSet[i].pre<<"##"<<dfa.tranSet[i].data<<"##"<<dfa.tranSet[i].next<<endl;
	}
	out.close();

	return dfa;
}
//�ж��Ƿ�����ĸ
bool isLetter(char ch) {
	if((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')) {
		return true;
	}
	return false;
}
//�ж��Ƿ�������
bool isDigit(char ch) {
	if(ch>='0'&&ch<='9') {
		return true;
	}
	return false;
}
map<char,char> getTrans(char nowNode) {
	map<char,char>transfer;
	for(int i=0; i<DFA.tranSet.size(); i++) {
		for(int j=0; j<DFA.tranSet[i].data.size(); j++) {
			if(DFA.tranSet[i].pre==nowNode) {
				transfer[DFA.tranSet[i].data[j]]=DFA.tranSet[i].next;		//����ת����ϵ
			}
		}
	}
	return transfer;
}
string symbol="+-*/<>=!;,()[]{}:";
string limiter=";,()[]{}:";
string operators="+-*/<>=!";
Ans runDFA(string str,int pos,int line) {    //
	string tokenType;  //token���� 
	string endState;   //DFA�ս�̬���� 
	for(int i=0; i<DFA.endNode.size(); i++) {
		endState.push_back(DFA.endNode[i]);
	}
	//�жϱ�ʶ��
	if(isLetter(str[pos])) { //��һ���ַ����ܱ������ǲ��Ǳ�ʶ��
		int finalPos=pos;
		string finalStr;
		//Ԥ��ȡһ���ַ���
		while(finalPos<str.length()&&symbol.find(str[finalPos])==string::npos&&str[finalPos]!=' ') { //�Ҳ���symbol�е��ַ����Һ��Կո�   ��ʶ�����ܺ���symbol�Ϳո�
			finalStr+=str[finalPos];
			finalPos++;
		}
		int curPos=0;
		string token;
		char nowNode=START+'A';//��ʼ
		//��ȡDFA����Ϣ
		map<char,char>transfer=getTrans(nowNode);//�õ���A  ����ʼ����ȥ�ܵõ���ת����ϵ
		while(curPos<finalStr.length()&&(transfer.find(finalStr[curPos])!=transfer.end())) {
			token+=finalStr[curPos];
			nowNode=transfer[finalStr[curPos]];
			transfer.clear();
			transfer=getTrans(nowNode);//�õ�ת��֮��  �����״̬�ܵõ���ת����ϵ
			curPos++;
		}
		if(curPos>=finalStr.length()&&endState.find(nowNode)!=string::npos) { //���finalStr�е�ÿ���ַ�����ȫʶ�����Զ�����nowNode�����ս�̬ ��ʾ�ɹ�ʶ��
			tokenType.clear();
			if(keyWords.find(token)!=keyWords.end()) { //�����keywordsӳ�����ҵ����token�ܹ�ӳ�䵽һ��keyword
				tokenType.insert(0,"keyword");
			} else {            //������Ǳ�ʶ��
				tokenType.insert(0,"identifier");
			}
			return Ans(line,finalPos-1,tokenType,token,"OK");
		} else {
			return Ans(line,finalPos-1,"","","��ʶ���Ƿ���");
		}
	} else if(isDigit(str[pos])) { //�ж�����
		//�ж�
		int finalPos=pos;
		string finalStr;
		//Ԥ��ȡһ���ַ���
		while(finalPos<str.length()&&(symbol.find(str[finalPos])==string::npos||str[finalPos]=='+'||str[finalPos]=='-')&&str[finalPos]!=' ') { //   + - �ܹ���������Ϊ�и����Ĵ���1+i 1-i
			finalStr+=str[finalPos];
			finalPos++;
		}
		int curPos=0;
		string token;
		char nowNode=START+'A';
		//��ȡDFA����Ϣ
		map<char,char>transfer=getTrans(nowNode);  //transfer�Ƕ�һ���ڵ���˵ ÿ���ߵ������ڵ��ӳ��  Sͨ��a����B��һ��charΪa,�ڶ���charΪB
		while(curPos<finalStr.length()&&(transfer.find(finalStr[curPos])!=transfer.end())) {
			token+=finalStr[curPos];
			nowNode=transfer[finalStr[curPos]];  //�ɹ�һ���� ����һ���ڵ�
			transfer.clear();
			transfer=getTrans(nowNode);//�õ�ת��֮��  �����״̬�ܵõ���ת����ϵ
			curPos++;
		}
		if(curPos==finalStr.length()&&endState.find(nowNode)!=string::npos) {
			tokenType.clear();
			tokenType.insert(0,"number");
			return Ans(line,finalPos-1,tokenType,token,"OK");
		}
		return Ans(line,finalPos-1,"","","�����Ƿ���");
	}
	else {  //�Ȳ����ַ�Ҳ�������� 
		int curPos=pos;
		string token;
		tokenType.clear();
		tokenType.insert(0,"limiter");  //�����ж����޶��� 
		char nowNode=START+'A';
		//��ȡDFA����Ϣ
		map<char,char>transfer=getTrans(nowNode);

		while(curPos<str.length()&&(limiter.find(str[curPos])!=string::npos)&&(transfer.find(str[curPos])!=transfer.end())) {
			token+=str[curPos];
			nowNode=transfer[str[curPos]];
			transfer.clear();
			transfer=getTrans(nowNode);
			curPos++;
		}
		if(endState.find(nowNode)!=string::npos) {   //���������̬�ͱ������޶�����������Ǿ�˵���ǲ����� 
			return Ans(line,curPos-1,tokenType,token,"OK");
		}
		curPos=pos;
		token.clear();
		tokenType.clear();
		tokenType.insert(0,"operator");
		while(curPos<str.length()&&(operators.find(str[curPos])!=string::npos)&&(transfer.find(str[curPos])!=transfer.end())) {
			token+=str[curPos];
			nowNode=transfer[str[curPos]];
			transfer.clear();
			transfer=getTrans(nowNode);
			curPos++;
		}
		if(endState.find(nowNode)!=string::npos) {
			return Ans(line,curPos-1,tokenType,token,"OK");
		}
		return Ans(line,curPos-1,"","","��������");
	}
}
int main() {
	preInit();//��ʼ����״̬��ֵ
	init();//��ʼ���ķ����ʽ
	createNFA(express);
	DFA=createDFA(NFA);
	fstream fp("./test1.txt",ios::in);
	if(!fp) {
		cout<<"����������"<<endl;
		exit(-1);
	}
	string str;
	int lineNum=0;
	bool lexError=false;
	vector<Ans> tokenTable;
	while(!fp.eof()) {
		getline(fp,str);//��ȡһ��
		int pos=0;
		lineNum++;
		while(pos<str.length()) {
			while(pos<str.length()&&(str[pos]=='\t'||str[pos]=='\n'||str[pos]==' '||str[pos]=='\r')) {
				pos++;
			}//ͨ��pos�����\t \n  \r ��ʼ��λ��
			if(pos<str.length()) {
				Ans temp=runDFA(str,pos,lineNum);
				pos=temp.column;
				if(temp.classify.length()==0) {
					cout<<"error at line "<<temp.line<<" column "<<temp.column<<" : "<<temp.error<<endl;
					lexError=true;
					break;
				} else {
					tokenTable.push_back(temp);
					cout<<"("<<temp.line<<","<<temp.classify<<","<<temp.value<<")"<<endl;
				}
				pos++;
			}
		}
	}
	if(!lexError) {
		//д�ļ�
		ofstream out("ans.txt");
		if(out.fail()) {
			cout<<"���������";
		}
		for(int i=0; i<tokenTable.size(); i++) {
			out<<tokenTable[i].line<<" "<<tokenTable[i].classify<<" "<<tokenTable[i].value<<endl;
		}
		out.close();
		system("pause");
		return 0;
	}
	cout<<"�ʷ���������";
	system("pause");
	return 0;
}

