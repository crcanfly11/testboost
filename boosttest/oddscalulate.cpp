#include "oddscalulate.h"

fixtures_base_odds::fixtures_base_odds(double win_odds, double shake_hands_odds, double lose_odds, const char* home_team_name, const char* away_team_name)
{
	team_odds_[home_team_win] = win_odds;
	team_odds_[shake_hands] = shake_hands_odds;
	team_odds_[away_team_win] = lose_odds;
        
	strcpy_s(home_team_, home_team_name);
	strcpy_s(away_team_, away_team_name);
};
    
double fixtures_base_odds::operator[] (int index) 
{
	if(index < max_odds_type) 
		return team_odds_[index];
	else 
		return -1;
};

//-----------------------------------------------------------------------

forecas_result::forecas_result(double odds, const char* result_msg) : 
	odds_(odds), multiple_(0), income_(0), yield_(0), total_cost_(0), net_income_(0), flag_(0x00)
{
	strcpy_s(result_msg_, sizeof(result_msg_), result_msg);
	//yield_ = (odds - 1)*100;
};

forecas_result::forecas_result(const forecas_result& result)
{
	odds_ = result.odds_;
	multiple_ = result.multiple_;
	income_ = result.income_;
	yield_ = result.yield_;
	total_cost_ = result.total_cost_;
	net_income_ = result.net_income_;
	flag_ = result.flag_;

	strcpy_s(result_msg_, sizeof(result_msg_), result.result_msg_);
};

void forecas_result::set_result_multiple(unsigned short multiple)
{ 
	multiple_ = multiple; 

	if(multiple_) income_ = lottery*multiple_*odds_;
};

void forecas_result::set_total_cost(double tcost)
{
	if(tcost == 0 || multiple_ == 0) return;

	total_cost_ = tcost;
	net_income_ = income_ - total_cost_;
	yield_ = (net_income_/total_cost_)*100;  
};

void forecas_result::clear_dynamic_data()
{
	multiple_ = 0;	
	total_cost_ = 0;
	net_income_ = 0;
	yield_ = 0;  
	income_ = 0;
};

//-----------------------------------------------------------------------

organizer::organizer() : htwin_(0), sh_(0), atwin_(0), flag_(0)
{
    memset(htname_, 0, sizeof(htname_));
    memset(atname_, 0, sizeof(atname_));
	memset(tmp_, 0, sizeof(tmp_));
	memset(result_, 0, sizeof(result_));
        
    init();
};

void organizer::init()
{
	char s1[20], s2[20], s3[20];

    while(1) {     
		cout<< "add new fixtures odds?(Y:yes/others:no)"<<endl;
		cin>> tmp_;
		cin.clear();
		
		if(!strcmp(tmp_, "Y") || !strcmp(tmp_, "y")) {
            cout<< "input fixtures odds and team names."<< endl;
			cin>> skipws>> s1>> s2>> s3;//>> htname_>> atname_;
			htwin_ = atof(s1);
			sh_ = atof(s2);
			atwin_ = atof(s3);
			cin.clear();

            fixtures_base_odds* pbo = new fixtures_base_odds(htwin_, sh_, atwin_, htname_, atname_);
            base_odds_.push_back(*pbo);
        }
		else
			break;
    }

	if(base_odds_.size() < 2) return;
    
    forecas_calculate(base_odds_.begin(),base_odds_.end());

	if(check_odds() != 0) 
		cout<< "have zero in the odds."<< endl;

	cout<< "every case of results."<< endl;
	for_each(forecas_results_.begin(), forecas_results_.end(), 
		boost::bind(&organizer::print, this, _1));

	//cout<< "How to buy? all in without loss or buy Specify combination."<< endl;
	
	position_ = new position(this);
	regulator_ = new regulator(this);

	//cout<< "Select mode 1, portion_mode; 2, whole_mode;"<< endl;
	//char smode[10];
	//cin>> skipws>> smode;
	//play_mode mode;
	//if(atoi(smode) == 1) {
	//	mode = portion_mode;

	//	cout<< "Weed out results is HL,AL"<< endl;
	//}
	//else if(atoi(smode) == 2) {
	//	mode = whole_mode;
	//}

	cout<< "results of all case."<< endl;
	play_mode mode = whole_mode;   //whole_mode portion_mode
	regulator_->set_play_mode(mode);
	print();
	
	cout<< "results that weed out  HW and AW"<< endl;
	mode = portion_mode;   //whole_mode portion_mode
	regulator_->set_play_mode(mode);
	print();
	//Individually adjusted

	//regulator_->set_adjusted_min_income(3);
	//regulator_->set_adjusted_min_yield(50);

	//cout<< "-----------------------------------------"<< endl;
	//for_each(forecas_results_.begin(), forecas_results_.end(), 
	//	boost::bind(&organizer::print_result, this, _1));

	optimization_result_ = new optimization_result(this, forecas_results_);
	optimization_result_->optimization();

	while (true)
	{
		char idx[4];
        cout<< "input Adjustment result multiple index."<< endl;
		cin>> skipws>> idx;
		cin.clear();

		if(!position_->add_someone_position(atoi(idx))) {
			cout<< "input wrong index."<< endl;
		}
		
		position_->refresh();

		print();
	}
};

void organizer::print(forecas_result_pair rpair)
{
	std::cout<< rpair.first<< ". "
		<< rpair.second.get_result_msg()<< ": "
		<< rpair.second.get_result_odds()
		<< endl;
		//<< " flag:"
		//<<  hex<< rpair.second.get_flag()
};

void organizer::print()
{
	cout<< "-------------------------------------------------------------------"<< std::endl;
	cout<<left <<setw(4) <<"ID" <<right <<setw(6) << "Result"
		<<right <<setw(8) <<"Odds" <<right <<setw(10) << "Multiple"
		<<right <<setw(12) <<"Netincome" <<right <<setw(11) << "TotalCost"
		<<right <<setw(8) << "Yield"<<"%"<< endl;

	forecas_result_map::iterator rpair = get_result_map()->begin();
	for(rpair;rpair  != get_result_map()->end();++rpair ) {
		if( rpair->second.get_result_multiple() == 0) 
			continue;

		cout.setf(ios::fixed);
		cout<< left<< setw(4)<< setprecision(0)<< rpair->first
			<< right <<setw(6)<< rpair->second.get_result_msg()
			<< right <<setw(8)<< setprecision(2)<< rpair->second.get_result_odds()
			<< right <<setw(10)<< rpair->second.get_result_multiple()
			<< right <<setw(12)<< setprecision(2)<< rpair->second.get_net_income()
			<< right <<setw(11)<< setprecision(0)<<rpair->second.get_total_cost()
			<< right <<setw(8)<< setprecision(2)<< rpair->second.get_result_yield()<< endl;
		//std::cout<< rpair->first<< ". "<< rpair->second.get_result_msg()<< ": "<< rpair->second.get_result_odds()<< 
		//" multiple:"<< rpair->second.get_result_multiple()<< " net income:"<< rpair->second.get_net_income()<<
		//" total cost:"<< rpair->second.get_total_cost()<< " yield:"<< rpair->second.get_result_yield()<< endl;
	}	
};

void organizer::print_result(forecas_result_pair rpair)
{
	if(!rpair.second.get_result_multiple()) return;

	std::cout<< rpair.first<< ". "<< rpair.second.get_result_msg()<< ": "<< rpair.second.get_result_odds()<< 
		" multiple:"<< rpair.second.get_result_multiple()<< " net income:"<< rpair.second.get_net_income()<<
		" total cost:"<< rpair.second.get_total_cost()<< " yield:"<< rpair.second.get_result_yield()<< endl;
};

int organizer::check_odds()
{
	forecas_result_map::iterator iter_check = forecas_results_.begin();
	for(iter_check;iter_check != forecas_results_.end();++iter_check) {
		if(iter_check->second.get_result_odds() == 1)
			return -1;
	}
	return 0;	
};

void organizer::forecas_calculate(base_odds_vector::iterator begin, base_odds_vector::iterator end)
{
	if(begin == end) return;

	base_odds_vector::iterator second = begin + 1;    
    for_each(second, end, 
		boost::bind(&organizer::set_forecas_result_map, this, *begin, _1));
    
    return forecas_calculate(second, end);
};

void organizer::set_forecas_result_map(fixtures_base_odds first, fixtures_base_odds second)
{
	unsigned int num=0;
	for(int i=0; i< max_odds_type; ++i) {
		for(int j=0; j< max_odds_type; ++j) {
			double odd = (first[i])*(second[j]);
			result_msg(i, j);
			forecas_result* frt = new forecas_result(((first[i])*(second[j])), result_);
			frt->set_flag(flag_);

			forecas_results_.insert(forecas_result_pair(++num, *frt));
		}
	}
};

void organizer::result_msg(int first, int second)
{
	memset(result_, 0, sizeof(result_));
	strcat_s(result_,sizeof(result_), msg_type(first));
	strcat_s(result_,sizeof(result_), msg_type(second));

	flag_ = 0x00; 
	flag_type(first);
	flag_type(second);
};

const char* organizer::msg_type(int index) 
{
	switch (index)
	{
	case 0:
		return "HW";
	case 1:
		return "SH";
	case 2:
		return "AW";
	default:
		return "N";
	}
};

short organizer::flag_type(int index)
{
	switch (index)
	{
	case 0:
		return flag_ |= host_win;
	case 1:
		return flag_ |= shake_hand;
	case 2:
		return flag_ |= away_win;
	default:
		return 0;
	}
};

//-----------------------------------------------------------------------

position::position(organizer* org) : 
	cost_(0), organizer_(org)
{
	//refresh();
};

void position::refresh() 
{
	cost_ = 0;

	memset(earnings_range_, 0, sizeof(earnings_range_));
	strcpy_s(earnings_range_, "The earnings range:");

	//for_each(organizer_->get_result_map()->begin(), organizer_->get_result_map()->end(), 
	//	boost::bind(&position::total_cost, this, _1));

	forecas_result_map::iterator iter_total_cost = organizer_->get_result_map()->begin();
	for(iter_total_cost;iter_total_cost != organizer_->get_result_map()->end();++iter_total_cost) {
		if(iter_total_cost->second.get_result_multiple() == 0) 
			continue;

		cost_ += iter_total_cost->second.get_result_multiple() * lottery;
	}

	//for_each(organizer_->get_result_map()->begin(), organizer_->get_result_map()->end(), 
	//	boost::bind(&position::set_result_cost, this, _1));

	forecas_result_map::iterator iter_set_result_cost = organizer_->get_result_map()->begin();
	for(iter_set_result_cost;iter_set_result_cost != organizer_->get_result_map()->end();++iter_set_result_cost) {
		iter_set_result_cost->second.set_total_cost(cost_);
	}

	//organizer_->print();

	//boost::bind Ç¶Ì×
	//for_each(organizer_->get_result_map().begin(), organizer_->get_result_map().end(), 
	//	boost::bind(&forecas_result::set_total_cost, this, 
	//		boost::bind(&forecas_result_pair::second, _1)));
};

void position::total_cost(forecas_result_pair rpair)
{
	if(rpair.second.get_result_multiple() == 0) return;

	cost_ += rpair.second.get_result_multiple() * lottery;
};

void position::set_result_cost(forecas_result_pair rpair)
{
	rpair.second.set_total_cost(cost_);
};

int position::add_someone_position(unsigned int index)
{
   forecas_result_map::iterator iter = organizer_->get_result_map()->find(index);

   if(iter == organizer_->get_result_map()->end()) 
	   return 0;

   iter->second.set_result_multiple(iter->second.get_result_multiple()+1);

   refresh();	

   return 1;
};

//-----------------------------------------------------------------------


optimization_result::optimization_result(organizer* org, forecas_result_map& optimization_result) : 
	max_min_yield_(0), max_yield_(0), organizer_(org), size_(0)
{
	memset(&forecas_results_, 0, sizeof(forecas_results_));
	memset(&optimization_results_, 0, sizeof(optimization_results_));

	memcpy(&forecas_results_, &optimization_result, sizeof(optimization_result));
	size_ = organizer_->get_position()->get_real_size();
};

void optimization_result::optimization()
{
	int min_idx = get_result_min_idx();

	forecas_result_map::iterator iter_min = forecas_results_.find(min_idx);
	max_min_yield_ = iter_min->second.get_result_yield();

	print();

	int cnt=0;
	while (true)
	{
		add_someone_position(min_idx);
		min_idx = get_result_min_idx();

		iter_min = forecas_results_.find(min_idx);
		double yield = iter_min->second.get_result_yield();

		print();

		if(yield > max_min_yield_) {
			max_min_yield_ = yield;

			forecas_result_map forecas_result;
			memcpy(&forecas_result, &forecas_results_, sizeof(forecas_results_));
			//insert can`t working
			optimization_results_.insert(optimization_result_pair(++cnt, forecas_result));
		}
		
		if(organizer_->get_position()->get_cost() > max_total_cost)
			break;
	}

	print_result();
};

void optimization_result::print_result()
{
	optimization_result_map::iterator opair = optimization_results_.begin();
	for(opair;opair != optimization_results_.end();++opair ) {
		cout<< "-------------------------------------------------------------------"<< std::endl;
		cout<< setprecision(0)<< opair->first<< ". optimal combination:";

		forecas_result_map::iterator iter_result = opair->second.begin();
		for(iter_result;iter_result!=opair->second.end();++iter_result) {
			cout<< " "<<iter_result->second.get_result_multiple();
		}

		cout<< " max min yeild:"<< max_min_yield_<< endl;
	}	
};

struct com_data{ double odd; unsigned int index; };
bool less_odd(const com_data & m1, const com_data & m2) {
        return m1.odd < m2.odd;
}

unsigned int optimization_result::get_result_min_idx()
{
	vector<com_data> min_index;	
	forecas_result_map::iterator iter_opt = forecas_results_.begin();
	for(iter_opt;iter_opt != forecas_results_.end();++iter_opt) {
		if(iter_opt->second.get_result_multiple() == 0)
			continue;
		
		com_data* cd = new com_data;
		cd->odd = iter_opt->second.get_net_income()/lottery;
		cd->index = iter_opt->first;
		min_index.push_back(*cd);
	}

	sort(min_index.begin(), min_index.end(), less_odd);

	return min_index.begin()->index;
};

void optimization_result::print()
{

	cout<< "-------------------------------------------------------------------"<< std::endl;
	cout<<left <<setw(4) <<"ID" <<right <<setw(6) << "Result"
		<<right <<setw(8) <<"Odds" <<right <<setw(10) << "Multiple"
		<<right <<setw(12) <<"Netincome" <<right <<setw(11) << "TotalCost"
		<<right <<setw(8) << "Yield"<<"%"<< endl;

	forecas_result_map::iterator rpair = forecas_results_.begin();
	for(rpair;rpair  != forecas_results_.end();++rpair ) {
		if( rpair->second.get_result_multiple() == 0) 
			continue;

		cout.setf(ios::fixed);
		cout<< left<< setw(4)<< setprecision(0)<< rpair->first
			<< right <<setw(6)<< rpair->second.get_result_msg()
			<< right <<setw(8)<< setprecision(2)<< rpair->second.get_result_odds()
			<< right <<setw(10)<< rpair->second.get_result_multiple()
			<< right <<setw(12)<< setprecision(2)<< rpair->second.get_net_income()
			<< right <<setw(11)<< setprecision(0)<<rpair->second.get_total_cost()
			<< right <<setw(8)<< setprecision(2)<< rpair->second.get_result_yield()<< endl;
	}
};

void optimization_result::get_result_info()
{

};

int optimization_result::add_someone_position(unsigned int index)
{
   forecas_result_map::iterator iter = forecas_results_.find(index);
   if(iter == forecas_results_.end()) 
	   return 0;

   iter->second.set_result_multiple(iter->second.get_result_multiple()+1);

   organizer_->get_position()->refresh();
   
   return 1;
};

//-----------------------------------------------------------------------

regulator::regulator(organizer* org) : 
	adjusted_income_(0), adjusted_yield_(0), organizer_(org), mode_(none_modem)
{	
};

void regulator::hedge_positions()
{
	int map_size = organizer_->get_position()->get_real_size();
	while(true)	{
		int break_cnt = 0;
		forecas_result_map::iterator iter = organizer_->get_result_map()->begin();		
		for(iter; iter != organizer_->get_result_map()->end(); ++iter) {
			if(iter->second.get_result_multiple() > 0) {
				int cost_cnt = (int)(organizer_->get_position()->get_cost()/2);
				int multiple = iter->second.get_result_multiple();
				if(iter->second.get_result_odds()*multiple - cost_cnt < 0) {
					double odd = iter->second.get_result_odds();
					int add_mul = (int)((cost_cnt-odd*multiple)/(odd-1));
					iter->second.set_result_multiple(multiple+(add_mul?add_mul:1));					
					organizer_->get_position()->refresh();
					break;
				}			
				else {
					break_cnt++;	
				}
			}
		}
		
		if(break_cnt == map_size) 
			break;

		if(max_total_cost < organizer_->get_position()->get_cost()) 
			break;
	}
};

void regulator::set_adjusted_min_income(double min_income)
{
	adjusted_income_ = min_income;
	adjusted_yield_ = 0;

	int map_size = organizer_->get_position()->get_real_size();
	while(true) {
		int break_cnt = 0;
		forecas_result_map::iterator iter = organizer_->get_result_map()->begin();
		for(iter; iter != organizer_->get_result_map()->end(); ++iter) {
			if(iter->second.get_result_multiple() > 0) {
				if(iter->second.get_net_income() < adjusted_income_) {
					double odd = iter->second.get_result_odds();
					int multiple = iter->second.get_result_multiple();
					//calculate the add_multiple that biger value
					int add_multiple = (int)((organizer_->get_position()->get_cost()-lottery*odd*multiple+adjusted_income_)/(lottery*(odd-1))) + 1; 
					iter->second.set_result_multiple(multiple+add_multiple);					
					organizer_->get_position()->refresh();
					break;
				}			
				else {
					break_cnt++;	
				}
			}
		}
		
		if(break_cnt == map_size) break;

		if(max_total_cost < organizer_->get_position()->get_cost()) break;
	}
};

void regulator::set_adjusted_min_yield(double min_yield)
{
	adjusted_yield_ = min_yield;
	adjusted_income_ = 0;
	
	int map_size = organizer_->get_position()->get_real_size();
	while(true) {
		int break_cnt = 0;
		forecas_result_map::iterator iter = organizer_->get_result_map()->begin();
		for(iter; iter != organizer_->get_result_map()->end(); ++iter) {
			if(iter->second.get_result_multiple() > 0) {
				if(iter->second.get_result_yield() < adjusted_yield_) {
					double odd = iter->second.get_result_odds();
					int multiple = iter->second.get_result_multiple();
					//calculate the add_multiple that biger value
					int add_multiple = (int)(((organizer_->get_position()->get_cost()/lottery)*(adjusted_yield_/100+1)-odd*multiple)/(odd-adjusted_yield_/100-1)) + 1; 
					iter->second.set_result_multiple(multiple+add_multiple);					
					organizer_->get_position()->refresh();
					break;
				}			
				else {
					break_cnt++;	
				}
			}
		}
		
		if(break_cnt == map_size) break;

		if(max_total_cost < organizer_->get_position()->get_cost()) break;
	}
};

void regulator::adjust_positions(double value)
{
	int real_size=0;
	forecas_result_map::iterator iter_size = organizer_->get_result_map()->begin();
	for(iter_size;iter_size != organizer_->get_result_map()->end();++iter_size ) {
		if(iter_size->second.get_result_multiple() > 0)
			++real_size;
	}

	int map_size = real_size;
	while(true) {
		int break_cnt = 0;
		forecas_result_map::iterator iter = organizer_->get_result_map()->begin();
		for(iter; iter != organizer_->get_result_map()->end(); ++iter) {
			if(iter->second.get_result_multiple() > 0) {
				if(iter->second.get_net_income() < adjusted_yield_) {
					double odd = iter->second.get_result_odds();
					int multiple = iter->second.get_result_multiple();
					//calculate the add_multiple that biger value
					int add_multiple;
					if(adjusted_income_ > 0) 
						add_multiple = (int)((organizer_->get_position()->get_cost()-lottery*odd*multiple+value)/(lottery*(odd-1))) + 1; 
					else if(adjusted_yield_ > 0) {
						add_multiple = (int)(((organizer_->get_position()->get_cost()/lottery+1)*value-odd*multiple)/(odd-value-1)) + 1; 
					}
					iter->second.set_result_multiple(multiple+add_multiple);					
					organizer_->get_position()->refresh();
					break;
				}			
				else {
					break_cnt++;	
				}
			}
		}
		
		if(break_cnt == map_size) break;

		if(max_total_cost < organizer_->get_position()->get_cost()) break;
	}
}

void regulator::set_play_mode(play_mode mode)
{
	//for_each(organizer_->get_result_map()->begin(), organizer_->get_result_map()->end(), 
	//	boost::bind(&regulator::init_results, this, _1));
	forecas_result_map::iterator iter_clear = organizer_->get_result_map()->begin();
	for(iter_clear;iter_clear != organizer_->get_result_map()->end();++iter_clear){
		iter_clear->second.clear_dynamic_data();
	}
 
	mode_ = mode;
	init_position();
};

void regulator::init_position()
{
	if(mode_ == whole_mode) {
		//for_each(organizer_->get_result_map()->begin(), organizer_->get_result_map()->end(), 
		//	boost::bind(&regulator::add_all_position, this, _1));		

		forecas_result_map::iterator iter_setmul = organizer_->get_result_map()->begin();
		for(iter_setmul;iter_setmul  != organizer_->get_result_map()->end();++iter_setmul ) {
			iter_setmul->second.set_result_multiple(1);
		}
	}
	else if(mode_ == portion_mode) {
		forecas_result_map::iterator iter_setmul = organizer_->get_result_map()->begin();
		for(iter_setmul;iter_setmul  != organizer_->get_result_map()->end();++iter_setmul ) {
			if(iter_setmul->second.get_flag()&away_win) 
				continue;

			iter_setmul->second.set_result_multiple(1);
		}		
	}
	else if(mode_ == manual_mode)
	{
	
	}

	int real_size=0;
	forecas_result_map::iterator iter_size = organizer_->get_result_map()->begin();
	for(iter_size;iter_size != organizer_->get_result_map()->end();++iter_size ) {
		if(iter_size->second.get_result_multiple() > 0)
			++real_size;
	}

	organizer_->get_position()->set_real_size(real_size);

	organizer_->get_position()->refresh();

	hedge_positions();

	//Automatic optimization data

	//organizer_->print();
};

void regulator::add_all_position(forecas_result_pair& rpair)
{
	rpair.second.set_result_multiple(1);
};

void regulator::init_results(forecas_result_pair rpair)
{
	rpair.second.clear_dynamic_data();
};

//-----------------------------------------------------------------------