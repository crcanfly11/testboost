#include "oddscalulate.h"

fixtures_base_odds::fixtures_base_odds(double win_odds, double shake_hands_odds, double lose_odds, const char* home_team_name, const char* away_team_name)
{
	team_odds_[home_team_win] = win_odds;
	team_odds_[shake_hands] = shake_hands_odds;
	team_odds_[away_team_win] = lose_odds;
        
	strncpy(home_team_, home_team_name, sizeof(home_team_));
	strncpy(away_team_, away_team_name, sizeof(away_team_));
};
 
double fixtures_base_odds::operator[] (int index) 
{
	if(index < max_odds_type) 
		return team_odds_[index];
	else 
		return -1;
};

//-----------------------------------------------------------------------

forecas_result::forecas_result(double odds, double probability, const char* result_msg) : 
	odds_(odds), multiple_(0), income_(0), yield_(0), total_cost_(0), net_income_(0), flag_(0x00), probability_(probability)
{
	strncpy(result_msg_, result_msg, sizeof(result_msg_));
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
	probability_ = result.probability_;

	strncpy(result_msg_, result.result_msg_, sizeof(result_msg_));
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

organizer::organizer() : htwin_(0), sh_(0), atwin_(0), flag_(0), index_(0)
{
    memset(htname_, 0, sizeof(htname_));
    memset(atname_, 0, sizeof(atname_));
	memset(tmp_, 0, sizeof(tmp_));
	memset(result_, 0, sizeof(result_));
        
    //init();
};

organizer::~organizer()
{
	clear();

	delete optimization_result_; optimization_result_ = NULL;
	delete regulator_; regulator_ = NULL;
	delete position_; position_ = NULL;
};

void organizer::init()
{
	char s1[10], s2[10], s3[10], s4[10] , s5[10], s6[10];

    //while(1) {     
		//cout<< "add new fixtures odds?(Y:yes/others:no)"<<endl;
		//cin>> tmp_;
		//cin.clear();
		//
		//if(!strcmp(tmp_, "Y") || !strcmp(tmp_, "y")) {
        //  cout<< "input fixtures odds and team names."<< endl;
		//	cin>> skipws>> s1>> s2>> s3;//>> htname_>> atname_;
		//	htwin_ = atof(s1);
		//	sh_ = atof(s2);
		//	atwin_ = atof(s3);
		//	cin.clear();
  //
  //          fixtures_base_odds* pbo = new fixtures_base_odds(htwin_, sh_, atwin_, htname_, atname_);
  //          base_odds_.push_back(*pbo);
  //      }
		//else
		//	break;
  //  }
	//
	//if(base_odds_.size() < 2) return;

	cout<< "input odds. for example 1.1 2 3 1.6 3 4"<< endl;
	cin>> skipws>> s1>> s2>> s3>> s4>> s5>> s6;//>> htname_>> atname_;

	htwin_ = atof(s1);
	sh_ = atof(s2);
	atwin_ = atof(s3);

	fixtures_base_odds pbo1(htwin_, sh_, atwin_, htname_, atname_);
    base_odds_.push_back(pbo1);	

	htwin_ = atof(s4);
	sh_ = atof(s5);
	atwin_ = atof(s6);

	fixtures_base_odds pbo2(htwin_, sh_, atwin_, htname_, atname_);
    base_odds_.push_back(pbo2);	
    
	//generate base result data
    forecas_calculate(base_odds_.begin(),base_odds_.end());

	if(check_odds() != 0) 
		cout<< "don`t have zero in the odds."<< endl;

	//cout<< "every case of results."<< endl;
	//for_each(forecas_results_.begin(), forecas_results_.end(), 
	//	boost::bind(&organizer::print, this, _1));
	
	position_ = new position(this);
	regulator_ = new regulator(this);

	//cout<< "results of all case."<< endl;
	//play_mode mode = whole_mode;   //whole_mode portion_mode
	//regulator_->set_play_mode(mode);
	//print();
	
	//cout<< "results that weed out  HW and AW"<< endl;
	play_mode mode = portion_mode;   //whole_mode portion_mode
	regulator_->set_play_mode(mode);
	//print();
	
	//regulator_->set_adjusted_min_income(3);
	//regulator_->set_adjusted_min_yield(50);

	optimization_result_ = new optimization_result(this);
	optimization_result_->optimization();

	////test
	//while (true)
	//{
	//	char idx[4];
 //       cout<< "input Adjustment result multiple index."<< endl;
	//	cin>> skipws>> idx;
	//	cin.clear();

	//	if(!position_->add_someone_position(atoi(idx))) {
	//		cout<< "input wrong index."<< endl;
	//	}
	//	
	//	position_->refresh();

	//	print();
	//}
};

void organizer::clear()
{
	htwin_ = sh_ = atwin_ = 0;
	memset(htname_, 0, sizeof(htname_));
	memset(atname_, 0, sizeof(atname_));
	memset(tmp_, 0, sizeof(tmp_));
	memset(result_, 0, sizeof(result_));
	
	flag_ = 0x00;
	index_ = 0;

	forecas_results_.clear();
	base_odds_.clear();
	
	optimization_result_->clear();
	regulator_->clear();
	position_->clear();	
};

void organizer::print(forecas_result_pair rpair)
{
	cout.setf(ios::fixed);
	std::cout<< rpair.first<< ". "
		<< rpair.second.get_result_msg()<< ": "
		<< rpair.second.get_result_odds()<< endl;
};

void organizer::print()
{
	cout<< "-------------------------------------------------------------------"<< std::endl;
	cout<<left <<setw(4) <<"ID" <<right <<setw(6) << "Result"
		<<right <<setw(8) <<"Odds" <<right <<setw(10) << "Multiple"
		<<right <<setw(12) <<"Netincome" <<right <<setw(11) << "TotalCost"
		<<right <<setw(8) << "Yield"<<"%"
		<<right <<setw(12) << "Probability"<<"%"
		<<endl;

	for(forecas_result_map::iterator rpair = get_result_map()->begin();
		rpair != get_result_map()->end();++rpair ) {
		if( rpair->second.get_result_multiple() == 0) 
			continue;

		cout.setf(ios::fixed);
		cout<< left<< setw(4)<< setprecision(0)<< rpair->first
			<< right <<setw(6)<< rpair->second.get_result_msg()
			<< right <<setw(8)<< setprecision(2)<< rpair->second.get_result_odds()
			<< right <<setw(10)<< rpair->second.get_result_multiple()
			<< right <<setw(12)<< setprecision(2)<< rpair->second.get_net_income()
			<< right <<setw(11)<< setprecision(0)<<rpair->second.get_total_cost()
			<< right <<setw(8)<< setprecision(2)<< rpair->second.get_result_yield()
			<< right <<setw(12)<< setprecision(2)<< rpair->second.get_result_probability()
			<< endl;
	}	
};

int organizer::check_odds()
{
	for(forecas_result_map::iterator iter_check = forecas_results_.begin();
		iter_check != forecas_results_.end();++iter_check) {
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
	for(int i=0; i< max_odds_type; ++i) {
		for(int j=0; j< max_odds_type; ++j) {
			result_msg(i, j);
			forecas_result frt(((first[i])*(second[j])), ((1/first[i])*(1/second[j])*100), result_);
			frt.set_flag(flag_);

			forecas_results_.insert(forecas_result_pair(++index_, frt));
		}
	}
};

void organizer::result_msg(int first, int second)
{
	memset(result_, 0, sizeof(result_));
	strncat(result_, msg_type(first), sizeof(result_));
	strncat(result_, msg_type(second), sizeof(result_));

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
	cost_(0), real_size_(0), winning_probability_(0), organizer_(org)
{
	//refresh();
};

void position::clear()
{
	memset(earnings_range_, 0, sizeof(earnings_range_));
	cost_ = 0;
	real_size_ = 0;
	winning_probability_ = 0;
	organizer_ = NULL;		
};

void position::refresh() 
{
	cost_ = 0;

	memset(earnings_range_, 0, sizeof(earnings_range_));
	strncpy(earnings_range_, "The earnings range:", sizeof(earnings_range_));

	//for_each(organizer_->get_result_map()->begin(), organizer_->get_result_map()->end(), 
	//	boost::bind(&position::total_cost, this, _1));

	for(forecas_result_map::iterator iter_total_cost = organizer_->get_result_map()->begin();
		iter_total_cost != organizer_->get_result_map()->end();++iter_total_cost) {
		if(iter_total_cost->second.get_result_multiple() == 0) 
			continue;

		cost_ += iter_total_cost->second.get_result_multiple() * lottery;
	}

	//for_each(organizer_->get_result_map()->begin(), organizer_->get_result_map()->end(), 
	//	boost::bind(&position::set_result_cost, this, _1));

	for(forecas_result_map::iterator iter_set_result_cost = organizer_->get_result_map()->begin();
		iter_set_result_cost != organizer_->get_result_map()->end();++iter_set_result_cost) {
		iter_set_result_cost->second.set_total_cost(cost_);
	}

	//organizer_->print();
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

optimization_result::optimization_result(organizer* org) : 
	max_min_yield_(0), organizer_(org), size_(0)
{
	//container can`t use memcpy as deep copy.
	//memcpy(&optimization_forecas_results_, (organizer_->get_result_map()), sizeof(*organizer_->get_result_map()));
	
	//copy optimization data
	//forecas_result_map::iterator iter_copy = organizer_->get_result_map()->begin();
	//for(iter_copy;iter_copy != organizer_->get_result_map()->end();++iter_copy) {
	//	optimization_forecas_results_.insert(forecas_result_pair(iter_copy->first,iter_copy->second));
	//}

	size_ = organizer_->get_position()->get_real_size();
};

void optimization_result::optimization()
{
	int min_idx = get_result_min_idx();

	forecas_result_map::iterator iter_min = organizer_->get_result_map()->find(min_idx);
	max_min_yield_ = iter_min->second.get_result_yield();

	while (true)
	{
		if(organizer_->get_position()->get_cost() > max_total_cost)
			break;

		organizer_->get_position()->add_someone_position(min_idx);
		min_idx = get_result_min_idx();

		iter_min = organizer_->get_result_map()->find(min_idx);
		double yield = iter_min->second.get_result_yield();

		if(yield > max_min_yield_) {
			max_min_yield_ = yield;
			
			optimization_results_.insert(optimization_result_pair(yield, *organizer_->get_result_map()));  
		}
	}

	print_result();
};

void optimization_result::clear()
{
	max_min_yield_ = 0;
	min_idx = 0;
	size_ = 0;

	organizer_->get_result_map()->clear();
	optimization_results_.clear();
	min_index_.clear();
	
	organizer_ = NULL;		
};

void optimization_result::print_result()
{
	//only using 4 result print
	cout<< "-------------------------------------------------------------------"<< std::endl;
	cout<<left <<setw(4) <<"ID" 
		<<right <<setw(5) <<"Yield" <<"%"
		<<right <<setw(8) <<"Cost" <<"(Y)"  
		<<right <<setw(8) <<"Winning" <<"%"
		<<right <<setw(6) <<"HWAW"
		<<right <<setw(6) <<"HWSH"
		<<right <<setw(6) <<"SHAW"
		<<right <<setw(6) <<"SHSH"
		<<endl;

	int cnt = 0;
	for(optimization_result_map::iterator opair = optimization_results_.begin();
		opair != optimization_results_.end();++opair ) {
			forecas_result_map::iterator iter_result = opair->second.begin();

			cout.setf(ios::fixed);
			cout<< left<< setw(4)<< setprecision(0)<< ++cnt
				<< right<< setw(5)<< setprecision(2)<< opair->first
				<< right<< setw(9)<< setprecision(2)<< iter_result->second.get_total_cost()
				<< right<< setw(11)<< setprecision(2)<< organizer_->get_position()->get_winning_probability()
				<< right<< setw(1) <<" ";

		for(;iter_result!=opair->second.end();++iter_result) {
			if(iter_result->second.get_result_multiple() == 0) 
				continue;
			cout<< right<< setw(6)<< setprecision(0)<< iter_result->second.get_result_multiple();
		}
		cout<< endl;
	}	
};

unsigned int optimization_result::get_result_min_idx()
{
	min_index_.clear();
	for(forecas_result_map::iterator iter_opt = organizer_->get_result_map()->begin();
		iter_opt != organizer_->get_result_map()->end();++iter_opt) {
		if(iter_opt->second.get_result_multiple() == 0)
			continue;
		
		com_data cd;
		cd.odd = iter_opt->second.get_net_income()/lottery;
		cd.index = iter_opt->first;
		min_index_.push_back(cd);
	}

	sort(min_index_.begin(), min_index_.end(), boost::bind(&optimization_result::less_odd,this,_1,_2));

	return min_index_.begin()->index;
};

bool optimization_result::less_odd(const com_data& m1, const com_data& m2) {
        return m1.odd < m2.odd;
}

void optimization_result::print()
{
	cout<< "-------------------------------------------------------------------"<< std::endl;
	cout<<left <<setw(4) <<"ID" <<right <<setw(6) << "Result"
		<<right <<setw(8) <<"Odds" <<right <<setw(10) << "Multiple"
		<<right <<setw(12) <<"Netincome" <<right <<setw(11) << "TotalCost"
		<<right <<setw(8) << "Yield"<<"%"
		<<right <<setw(12) << "Probability"<<"%"
		<<endl;

	for(forecas_result_map::iterator rpair = organizer_->get_result_map()->begin();
		rpair  != organizer_->get_result_map()->end();++rpair ) {
		if( rpair->second.get_result_multiple() == 0) 
			continue;

		cout.setf(ios::fixed);
		cout<< left<< setw(4)<< setprecision(0)<< rpair->first
			<< right <<setw(6)<< rpair->second.get_result_msg()
			<< right <<setw(8)<< setprecision(2)<< rpair->second.get_result_odds()
			<< right <<setw(10)<< rpair->second.get_result_multiple()
			<< right <<setw(12)<< setprecision(2)<< rpair->second.get_net_income()
			<< right <<setw(11)<< setprecision(0)<<rpair->second.get_total_cost()
			<< right <<setw(8)<< setprecision(2)<< rpair->second.get_result_yield()
			<< right <<setw(12)<< setprecision(2)<< rpair->second.get_result_probability()
			<< endl;
	}
};

//-----------------------------------------------------------------------

regulator::regulator(organizer* org) : 
	adjusted_income_(0), adjusted_yield_(0), organizer_(org), mode_(none_modem)
{	
};

void regulator::clear()
{
	adjusted_income_ = 0;
	adjusted_yield_ = 0;
	organizer_ = NULL;	
};

void regulator::hedge_positions()
{
	int map_size = organizer_->get_position()->get_real_size();
	while(true)	{
		int break_cnt = 0;	
		for(forecas_result_map::iterator iter = organizer_->get_result_map()->begin(); 
			iter!= organizer_->get_result_map()->end(); ++iter) {
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
		for(forecas_result_map::iterator iter = organizer_->get_result_map()->begin(); 
			iter != organizer_->get_result_map()->end(); ++iter) {
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
		for(forecas_result_map::iterator iter = organizer_->get_result_map()->begin(); 
			iter!= organizer_->get_result_map()->end(); ++iter) {
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
	for(forecas_result_map::iterator iter_size = organizer_->get_result_map()->begin();
		iter_size != organizer_->get_result_map()->end();++iter_size ) {
		if(iter_size->second.get_result_multiple() > 0)
			++real_size;
	}

	int map_size = real_size;
	while(true) {
		int break_cnt = 0;
		for(forecas_result_map::iterator iter = organizer_->get_result_map()->begin(); 
			iter != organizer_->get_result_map()->end(); ++iter) {
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
	//using this
	//for_each(organizer_->get_result_map()->begin(), organizer_->get_result_map()->end(), 
	//	boost::bind(&regulator::init_results, this, _1));
	//can`t proper operation clear_dynamic_data() why? 
	//i suspect with multiple calls related to the copy constructor

	for_each(organizer_->get_result_map()->begin(), organizer_->get_result_map()->end(), 
		boost::bind(&regulator::init_results, this, boost::bind<forecas_result&>(&forecas_result_map::value_type::second,_1)));
	
	//for(forecas_result_map::iterator iter_clear = organizer_->get_result_map()->begin();
	//	iter_clear != organizer_->get_result_map()->end();++iter_clear){
	//	iter_clear->second.clear_dynamic_data();
	//}

	mode_ = mode;
	init_position();
};

void regulator::init_position()
{
	if(mode_ == whole_mode) {
		add_all_position();
	}
	else if(mode_ == portion_mode) {
		add_all_position(false);
	}
	else if(mode_ == manual_mode)
	{
	
	}

	int real_size=0;
	double winning_probability=0;
	for(forecas_result_map::iterator iter_size = organizer_->get_result_map()->begin();
		iter_size != organizer_->get_result_map()->end();++iter_size ) {
		if(iter_size->second.get_result_multiple() > 0) {
			++real_size;
			winning_probability += iter_size->second.get_result_probability();
		}
	}

	organizer_->get_position()->set_real_size(real_size);
	organizer_->get_position()->set_winning_probability(winning_probability);

	organizer_->get_position()->refresh();

	hedge_positions();
};

void regulator::add_all_position(bool is_all)
{	
	for(forecas_result_map::iterator iter_setmul = organizer_->get_result_map()->begin();
		iter_setmul != organizer_->get_result_map()->end();++iter_setmul ) {
		if(!is_all && iter_setmul->second.get_flag()&away_win) 
			continue;

		iter_setmul->second.set_result_multiple(1);
	}	
};

void regulator::init_results(forecas_result& rpair)
{
	rpair.clear_dynamic_data();
};

//-----------------------------------------------------------------------
