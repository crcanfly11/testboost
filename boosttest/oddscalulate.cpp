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
	odds_(odds), multiple_(0), income_(0), yield_(0), total_cost_(0), net_income_(0)
{
	strcpy_s(result_msg_, sizeof(result_msg_), result_msg);
	//yield_ = (odds - 1)*100;
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
};

//-----------------------------------------------------------------------

organizer::organizer() : htwin_(0), sh_(0), atwin_(0)
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

	cout<< "every case of results."<< endl;
	for_each(forecas_results_.begin(), forecas_results_.end(), 
		boost::bind(&organizer::print, this, _1));

	//cout<< "How to buy? all in without loss or buy Specify combination."<< endl;
	
	position_ = new position(this);
	regulator_ = new regulator(this);

	cout<< "Select mode 1, portion_mode; 2, whole_mode;"<< endl;
	char smode[10];
	cin>> skipws>> smode;
	play_mode mode;
	if(atoi(smode) == 1) {
		mode = portion_mode;

		cout<< "Weed out results is HL,AL"<< endl;
	}
	else if(atoi(smode) == 2) {
		mode = whole_mode;
	}
	
	regulator_->set_play_mode(mode);
	regulator_->set_adjusted_min_income(0);

	cout<< "-----------------------------------------"<< endl;
	for_each(forecas_results_.begin(), forecas_results_.end(), 
		boost::bind(&organizer::print_result, this, _1));

};

void organizer::print(forecas_result_pair rpair)
{
	std::cout<< rpair.first<< ". "<<rpair.second.get_result_msg()<< ": "<< rpair.second.get_result_odds()<< endl;
};

void organizer::print_result(forecas_result_pair rpair)
{
	if(!rpair.second.get_result_multiple()) return;

	std::cout<< rpair.first<< ". "<< rpair.second.get_result_msg()<< ": "<< rpair.second.get_result_odds()<< 
		" multiple:"<< rpair.second.get_result_multiple()<< " net income:"<< rpair.second.get_net_income()<<
		" total cost:"<< rpair.second.get_total_cost()<< " yield:"<< rpair.second.get_result_yield()<< endl;
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
			result_msg(i, j);
			forecas_result* frt = new forecas_result(((first[i])*(second[j])), result_);
			
			num++;
			forecas_results_.insert(forecas_result_pair(num, *frt));
		}
	}
};

void organizer::result_msg(int first, int second)
{
	memset(result_, 0, sizeof(result_));

	switch(first) {
	case 0:
		strcat_s(result_,sizeof(result_),"1");
		break;
	case 1:
		strcat_s(result_,sizeof(result_),"2");
		break;
	case 2:
		strcat_s(result_,sizeof(result_),"3");
		break;
	}

	switch(second) {
	case 0:
		strcat_s(result_,sizeof(result_),"4");
		break;
	case 1:
		strcat_s(result_,sizeof(result_),"2");
		break;
	case 2:
		strcat_s(result_,sizeof(result_),"5");
		break;
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
		if(iter_total_cost->second.get_result_multiple() == 0) return;

		cost_ += iter_total_cost->second.get_result_multiple() * lottery;
	}


	//for_each(organizer_->get_result_map()->begin(), organizer_->get_result_map()->end(), 
	//	boost::bind(&position::set_result_cost, this, _1));

	forecas_result_map::iterator iter_set_result_cost = organizer_->get_result_map()->begin();
	for(iter_set_result_cost;iter_set_result_cost != organizer_->get_result_map()->end();++iter_set_result_cost) {
		iter_set_result_cost->second.set_total_cost(cost_);
	}

	//boost::bind Ƕ��
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

void position::add_someone_position(unsigned int index)
{
   forecas_result_map::iterator iter = organizer_->get_result_map()->find(index);

   if(iter == organizer_->get_result_map()->end()) 
	   return ;

   iter->second.set_result_multiple(iter->second.get_result_multiple()+1);

   refresh();	
};

//-----------------------------------------------------------------------

regulator::regulator(organizer* org) : 
	adjusted_income_(0), adjusted_yield_(0), organizer_(org), mode_(none_modem)
{	
};

void regulator::set_adjusted_min_income(double min_income)
{
	adjusted_income_ = min_income;
	adjust_positions(adjusted_income_);
};

void regulator::set_adjusted_min_yield(double min_yield)
{
	adjusted_yield_ = min_yield;
	adjust_positions(adjusted_yield_);
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
					int add_multiple = (int)((organizer_->get_position()->get_cost()-lottery*odd*multiple+value)/(lottery*(odd-1))) + 1; 
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
	for_each(organizer_->get_result_map()->begin(), organizer_->get_result_map()->end(), 
		boost::bind(&regulator::init_results, this, _1));
 
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
			std::string str(iter_setmul->second.get_result_msg());

			if(str.find("3")!=string::npos || str.find("5")!=string::npos) continue;

			iter_setmul->second.set_result_multiple(1);
		}		
	}
	else if(mode_ == manual_mode)
	{
	
	}

	//organizer_->get_position()->set_real_size(real_size);

	organizer_->get_position()->refresh();

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