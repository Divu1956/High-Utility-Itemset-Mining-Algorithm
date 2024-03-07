#include<bits/stdc++.h>

using namespace std;

#define pii pair<int,int>
#define x first
#define y second


mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());


double rng_real(){
	return uniform_real_distribution<double>(0,1)(rng);
}



template<typename T>
class RandomSelection{
	
public:
	vector<T>w; 
	RandomSelection(){}
	RandomSelection(vector<T>a){
		w.resize(a.size());
		T sum = 0;
		sum  = accumulate(a.begin(),a.end(),sum);
		w[0] = a[0] / sum;
		for(int i = 1; i < a.size(); i++){
			w[i] = w[i-1] + a[i]/sum;
		}
	}
	int select(){
		T L = w.front();
		T R = w.back();
		T randnum = uniform_real_distribution<T>(0,1)(rng);
		int j = 0;
		for(auto i:w){
			if(i >= randnum){
				return j;
			}
			j++;
		}
		return j;
	}

};

const int M = 1e7;
const int pop_size = 10;


struct node{
	int item;
	int l;int r;
}t[M];
int temp = 0;
int root = 0;
int minutil;

map<int,int>mp;
vector<vector<pii>>revised_database;
map<int,int>twu;
vector<vector<double>>V;
vector<int>items;


const int MAX_TRANS  = 3400;
const int MAX_ITEMS  = 3000;
const int times = 10;
const int limit = 5;
int max_on = 2;



struct Particle{
	bitset<MAX_TRANS> X;
	int fitness;
	int trail;
	Particle(){

	}
};
vector<Particle> pbest;
vector<Particle>employed,onlooker;
Particle gbest;
vector<Particle> pop;
set<pair<int,vector<int>>>HUI;
set<vector<int>>maximal_pattern;

void maximal(vector<int>pattern){
	if(maximal_pattern.empty()){
		maximal_pattern.insert(pattern);
	}
	else{	bool found = 0;
		for(auto temp_pattern:maximal_pattern){
			int j = 0;
			int k = 0;
			int temp = 0;
			while(j < pattern.size() and k < temp_pattern.size()){
				if(pattern[j] < temp_pattern[k]){
					j++;
				}
				else if(pattern[j] > temp_pattern[k]){
					k++;
				}
				else{
					j++;
					k++;
					temp++;
				}
			}
			if(temp == pattern.size() or temp == temp_pattern.size()){
				found = 1;
				if(pattern.size() > temp_pattern.size()){
					maximal_pattern.erase(temp_pattern);
					maximal_pattern.insert(pattern);
					break;
				}
				else{
					break;
				}
			}
		}
		if(!found){
			maximal_pattern.insert(pattern);
		}
	}
}




int fitness_calc(Particle temp){
	int fit = 0;
	int k = 0;
	for(int i = 0; i < items.size(); i++){
		if(temp.X[i]) k++;
	}

	for(int ptr = 0; ptr < revised_database.size(); ptr++){
		int i = 0;
		int j = 0;
		int q = 0;
		int t = 0;
		int sum = 0;
		while(j < k and q < revised_database[ptr].size() and i < items.size()){
			if(temp.X[i] == 1){
				if(revised_database[ptr][q].x < items[i]){
					q++;
				}
				else if(revised_database[ptr][q].x == items[i]){
					sum = sum + revised_database[ptr][q].y;
					j++;
					q++;
					t++;
					i++;
				}else if(revised_database[ptr][q].x > items[i]){
					j++;
					i++;
				}
			}
			else{
				i++;
			}
		}
		if(t == k){
			fit += sum;
		}
	}
	return fit;
}


void insert(Particle temp){
	vector<int>elements;
	int f = fitness_calc(temp);
	for(int i = 0; i < items.size(); i++){
		if(temp.X[i]){
			elements.push_back(items[i]);
		}
	}
	if(elements.size() == 0)return;
	HUI.insert({f,elements});
}


void change_kth_bit (Particle &tempnode,int num){
	for(int i = 0; i < num; i++){
		vector<int>bits;
		for(int i = 0; i < items.size(); i++){
			if(tempnode.X[i]){
				bits.push_back(i);
			}
		}		
		if(bits.size() < max_on){
			int m = items.size();
			int j = rng()%m;
			tempnode.X[j] = 1 - tempnode.X[j];
		}
		else{
			int j = rng()%(bits.size());
			tempnode.X[bits[j]] = 0;
		}
	}
}


vector<int>check_PBV(Particle temp){
	vector<int>v;
	int k = 0;
	for(int i = 0; i < items.size(); i++){
		if(temp.X[i]) k++;
	}

	for(int ptr = 0; ptr < revised_database.size(); ptr++){
		int i = 0;
		int j = 0;
		int q = 0;
		int t = 0;
		while(j < k and q < revised_database[ptr].size() and i < items.size()){
			if(temp.X[i] == 1){
				if(revised_database[ptr][q].x < items[i]){
					q++;
				}
				else if(revised_database[ptr][q].x == items[i]){
					j++;
					q++;
					t++;
					i++;
				}else if(revised_database[ptr][q].x > items[i]){
					j++;
					i++;
				}
			}
			else{
				i++;
			}
		}
		if(t == k){
			v.push_back(ptr);
		}
	}
	return v;	
}

void dance(Particle &bestnode){
	Particle tempnode = bestnode;
	int j = 0;
	do{
		vector<int>templist;
		int t = 0;
		while(true){
			t++;
			change_kth_bit(tempnode,1);
			templist = check_PBV(tempnode);

			if(templist.size() > 0)break;
		}

		tempnode.fitness = fitness_calc(tempnode);

		if(tempnode.fitness > bestnode.fitness){
			bestnode = tempnode;
		}
		else{
			tempnode = bestnode;
		}
		j++;
	}while(j < times and bestnode.fitness < minutil);
	
	if(bestnode.fitness >= minutil){
		insert(bestnode);
	}

}

Particle create(){
	int cur = root;
	Particle gen;
	int j = 0;
	for(auto item:items){
		if(t[cur].l == 0){
			gen.X[j] = 0;
			cur = t[cur].r;
		}else if(t[cur].r == 0){
			double t1 = rng_real();
			if(t1 > 0.5){
				gen.X[j] = 1;
				cur = t[cur].l;
			}
			else {
				gen.X[j] = 0;
				cur = t[cur].l;
			}
		}else{
			double temp = rng_real();
			if(temp > 0.5){
				double t1 = rng_real();
				if(t1 > 0.5){
					gen.X[j] = 1;
					cur = t[cur].l;
				}
				else {
					gen.X[j] = 0;
					cur = t[cur].l;
				}
			}else{
				gen.X[j] = 0;
				cur = t[cur].r;
			}
		}
		j++;
	}
	return gen;
}

Particle particle_update(Particle part,int i){
	int cur = root;
	int j = 0;
	for(auto item:items){
		if(t[cur].l == 0){
			part.X[j] = 0;
			cur = t[cur].r;
		}else if(t[cur].r == 0){
			double t1 = rng_real();
			double t2 = 1 / (1 + exp(-V[i][j]));
			if(t1 < t2){
				part.X[j] = 1;
				cur = t[cur].l;
			}
			else {
				part.X[j] = 0;
				cur = t[cur].l;
			}
		}else{
			double temp = rng_real();
			if(temp > 0.5){
				double t1 = rng_real();
				double t2 = 1.0/(1 + exp(-V[i][j]));
				if(t1 < t2){
					part.X[j] = 1;
					cur = t[cur].l;
				}
				else {
					part.X[j] = 0;
					cur = t[cur].l;
				}
			}else{
				part.X[j] = 0;

				cur = t[cur].r;
			}
		}
		j++;
	}
	return part;
}


void construct(){
	root = ++temp;
	t[root].item = items[0];
	for(auto pattern:maximal_pattern){
		int cur = root;
		int j = 0;
		int k = 0;
		while(k < items.size()){
			if(j < pattern.size()){
				if(pattern[j] > items[k]){
					if(t[cur].r == 0){
						int new_node = ++temp;
						if(k + 1 < items.size()) t[new_node].item = items[k+1];
						else t[new_node].item = -1;
						t[cur].r = new_node;
					}
					k++;
					cur = t[cur].r;
				}else{
					if(t[cur].l == 0){
						int new_node = ++temp;
						if(k + 1 < items.size())t[new_node].item = items[k+1];
						else t[new_node].item = -1;
						t[cur].l = new_node;
					}
					k++;
					j++;
					cur = t[cur].l;
				}
			}else{
				if(t[cur].r == 0){
					int new_node = ++temp;
					if(k + 1 < items.size()) t[new_node].item = items[k+1];
					else t[new_node].item = -1;
					t[cur].r = new_node;
				}
				cur = t[cur].r;
				k++;
			}
		}
	}
}


void init(vector<vector<pii>>database){

	for(auto i: database){
		int s = 0;
		for(auto j:i) s += j.y;
		for(auto j:i) mp[j.x] += s;	
	}

	
	for(auto i:database){
		vector<pii>tr;
		vector<int>pattern;
		for(auto j:i){
			if(mp[j.x] >= minutil){
				tr.push_back(j);
				twu[j.x] = mp[j.x];
				pattern.push_back(j.x);
			}

		}
		if(tr.size() > 0){
			sort(pattern.begin(),pattern.end());
			maximal(pattern);
			revised_database.push_back(tr);
		}
	}	
	for(auto j:twu){
		items.push_back(j.x);
	}


}


void generate_pop(){
	for(int i = 0; i < pop_size; i++){
		Particle temp ;
		temp = create();
		
		temp.fitness = fitness_calc(temp);

		pop.push_back(temp);

		if(pop[i].fitness >= minutil){
			insert(pop[i]);
		}
		/**if(i == 0){
			gbest = pbest[i];
		}else{
			if(pbest[i].fitness > gbest.fitness)gbest = pbest[i];
		}
		vector<double>vt;
		for(int j = 0; j < items.size(); j++){
			vt.push_back(rng_real());
		}
		V.push_back(vt);**/
	}	
}


void employed_bee(){
	employed = pop;
	for(int i = 0; i < pop_size; i++){
		dance(employed[i]);
		if(employed[i].fitness > pop[i].fitness){
			pop[i] = employed[i];
		}
		else{
			pop[i].trail++;
		}
	}
}


void onlooker_bee(){
	vector<double>select(pop_size);

	for(int i = 0; i < pop_size; i++){
		select[i] = pop[i].fitness;
	}
	RandomSelection<double> rs(select);

	onlooker = pop;

	for(int i = 0; i < pop_size; i++){
		int j = rs.select();
		onlooker[i] = pop[j];
		dance(onlooker[i]);
		if(onlooker[i].fitness > pop[i].fitness){
			pop[i] = onlooker[i];
		}
		else{
			pop[i].trail++;
		}
	}

}
void scout_bee(){

	for(int i = 0; i < pop_size; i++){
		if(pop[i].trail > limit){
			int j = 0;
			Particle bestnode;
			do{
				Particle temp = create();
				if(temp.fitness > bestnode.fitness){
					bestnode = temp;
				}
				j++;

			}while(j < times and bestnode.fitness < minutil);
			pop[i] = bestnode;
			if(pop[i].fitness >= minutil){
				insert(pop[i]);
			}
		}
	}

}



void print(double *total_utility, int *mn, int *mx){
        int cnt = 0;
        for(auto i:HUI){
        	*total_utility+=i.first;
        	*mx=max(*mx,i.first);
        	*mn=min(*mn,i.first);
               cnt++;
               cout << "HUI #:"  << cnt << endl;
               cout << "Utility value: " << i.first <<endl;
               cout << "Itemset: ";
               for(auto j:i.second) cout << j << " ";
                 	cout << endl;
        }
}



double total = 0;
vector<vector<pair<int,int>>> input(int *totalTransactions){
        freopen("3000_small/3000_small.txt","r",stdin);
        vector<vector<pair<int,int>>>database;
        while(!cin.eof()){
        		(*totalTransactions)++;
                        string s;
                        getline(cin,s);
                        vector<int>item;
                        vector<double>cost;
                        int cnt = 0;
                        int num = 0;
                        for(auto i:s){
                                if(i==':'){
                                        if(cnt==0){
                                                item.push_back(num);
                                                num=0;
                                        }
                                        cnt++;continue;
                                }       
                                if(cnt==1){
                                        continue;
                                }
                                if(cnt==0){
                                        if(i==' ')item.push_back(num),num=0;
                                        else num = num*10 + i-48;
                                }
                                else{
                                        if(i==' ' || i==13)cost.push_back(num),num=0;
                                        else num=num*10+i-48;
                                }
                        }
                        cost.push_back(num);    
                        vector<pair<int,int>>tran;
                        for(int i=0;i<item.size();i++){
                                total += cost[i];
                                tran.push_back({item[i],cost[i]});
                        }
                        database.push_back(tran);
                        
        }
        return database;
}



int main(){

	int totalTransactions = 0;
	vector<vector<pii>>database =  input(&totalTransactions);
	init(database);
	construct();
	generate_pop();
	int itr = 2000;
	minutil = 1000;
	int cnt=0;
	while(itr--){
		employed_bee();
		onlooker_bee();
		scout_bee();
		cnt++;
		cout << "AFTER "<< cnt <<" ITERATION: " << "HUIS found ";
		cout << HUI.size() << endl;
	}
	
	freopen("3000_small/output_abc_tree.txt","w",stdout);
	double total_utility=0;
	int minimum_util=INT_MAX,maximum_util=0;
	print(&total_utility, &minimum_util, &maximum_util);

	freopen("3000_small/stat_abc_tree.txt","w",stdout);

        double t =  1.0 * clock() / CLOCKS_PER_SEC;
     	cout << "Total No of Transactions: " << totalTransactions <<endl;
	cout << "TIME TAKEN : " << t << endl;
	cout << "efficiency: "  << HUI.size()/t << endl;
	cout << "No of Iterations: " << cnt << endl;
	cout << "Total HUIS found: " << HUI.size() << endl;
	cout << "MinUtility required for HUIM: " << minutil << endl;
	cout << "Minimum utility among HUIs found: " << minimum_util << endl;
	cout << "Maximum Utility among HUIs found: " << maximum_util << endl;
	cout << "Average Utility: " << total_utility/HUI.size() << endl;
}

