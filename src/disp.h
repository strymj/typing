#ifndef DISP_H
#define DISP_H

#include <iostream>/*{{{*/
#include <cmath>
#include "kbhit.h"
using namespace std;/*}}}*/

class Disp/*{{{*/
{
	private:
		int ncin(int);
		char scin(int);
	public:
		Disp(){};
		void move(int,int,bool);
		void clear(int);
		void cprint(int,string);
		int ndef(int,int,int,bool);
		bool yndef(int,bool);
		int nod(int);
		void clearcin();
};/*}}}*/

int getch(void)/*{{{*/
{
	struct termios oldattr, newattr;
	int ch;
	tcgetattr( STDIN_FILENO, &oldattr );
	newattr = oldattr;
	newattr.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
	return ch;
}/*}}}*/

void Disp::move(int up, int right, bool clear)/*{{{*/
{
	if (up>0) for (int i=0; i<up; i++) cout<<"\x1b[1A";
	else for (int i=0; i<-up; i++) cout<<"\x1b[1B";
	if (right>0) for (int i=0; i<right; i++) cout<<"\x1b[1C";
	else for (int i=0; i<-right; i++) cout<<"\x1b[1D";
	if (clear) cout<<"\x1b[K";
}/*}}}*/

void Disp::clear(int rows)/*{{{*/
{
	for (int i=0; i<rows; i++) cout<<"\x1b[1A"<<"\x1b[K";
}/*}}}*/

void Disp::cprint(int color,string sentence)   // 0:黒 1:赤 2:緑 3:黄 4:青/*{{{*/
{
	if (color == 0) cout<<"\x1b[30m";
	else if (color == 1) cout<<"\x1b[31m";
	else if (color == 2) cout<<"\x1b[32m";
	else if (color == 3) cout<<"\x1b[33m";
	else if (color == 4) cout<<"\x1b[34m";
	cout<<sentence<<"\x1b[39m";
}/*}}}*/

int Disp::ncin(int cols)/*{{{*/
{
	int num;
	cout<<endl;
	move(1,cols,1);
	while (!(cin >> num))
	{
		cin.clear();
		cin.ignore(256,'\n');
		move(1,cols,1);
	}
	cin.clear();
	cin.ignore(256,'\n');
	return num;
}/*}}}*/

char Disp::scin(int cols)/*{{{*/
{
	char s;
	cout<<endl;
	move(1,cols,1);
	while (!(cin >> s))
	{
		cin.clear();
		cin.ignore(256,'\n');
		move(1,cols,1);
	}
	cin.clear();
	cin.ignore(256,'\n');
	return s;
}/*}}}*/

int Disp::ndef(int cols,int min, int max, bool disp)/*{{{*/
{
	int ans = max + 1;
	while (!(min<= ans && ans<=max))
	{
		ans = ncin(cols);
		move(1,cols,1);
	}
	if (disp) cout<<ans;
	return ans;
}/*}}}*/

bool Disp::yndef(int cols, bool disp)/*{{{*/
{
	char s = ' ';
	bool ans = false;
	while (s != 'y' && s != 'n')
	{
		s = scin(cols);
		move(1,cols,1);
	}
	if (disp) cout<<s;
	if (s=='y') ans = true;
	return ans;
}/*}}}*/

int Disp::nod(int num)/*{{{*/
{
	return 1+(int)log10(num);
}/*}}}*/

void Disp::clearcin()/*{{{*/
{
	cin.clear();
	cin.ignore(256,'\n');
}/*}}}*/

#endif
