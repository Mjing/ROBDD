#include <stdio.h>
#include <vector>
#include <string.h>
#include <map>
#include <tuple>
#include <string.h>
#include <math.h>
#include "prop.h"

int limit = 0;
#define BOUND(x) (x>limit)?printf("outof bound %d %d\n",x,limit):0;	

extern int yyparse();
extern vector<prop*> prop_dump;

using namespace std; 
typedef tuple<int, int, int> tripple;

class Ttable{
	vector <tripple> table; //0-false, 1-true, .....
public:
	Ttable(){
		table.push_back(tripple(0,0,0));
		table.push_back(tripple(1,1,1));
	}

	int size(){
		return table.size();
	}
	int add(tripple t){
		int u = table.size();
		table.push_back(t);
		return u;
	}

	tripple * lookup(int u){
		if(u < table.size())
			return &table[u];
		else 
			return NULL;
	}

};

class Htable{
	map<const tripple, int> table;
public:
	
	//if (return = True) then 1 else 0 
	int member(tripple t){
		return table.count(t);
	}

	int lookup(tripple t){
		if(this->member(t))
			return table[t];
		else{
			return -1;
		}
	}

	int insert(tripple t, int u){
		table[t] = u;
	}

};

class MKtable{
	Ttable t;
	Htable h;

	int build_helper(prop t, vector<int>::iterator curr, vector<int>::iterator end){
		// printf("%d %d \n", t.type, *curr);
		
		#ifdef DEBUG_MODE
			BOUND(t.type);
		#endif

		if(t.type == -1)
			return 0;
		else if(t.type == -2)
			return 1;
		if(curr == end){
			printf("end cond\n");
			return 0;
		}
		else{
			int v0 = build_helper(t.substitute(*curr,0), curr+1,end);
			int v1 = build_helper(t.substitute(*curr,1), curr+1,end);
			
			#ifdef DEBUG_MODE1
				printf("mk1 %d %d\n",v0, v1);
			#endif
			
			// printf("tt %d %d %d %d\n",t.type, *curr, v0, v1);
			return this->mk(*curr, v0, v1);
		}
	}

	//OP should always be a binary operator
	int apply_helper(int op, int u1, int u2, map<tuple<int,int>,int> &G){
		tuple<int,int> upair (u1,u2);
		tripple look1 = * (t.lookup(u1)), look2 = *(t.lookup(u2));
		int u ;
		if(G.count(upair) != 0)
			return G[upair];
		// else if (u1 <= 1 && u2 <=1) return operate_terminal(op, u1,u2);
		else if(u1 <= 1 || u2 <= 1){
			return u1 * u2;
		}
		else if (get<0>(look1) == get<0>(look2))
			u = this->mk(get<0>(look1), this->apply_helper(op, get<1>(look1), get<1>(look2),G),
				this->apply_helper(op, get<2>(look1), get<2>(look2),G));
		else if(get<0>(look1) < get<0>(look2))
			u = this->mk(get<0>(look1), this->apply_helper(op,get<1>(look1),u2,G),
				this->apply_helper(op,get<2>(look1),u2,G));
		else
			u = this->mk(get<0>(look2), this->apply_helper(op,u1,get<1>(look2),G),
				this->apply_helper(op,u1,get<2>(look2),G));
		G[upair] = u;
		return u;
	}

	bool sort_lessthan(vector<int> &sort, int u1, int u2, bool use_sort){
		if(use_sort){	
			for(int i=0;i<sort.size();i++){
				if(sort[i] == u1)
					return true;
				if(sort[i] == u2)
					return false;
			}
		}
		return (u1 < u2);
	}

	bool anysat_helper(int u, vector<int> &ret){
		tripple look = * t.lookup(u);
		
		#ifdef DEBUG_MODE
			BOUND(get<0>(look))
		#endif

		// printf("%d \n", u);
		if(u == 0)
			return false;
		else if(u ==1)
			return true;
		else{
			if(anysat_helper(get<1>(look),ret)){
				ret.push_back(get<0>(look));
				ret.push_back(0);
			}
			else if(anysat_helper(get<2>(look), ret)){
				ret.push_back(get<0>(look));
				ret.push_back(1);
			}
			else return false; 
			return true;
		}
	}

	int satcount_helper(int u){
		// printf("sh %d\n", u);
		int res = 0;
		if(u == 0)
			res = 0;
		else if(u == 1)
			res = 1;
		else{
			tripple look = *t.lookup(u);
			res = pow(2, get<0>(*t.lookup(get<1>(look))) - 
				get<0>(look) -1 ) * this->satcount_helper(get<1>(look)) +
				pow(2, get<0>(*t.lookup(get<2>(look))) - 
				get<0>(look) -1 ) * this->satcount_helper(get<2>(look)) ;
		}
		return res;
	}

	void print_helper(int u,int h){
		tripple * look = t.lookup(u);
		printf("%d:%d<-(%d,%d,%d) ", u,h,get<0>(*look),get<1>(*look),get<2>(*look));
		if(u >1){
			this->print_helper(get<1>(*look), h+1);
			this->print_helper(get<2>(*look), h+1);
		}
	}



public:
	MKtable(){
		h.insert(tripple(0,0,0), 0);
		h.insert(tripple(1,1,1), 1);
	}

	void print(int u){
		this->print_helper(u,0);
		printf("\n");
	}

	int mk(int var, int low, int high){
		if(low==high)
			return low;
		else{
			tripple curr (var, low, high);
			int u = h.lookup( curr);
			if( u >= 0){
				return u;
			}
			else{
				// printf("zzz\n");
				u = t.add( curr);
				h.insert( curr, u);
				return u;
			}
		}
	}

	int build(prop t){
		vector<int> order = t.sort();
		#ifdef DEBUG_MODE
			printf("order "); 
			for (int i = 0; i < order.size(); ++i)
			{
				printf("%d ", order[i]);
			}
			printf("\n");
		#endif
		return this->build_helper(t,order.begin(),order.end());
	}

	vector<int> build(){
		yyparse();
		vector<int> ret;
		for(vector<prop*>::iterator it = prop_dump.begin(); it != prop_dump.end(); it++){
			int u = this->build(**it);
			ret.push_back(u);
		}
		return ret;
	}

	int apply(int op, int u1, int u2){
		map <tuple<int,int>, int> G;
		return apply_helper(op,u1,u2,G);
	}

	int restrict(int u, int j, int b, vector<int> &sort){
		printf("%d %d %d\n", u,j,b);
		tripple look = (*t.lookup(u));
		if(get<0>(look) == j){
			if(b == 0)
				return this->restrict(get<1>(look),j,b,sort);
			else
				return this->restrict(get<2>(look),j,b,sort);
		}
		if(get<0>(look) == j){
			if(b ==0)
				return this->restrict(get<1>(look), j, b, sort);
			else
				return this->restrict(get<2>(look), j,b, sort);
		}
		else if(this->sort_lessthan(sort,j,get<0>(look), false) ) 
			return u;
		else
			return mk(get<0>(look), this->restrict(get<1>(look), j, b, sort), 
			this->restrict(get<2>(look), j, b, sort));
	}

	int satcount2(int u, int var_count){
		if(u == 0)
			return 0;
		else if(u == 1)
			return pow(2,var_count);
		tripple look = *t.lookup(u);
		return satcount2(get<1>(look), var_count-1) + satcount2(get<2>(look), var_count-1);
	}

	vector<int> anysat(int u){
		vector<int> v;
 		this->anysat_helper(u,v);
 		return v;
	}

};


prop * better_nqueen(int n){
	prop * ret=0;
	if(n<=1) return 0;
	for(int i = 0;i<n;i++){
		int bi1 = i*n + 2;
		vector<prop *> row_cells;
		for(int j=0;j<n;j++){
			prop * cell = new prop(bi1 + j);
			row_cells.push_back(cell);
			int bi2 = j+2;
			
			prop* reachable_cells = 0;
			for (int k = 0; k < n; ++k)
			{
				if(j != k){
					prop* cell_ik = new prop(bi1 + k);
					cell_ik = negation(cell_ik);
					if(reachable_cells)
						reachable_cells = andop(reachable_cells, cell_ik);
					else
						reachable_cells = cell_ik;
				}
				if(i != k){
					prop* cell_kj = new prop(bi2+n*k);
					cell_kj = negation(cell_kj);
					if(reachable_cells)
						reachable_cells = andop(reachable_cells, cell_kj);
					else
						reachable_cells = cell_kj;
				}
			}
			//Diagonal pred slope 1
			int x = i, y = j;
			int inc = 1;	
			// -1<= x <=n && -1<= y <= n
			while(1){
				x += inc; y+= inc;
				if(x == n || y == n){
					x=i; y=j;
					inc = -1;
					continue;
				}
				if(x<0 || y<0){
					break;
				}
				if(x!= i){
					prop * cell_xy = new prop(x*n+y+2);
					reachable_cells = andop(reachable_cells, negation(cell_xy));
				}
			}
			//Diagonal pred slope -1
			x = i; y = j;
			inc = 1;	
			// -1<= x <=n && -1<= y <= n
			while(1){
				x += inc; y-= inc;
				if(x == n || y < 0){
					x=i; y=j;
					inc = -1;
					continue;
				}
				if(x<0 || y == n){
					break;
				}
				if(x!= i){
					prop * cell_xy = new prop(x*n+y+2);
					reachable_cells = andop(reachable_cells, negation(cell_xy));
				}
			}
			// cell => (reachable_cells predicate)
			if(ret)
				ret = andop(ret, implication(cell, reachable_cells));
			else
				ret = implication(cell, reachable_cells);
		}
		ret = andop(ret, disjunction(row_cells));	
	}
	#ifdef DEBUG_MODE
		if(ret->well_formed(n*n+1))
			printf("well_formed\n");
		else
			printf("ill_formed\n");
	#endif
	return ret;
}

void draw_board(int n, vector<int> sat){
	bool board [n][n];
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			board[i][j]= 0;
		}
	}

	for (int i = 0; i < sat.size(); i+=2)
	{
		if(i<2)
			continue;
		if(sat[i+1]){
			int x = (sat[i] -2) / n , y = (sat[i] -2 - x*n);
			
			#ifdef DEBUG_MODE
				if(not(x>=0 && x<n)){
					printf("xxx %d\n", sat[i]);
					continue;
				}
				if(not(y>=0 && y<n)){
					printf("yyy %d\n", sat[i]);
					continue;
				}
			#endif

			board[x][y] = 1;
		}
	}


	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			if(board[i][j])
				printf("X");
			else printf("_");
			printf("|");
		}
		printf("\n");
	}
}

int main(int argc, char** argv){
	#ifdef QUEEN
		if(argc != 2){
			printf("usage <bin> <input1>, where input1:int >=2 \n");
			return 0;
		}
		int board_size = atoi(argv[1]);
		if(board_size <2){
			printf("input1 >=2\n");
			return 0;
		}
		limit = board_size * board_size +2;

		MKtable env;
		prop * queen = better_nqueen(board_size );
		int u = env.build(*queen);
		
		printf("sat_count queen %d\n", env.satcount2(u,board_size*board_size));
		vector <int> sat = env.anysat(u);
		draw_board(board_size, sat);

		#ifdef DEBUG_MODE
			// env.print(u);
			for (int i = 0; i < sat.size(); i+= 2)
			{
				printf("[%d<-%d] ", sat[i],sat[i+1]);
			}
			printf("\n");
		#endif

	#endif

	#if defined(DEBUG_MODE2) || defined(DEBUG_MODE)
		MKtable env;
		prop x (2), y(3), z(4);
		prop* AND = andop(&x , &y);
		prop* NOT = negation(AND);
		prop* AND2 = andop(&y,&z);
		int u1 = env.build(* NOT), u2 = env.build(*AND2);
		int u3 = env.apply(1,u1,u2);
		env.print(u3);
	#endif
	
	// //stdin
	#ifdef STDIN
		MKtable env;
		vector <int> nodes = env.build();
		for(int i = 0; i != nodes.size(); i++){
			printf("ROBDD = \n");
			env.print(nodes[i]);			
			printf("sat_count = %d\n", env.satcount2(nodes[i], prop_dump[i]->sort().size()));
		}
	#endif
}