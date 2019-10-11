#include <vector>
using namespace std;


class prop{
	public:
		int type;
		prop * left, *right;
	prop(){
		left = 0;
		right = 0;		
	}

	prop(int t){
		type = t;
		left = 0;
		right = 0;
	}

	void print(int);
	prop substitute(int id, int with);
	bool well_formed(int);
	vector <int> sort (void);
};

//not x
prop* negation(prop*);
prop* andop(prop*,prop*);
// x->y
prop* implication(prop*, prop*);
// AND s
prop* conjunction(vector <prop*>);
// OR s
prop* disjunction(vector <prop*>);