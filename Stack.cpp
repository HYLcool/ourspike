#include<iostream>
#include "Stack.h"
using namespace std;

Stack::Stack(){
	stackLength=1000001;
	stacktop=0;
	for(int i=0;i<=stackLength-1;i++){
		s[i]=0;
	}
}

bool Stack::push(long long unsigned val,int w){  //hign in hign
	unsigned int v[8];       //high to low
	unsigned int c=(1<<8)-1;
	for(int i=0;i<=7;i++){
		v[i]=(val>>((7-i)*8))&c;
	}

	for(int i=7;i>=(8-w);i--){
		if(stacktop>=stackLength){
			cout<<"stack overflow when push"<<endl;
			return 0;
		}
		s[stacktop++]=v[i];
	}
	return 1;


}
long long unsigned Stack::pop(int w){
	unsigned v[8];
	long long unsigned ans=0;
	if(stacktop<w){
		cout<<"stack overflow when pop"<<endl;
		return ~0;
	}
	for(int i=0;i<=w-1;i++){
		v[i]=s[--stacktop];
	}
	for(int i=0;i<=w-1;i++){
		ans+=v[i]<<(8*(w-i-1));
	}
	return ans;


}


/*int main(){
	Stack s=Stack();
	//cout<<s.pop(1)<<endl;
	s.push(12,1);
	cout<<s.pop(1)<<endl;
	//cout<<sizeof(s.pop(1))<<endl;
	s.push((1<<31)+33,4);
	cout<<s.pop(4)<<endl;
	//s.pop(1);
	//cout<<sizeof()
	return 0;
}*/