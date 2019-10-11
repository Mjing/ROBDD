#include <stdio.h>
#include "prop.h"
#include <queue>
#include <algorithm>
#ifndef PROP_H
#define PROP_H 




bool cmp (int i,int j) { return (i<j); }

// return : NOT (x)
prop* negation(prop*x){
	if(x->type == 0)
		return x->left;
	else{
		prop* ret = new prop(0);
		ret->left = x;
		return ret;
	}
}

//x and y
prop* andop(prop*x, prop*y){
	prop* ret = new prop(1);
	ret->left = x;
	ret->right= y;
	return ret;
}

//BIG_AND (s)
prop* conjunction(vector <prop*> s){
	prop * ret =0;
	for(vector<prop*>::iterator it = s.begin();it != s.end();it++){
		if(ret){
			ret = andop(ret, *it);
		}
		else
			ret = *it ;
	}
	return ret;
}

prop* disjunction(vector <prop*> s){
	if(s.size() == 1)
		return s[0];
	vector<prop*> not_s;
	for (vector<prop*>::iterator it = s.begin(); it!= s.end();it++){
		not_s.push_back(negation(*it));
	}
	prop * ret = negation(conjunction(not_s));
	return ret;
}

// x->y
prop* implication(prop* x , prop* y){
	prop * not_x = negation(x);
	vector <prop*> OR;
	OR.push_back(not_x);
	OR.push_back(y);
	return disjunction(OR);
}

void prop::print(int i){
	printf("%d:%d ",type, i );
	if(left != NULL)
		left->print(i+1);
	if(right != NULL)
		right->print(i+1);
}

prop prop::substitute(int id, int with){
	//with \in {1:true, 0:false}
	prop ret;
	if(id == type){
		ret.type = -1 -  with;
		return ret;
	}

	else if(type == 0){
		prop l = left->substitute(id, with);
		if(l.type == -1)
			ret.type = -2;
		else if(l.type == -2)
			ret.type = -1;
		else{
			ret.type = 0;
			ret.left = new prop;
			*ret.left = l;
		}
		// printf("not %d\n", ret.type);
		return ret;
	}
	//AND
	else if(type == 1){
		prop l = left->substitute(id , with), r = right->substitute(id,with);
		if(l.type == -1 || r.type == -1)
			ret.type = -1;
		else if (l.type == -2)
			ret = r;
		else if (r.type == -2)
			ret = l;
		else{
			ret.type = 1;
			ret.left = new prop();
			ret.right = new prop();
			*ret.left = l, *ret.right = r;
		}
		// printf("and %d\n", ret.type);
		return ret;
	}
	ret.type = type;
	// printf("id %d\n", ret.type);
	return ret;
}

bool prop::well_formed(int limit){
	if(type > limit)
		return false;
	switch (type){
		case 0:
			if(left == 0 || right!= 0){
				return false;
			}
			else
				return left->well_formed(limit);
		case 1:
			if(left == 0 || right == 0)
			{	
				return false;
			}
			else
				return left->well_formed(limit) && right->well_formed(limit);
		default:
			if(left == 0 && right == 0)
				return true;
			else 
				return false;
	}
}

vector <int> prop::sort (){
	vector <int> ret;
	queue <prop *> bfs;
	bfs.push(this);
	prop * curr;
	while (! bfs.empty()){
		curr = bfs.front();
		bfs.pop();
		switch (curr->type){
			case 0:	//not
				bfs.push(curr->left);
				break;
			case 1: //and
				bfs.push(curr->left);
				bfs.push(curr->right);
				break;
			default: //id
				ret.push_back(curr->type);
		}
	}
	std::sort(ret.begin(), ret.end(), cmp);
	vector<int> new_ret;
	int last = ret[0];
	new_ret.push_back(last);
	#ifdef DEBUG_MODE
		printf("asd %d %d\n", ret[0], new_ret[0]);
	#endif
	//Induction variables
	vector<int>::iterator it = ret.begin();
	it++;
	//inv: it >= last
	for (it; it != ret.end(); it++){
		// printf("cc %d %d\n", last, *it);
		if(*it != last){
			last = *it;
			new_ret.push_back(last);
		}
	}

	// printf("dccc %d\n", (new_ret.size() == ret.size()));
	return new_ret;
}

#endif

