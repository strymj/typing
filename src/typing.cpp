// typing/*{{{*/
// ～更新履歴～
// 2016.04.07   ver1.0  作成
//      04.08   ver2.0  試験的に文バージョンを導入
//      04.14   ver2.1  missファイルの保存形式の変更
/*}}}*/

#include <ros/ros.h>/*{{{*/
#include <std_msgs/Int32.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>     // for rand()
#include <ctime>       // for srand()
#include <string>
#include <sstream>
#include <vector>
#include "disp.h"

#define STARN 5

using namespace std;/*}}}*/

class Ety/*{{{*/
{
	private:
		int dat2num(unsigned char);
		void print_japanese(int);
		void print_unchiku(int);
		vector<string> english;
		vector<string> japanese;
		vector<unsigned char> wrong_dat;
		vector<int> n_sort;
		vector<int> n_miss;
		char filename[2][3][64];
		int modeselect;
		int state;
		int regist_num;
		int question_num;
		int miss;
		int min_num;
		int max_num;
		int range_num;
		int kagen;
		enum {NOMAL=0, STAR=1, MISS=2};
		enum {WORD=0, STATEMENT=1};
	public:
		Disp disp;
		void output_filepass();
		void hello();
		int input_mode();
		int state_select();
		void freadwords();
		void freadstatements();
		void freadmissrate();	
		void input_range();
		void input_starrate();
		void sort();
		void miss_sort();
		void star_sort();
		void shuffle();
		void countdown();
		void run();
		void fwritemissn();
		bool result();
		void see_you();
		ros::NodeHandle nh;
		ros::Publisher sound_pub;
		std_msgs::Int32 sound_msg;
		Ety()
		{
			sound_pub = nh.advertise<std_msgs::Int32>("ring",1);
			strcpy(filename[0][0], "../ros_catkin_ws/src/typing/txtfile/duotan.txt");
			strcpy(filename[0][1], "../ros_catkin_ws/src/typing/record/misstan.txt");
			strcpy(filename[0][2], "../ros_catkin_ws/src/typing/record/misstam_sub.txt");
			strcpy(filename[1][0], "../ros_catkin_ws/src/typing/txtfile/duobun.txt");
			strcpy(filename[1][1], "../ros_catkin_ws/src/typing/record/missbun.txt");
			strcpy(filename[1][2], "../ros_catkin_ws/src/typing/record/missbun_sub.txt");
			modeselect = 0;
			regist_num = 0;
			miss = 0;
			state = 0;
			kagen = 0;
		};
		void spin();
};/*}}}*/

int Ety::dat2num(unsigned char num)/*{{{*/
{
	int ans = 0;
	for (int i=0; i<STARN; i++) {
		ans +=  num%2;
		num /= 2;
	}
	return ans;
}/*}}}*/

void Ety::print_japanese(int now)/*{{{*/
{
	for (int i=0; i<japanese[now].size(); i++)
	{
		if (japanese[now][i] == '#') break;
		cout<<japanese[now][i];
	}
}/*}}}*/

void Ety::print_unchiku(int now)/*{{{*/
{
	bool flag = false;
	for (int i=0; i<japanese[now].size(); i++)
	{
		if (flag) cout<<japanese[now][i];
		if (japanese[now][i] == '#') flag = true;
	}
}/*}}}*/

void Ety::output_filepass()/*{{{*/
{
	char pathname[512]; 
	memset(pathname, '\0', 512); 
	if (getcwd(pathname, 512)) cout<<endl<<pathname<<endl;
}/*}}}*/

void Ety::hello()/*{{{*/
{
	sound_msg.data = 0;
	sound_pub.publish(sound_msg);
	cout<<endl<<endl;
	cout<<"========================================"<<endl<<endl;
	cout<<"               -Typing-"<<endl<<endl;
	cout<<"========================================"<<endl<<endl<<endl;
	// 0
	cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl;   // 0->8
	disp.move(8,0,0);
}/*}}}*/

int Ety::input_mode()/*{{{*/
{
	//////////////////////////////////////////////////////////////////////
	cout<<"          < mode select >"<<endl;   // 0->1
	cout<<endl;
	cout<<"          [0] : words"<<endl;
	cout<<"          [1] : statements"<<endl;   // 3->4
	//////////////////////////////////////////////////////////////////////
	modeselect = disp.ndef(0,0,1,false);
	disp.clear(4);   // 3-0
	return modeselect;
}/*}}}*/

int Ety::state_select()/*{{{*/
{
	char s;
	//////////////////////////////////////////////////////////////////////
	cout<<"          < state select >"<<endl;
	cout<<endl;
	cout<<"        [0]  範囲指定"<<endl;   // 2->3
	cout<<"        [1]  ミスの多い単語から"<<endl;   // 3->4
	if (miss) cout<<"        [2]  間違えた単語から";   // 4
	cout<<endl;   // 4->5
	//////////////////////////////////////////////////////////////////////
	int statemax = 1;
	if (miss) statemax = 2;
	state = disp.ndef(0,0,statemax,false);
	/*while (1)
	{
		cin >> s;
		disp.clearcin();   // 5->6
		if (s == '0')
		{
			state = NOMAL;
			break;
		}
		else if (s == '1') {
			state = STAR;
			break;
		}
		else if (miss && s == '2') {
			state = MISS;
			break;
		}
		disp.clear(1);
	}*/
	disp.clear(5);   // 4-0
	return state;
}/*}}}*/

void Ety::freadwords()/*{{{*/
{
	ifstream ifs(filename[modeselect][0]);
	if (!ifs) {
		cerr<<"Can't open file"<<endl;
		exit(1);
	}
	string input_str;
	string tmp;
	while (getline(ifs,input_str,','))
	{
		english.push_back(input_str);
		getline(ifs,input_str);
		japanese.push_back(input_str);
		regist_num++;
	}
	if (regist_num == 0)
	{
		cerr<<"Empty file"<<endl;
		exit(2);
	}
	ifs.close();
}/*}}}*/

void Ety::freadstatements()/*{{{*/
{
	ifstream ifs(filename[modeselect][0]);
	if (!ifs) {
		cerr<<"Can't open file"<<endl;
		exit(1);
	}
	string input_str;
	string tmp;
	while (getline(ifs,input_str,'.'))
	{
		getline(ifs,input_str,'#');
		english.push_back(input_str);
		getline(ifs,input_str,'.');
		getline(ifs,input_str);
		japanese.push_back(input_str);
		regist_num++;
	}
	if (regist_num == 0)
	{
		cerr<<"Empty file"<<endl;
		exit(2);
	}
	ifs.close();
}/*}}}*/

void Ety::freadmissrate()/*{{{*/
{
	wrong_dat = vector<unsigned char>(regist_num,0x00);
	ifstream ifs(filename[modeselect][1]);
	if (ifs) {
		unsigned char s;
		for (int i=0; i<regist_num; i++)
		{
			ifs >> s;
			if (ifs.eof()) break;
			wrong_dat[i] = s & 0xbf;   // 7bit目を0
		}
		ifs.close();    // 開けなかったときにどうなるのか
	}
}/*}}}*/

void Ety::input_range()/*{{{*/
{
	question_num = 0;
	min_num = 0;
	max_num = 0;
	//////////////////////////////////////////////////////////////////////
	cout<<endl;  // 0->1
	cout<<"          < 登録数   "<<regist_num<<" >"<<endl;   // 1
	cout<<"          出題範囲 :"<<endl;   // 2
	cout<<"           問題数  :"<<endl;   // 3->4
	//////////////////////////////////////////////////////////////////////
	disp.move(2,0,0);   // ->2
	min_num = disp.ndef(21,1,regist_num,true);
	cout<<" ~ ";
	max_num = disp.ndef(24+disp.nod(min_num),1,regist_num,true);
	if (max_num < min_num)
	{
		int tmp = max_num;
		max_num = min_num;
		min_num = tmp;
		cout<<endl;
		disp.move(1,21,1);
		cout<<min_num<<" ~ "<<max_num;
	}
	range_num = max_num - min_num + 1;
	cout<<" ("<<range_num<<"words)"<<endl;
	question_num = disp.ndef(21,1,range_num,true);
	cout<<endl;
	disp.clear(4);   // 3-0
}/*}}}*/

void Ety::input_starrate()/*{{{*/
{
	int num[STARN+1] = {0};
	for (int i=0; i<regist_num; i++)
	{
		num[dat2num(wrong_dat[i])]++;
	}
	//////////////////////////////////////////////////////////////////////
	cout<<"           ★           :  "<<num[1]<<" words"<<endl;   // 0->1
	cout<<"           ★ ★         :  "<<num[2]<<" words"<<endl;
	cout<<"           ★ ★ ★       :  "<<num[3]<<" words"<<endl;
	cout<<"           ★ ★ ★ ★     :  "<<num[4]<<" words"<<endl;
	cout<<"           ★ ★ ★ ★ ★   :  "<<num[5]<<" words"<<endl;   // 4->5
	cout<<endl;   // 5->6
	cout<<"                 ★   "<<endl;   // 6->7
	cout<<"            以上の問題を出題"<<endl;   // 7->8
	//////////////////////////////////////////////////////////////////////
	disp.move(2,0,0);   // 8->6
	kagen = disp.ndef(20,1,STARN,true);
	cout<<endl<<endl;   // 6->8
	disp.clear(8);   // 7-0
}/*}}}*/

void Ety::sort()/*{{{*/
{
	n_sort = vector<int>(range_num,0);
	for (int i=0; i<range_num; i++)
	{
		n_sort[i] = min_num+i-1;
	}
}/*}}}*/

void Ety::miss_sort()/*{{{*/
{
	question_num = miss;
	range_num = miss;
	n_sort = vector<int>(range_num,0);
	for (int i=0; i<range_num; i++)
	{
		n_sort[i] = n_miss[i];
	}
}/*}}}*/

void Ety::star_sort()/*{{{*/
{
	int num = 0;
	for (int i=0; i<regist_num; i++)
	{
		if (dat2num(wrong_dat[i]) >= kagen)
		{
			n_sort.push_back(i);
			num++;
		}
	}
	question_num = num;
	range_num = num;
}/*}}}*/

void Ety::shuffle()/*{{{*/
{
	srand((unsigned int)time(NULL));
	int rand_num;
	string stmp;
	int ntmp;
	for (int i=0; i<question_num; i++)
	{
		rand_num  = rand()%(range_num-i);

		ntmp = n_sort[i];
		n_sort[i] = n_sort[rand_num];
		n_sort[rand_num] = ntmp;
	}
}/*}}}*/

void Ety::countdown()/*{{{*/
{
	//////////////////////////////////////////////////////////////////////
	cout<<endl;
	cout<<endl;   // 0->2
	cout<<"            Are you ready?"<<endl;  // 2->3
	cout<<"        (Hit any key to start!)"<<endl;   // 3->4
	//////////////////////////////////////////////////////////////////////
	getch();
	disp.clear(2);   // 3-2
	cout<<"                < 3 >"<<endl<<endl;
	sleep(1);
	disp.clear(2);	
	cout<<"               << 2 >>"<<endl<<endl;
	sleep(1);
	disp.clear(2);	
	cout<<"              <<< 1 >>>"<<endl<<endl;   // 2->4
	sleep(1);
	disp.clear(4);	// 3-0
}/*}}}*/

void Ety::run()/*{{{*/
{
	n_miss = vector<int>(question_num,0);
	miss = 0;
	string answer;
	for (int i=0; i<question_num; i++)
	{
		cout<<" ("<<i+1<<"/"<<question_num<<")               ";
		cout<<endl;
		disp.move(1,12,0);
		cout<<"間違い度 ";
		for (int j=0; j<STARN; j++)
		{
			if (j<dat2num(wrong_dat[n_sort[i]])) cout<<"★ ";
			else cout<<"☆ ";
		}
		cout<<" ("<<dat2num(wrong_dat[n_sort[i]])<<")";
		cout<<endl<<endl;   // 0->2
		cout<<"   No."<<n_sort[i]+1<<endl;
		disp.move(1,12,0);
		print_japanese(n_sort[i]);   // 2
		cout<<endl<<endl<<endl;   // 2->5
		disp.move(0,10,0);
		int w_num = 1;
		for (int j=0; j<english[n_sort[i]].length(); j++) {
			if (english[n_sort[i]][j] == ' ') w_num++;
		}
		if (w_num != 1) cout<<"("<<w_num<<")  ";
		else cout<<"     ";
		cout<<english[n_sort[i]][0]<<"-  :       "<<endl;   // 5->6
		disp.move(1,23,0);   // 6->5
		
		getline(cin,answer);   // 5->6
		disp.move(1,23,0);   // 6->5

		wrong_dat[n_sort[i]] = wrong_dat[n_sort[i]] << 1;
		if (answer == english[n_sort[i]])
		{
			disp.cprint(2,answer);
			cout<<endl<<endl;   // 5->7
			sound_msg.data = 1;
		}
		else
		{
			disp.cprint(1,answer);
			cout<<endl;   // 5->6
			disp.move(0,23,0);
			cout<<english[n_sort[i]]<<endl;  // 6->7
			sound_msg.data = 2;
			n_miss[miss] = n_sort[i];
			miss++;
			wrong_dat[n_sort[i]] += 1;
		}
		wrong_dat[n_sort[i]] = wrong_dat[n_sort[i]] & 0x1F;

		disp.move(4,13,0);   // 7->3
		print_unchiku(n_sort[i]);   // 3
		cout<<endl<<endl<<endl<<endl;   // 3->7

		fwritemissn();
		sound_pub.publish(sound_msg);
		getch();
		disp.clear(7);   // 6-0
	}
}/*}}}*/

void Ety::fwritemissn()/*{{{*/
{
	ofstream ofs(filename[modeselect][2]);
	if (!ofs) {
		cerr<<"Can't open file"<<endl;
		exit(3);
	}
	unsigned char input_s;
	for (int i=0; i<regist_num; i++)
	{
		input_s = wrong_dat[i] | 0x40;   // 7bit目を1
		ofs << input_s;
	}
	ofs.close();
	remove(filename[modeselect][1]);
	rename(filename[modeselect][2],filename[modeselect][1]);
}/*}}}*/

bool Ety::result()/*{{{*/
{
	cout<<endl;   // 0->1
	cout<<"           お疲れさまでした"<<endl;   // 1->2
	cout<<"           正答率 : "<<question_num-miss<<"/"<<question_num<<endl<<endl;   // 2->4
	cout<<"           continue? y/n"<<endl;   // 4->5
	bool ans = disp.yndef(0,false);
	disp.clear(5);   // 4-0
	return ans;
}/*}}}*/

void Ety::see_you()/*{{{*/
{
	cout<<endl;   // 0->1
	cout<<"          Thanks for playing!"<<endl<<endl;   // 1->3
	cout<<"            See you again!"<<endl<<endl<<endl;   // 3->6
}/*}}}*/

void Ety::spin()/*{{{*/
{
	sleep(1);
	//output_filepass();
	hello();
	switch (input_mode()) {
		case WORD: 
			freadwords();
		case STATEMENT:
			freadstatements();
	}
	freadmissrate();
	while(1)
	{
		switch (state_select()) {
			case NOMAL:
				input_range();
				sort();
				break;
			case STAR:
				input_starrate();
				star_sort();
				break;
			case MISS:
				miss_sort();
				break;
		}
		n_miss.clear();
		shuffle();
		countdown();
		run();
		n_sort.clear();
		if (!result()) break;
	}
	see_you();
}/*}}}*/

int main(int argc, char **argv)/*{{{*/
{
	ros::init(argc, argv, "typing");
	Ety ety;
	ety.spin();
	return 0;
}/*}}}*/
