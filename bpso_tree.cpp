#include<bits/stdc++.h>

using namespace std;

#define pii pair<int,int>
#define x first
#define y second


mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());


double rng_real(){
	return uniform_real_distribution<double>(0,1)(rng);
}

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

const int MAX_TRANS  = 3000;
const int MAX_ITEMS  = 3000;




struct Particle{
	bitset<MAX_TRANS> X;
	int fitness;
	Particle(){

	}
};
vector<Particle> pbest;
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

void generate_pop(){
	for(int i = 0; i < pop_size; i++){
		Particle temp ;
		temp = create();
		
		temp.fitness = fitness_calc(temp);

		pop.push_back(temp);
		pbest.push_back(temp);

		if(pop[i].fitness >= minutil){
			insert(pop[i]);
		}
		if(i == 0){
			gbest = pbest[i];
		}else{
			if(pbest[i].fitness > gbest.fitness)gbest = pbest[i];
		}
		vector<double>vt;
		for(int j = 0; j < items.size(); j++){
			vt.push_back(rng_real());
		}
		V.push_back(vt);
	}	
}

void update_phase(){


	for(int i = 0; i < pop_size; i++){
		double r1 = rng_real();
		double r2 = rng_real();

		for(int j = 0; j < items.size(); j++){
			double temp = V[i][j] + r1*(pbest[i].X[j] - pop[i].X[j]) + r2*(gbest.X[j] - pop[i].X[j]);
			V[i][j] = temp;
			if(V[i][j] < -2.0) V[i][j] = -2.0;
			if(V[i][j] > 2.0) V[i][j] = 2.0;
		}
		pop[i] = particle_update(pop[i],i);
		pop[i].fitness = fitness_calc(pop[i]);
		if(pop[i].fitness >= minutil){
			insert(pop[i]);
		}
		if(pop[i].fitness > pbest[i].fitness){
			pbest[i] = pop[i];
			if(pbest[i].fitness > gbest.fitness){
				gbest = pbest[i];
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
        freopen("3000_large/3000_large.txt","r",stdin);
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
	int itr = 20000;
	int itr2=itr;
	minutil = 100;
	int cnt = 0;
	while(itr--){
		update_phase();
		cnt++;
              	cout << "AFTER "<< cnt <<" ITERATION: " << "HUIS found ";
      		cout << HUI.size() << endl;
	}
                

        freopen("3000_large/output_bpso_tree.txt","w",stdout);
	double total_utility=0;
	int minimum_util=INT_MAX,maximum_util=0;
        print(&total_utility, &minimum_util, &maximum_util);

        freopen("3000_large/stat_bpso_tree.txt","w",stdout);
        double t =  1.0 * clock() / CLOCKS_PER_SEC;
     	cout << "Total No of Transactions: " << totalTransactions <<endl;
	cout << "TIME TAKEN : " << t << endl;
      	cout << "efficiency: "  << HUI.size()/t << endl;
     	cout << "No of Iterations: " << itr2 << endl;
	cout << "Total HUIS found: " << HUI.size() << endl;
	cout << "MinUtility required for HUIM: " << minutil << endl;
	cout << "Minimum utility among HUIs found: " << minimum_util << endl;
	cout << "Maximum Utility among HUIs found: " << maximum_util << endl;
	cout << "Average Utility: " << total_utility/HUI.size() << endl;
 
}
