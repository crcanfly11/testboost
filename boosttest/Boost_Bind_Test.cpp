#include <iostream>
#include <string>
#include "boost/bind.hpp"

using namespace std;

////ռλ��ʲô��ʲôʱ����/////
//void nine_arguments(int i1,int i2,int i3,int i4,
//					int i5,int i6,int i7,int i8, int i9) 
//{
//  std::cout << i1 << i2 << i3 << i4 << i5
//    << i6 << i7 << i8 << i9 << '\n';
//}
//
//int main() {
//  int i1=1,i2=2,i3=3,i4=4,i5=5,i6=6,i7=7,i8=8,i9=9;
//  (boost::bind(&nine_arguments,_9,_2,_1,_6,_3,_8,_4,_5,_7))
//    (i1,i2,i3,i4,i5,i6,i7,i8,i9);
//
//  //ʲô�������
//  boost::bind(&nine_arguments,i1,i2,i3,i4,i5,i6,i7,i8,i9);
//
//  std::system("pause");
//}

////for_each()�� bind Ӧ��////
//#include <algorithm>  //for_each() 
//#include <functional> //mem_fun() mem_fun_ref()
//#include <vector>
//
//using namespace std;
//
//class status_t
//{
//	string name_;
//	bool ok_;
//public:
//	status_t(const string& name) : name_(name),ok_(true) {}
//
//	void break_it() { ok_ = false; }
//	bool is_broken() const { return ok_; }
//
//	void report() const
//	{
//		cout<< name_.c_str()<< " is "<< (ok_?"working normal":"borken")<< endl;
//	}
//	
//	void reportInt(int index) const
//	{
//		cout<< " in index is "<< index<< endl;
//	}
//};
//
//void reportn() 
//{
//	cout<< "nomenber class"<< endl;
//}
//
//void main()
//{
//	vector<status_t> status;
//	status.push_back(status_t("status 1"));
//	status.push_back(status_t("status 2"));
//	status.push_back(status_t("status 3"));
//	status.push_back(status_t("status 4"));
//
//	status[1].break_it();
//	status[2].break_it();
//
//	for_each(status.begin(),status.end(),std::mem_fun_ref(&status_t::report));
//
//	vector<status_t*> status_p;
//	status_p.push_back(new status_t("status_p 1"));
//	status_p.push_back(new status_t("status_p 2"));
//	status_p.push_back(new status_t("status_p 3"));
//	status_p.push_back(new status_t("status_p 4"));
//
//	status_p[1]->break_it();
//	status_p[3]->break_it();	
//
//	for_each(status_p.begin(),status_p.end(),mem_fun(&status_t::report));
//
//	cout<< "use boost::bind method."<< endl;
//	for_each(status_p.begin(),status_p.end(),(boost::bind(&reportn)));
//
//	//������ת�� ����main����������this,thisֻ�����ڷǾ�̬��Ա������
//	for_each(status.begin(),status.end(),(boost::bind(&status_t::reportInt,_1,_2))); 
//
//	system("pause");
//}


////�󶨺���////
//class some_class 
//{
//public:
//	typedef void result_type;
//
//	some_class() 
//	{
//		std::cout<< "construct called."<< std::endl;
//	}
//
//	void print_string(const std::string& s) const 
//	{
//		std::cout<< "call print_stirng method"<< std::endl;
//		std::cout << s << '\n';
//	}
//};
//
//void print_string(const std::string s) 
//{
//	std::cout << s << '\n';
//}
//
//int main() {
//  (boost::bind(&print_string,_1))("Hello func!");
//
//  std::cout<< "class bind test."<< std::endl;
//
//  //some_class sc;
//  some_class* p_sc = new some_class();
//
//  (boost::bind(&some_class::print_string,_1,_2))(p_sc,"Hello member!");
//
//  system("pause");
//}

////bind�� ��������////
//class tracer {
//public:
//  tracer() {
//    std::cout << "tracer::tracer()\n";
//  }
//
//  tracer(const tracer& other) {
//    std::cout << "tracer::tracer(const tracer& other)\n";
//  }
//
//  tracer& operator=(const tracer& other) {
//    std::cout <<
//      "tracer& tracer::operator=(const tracer& other)\n";
//    return *this;
//  }
//
//  ~tracer() {
//    std::cout << "tracer::~tracer()\n";
//  }
//
//  void print(const std::string& s) const {
//    std::cout << s << '\n';
//  }
//};
//
//void main()
//{
//	tracer t;
//	
//	std::cout<< "start bind construct"<< std::endl;
//	//�����5�ο������캯����5����������
//	//����6�� Ϊʲô������
//	//boost::bind(&tracer::print,t,_1)(std::string("I'm called on a copy of t\n"));
//
//	//boost::ref() �������ã�boost::cref() ����const���� 
//	//���� boost::ref() ���캯���������������Ե�����һ��
//	//boost::bind(&tracer::print,boost::ref(t),_1)(std::string("I'm called on a copy of t\n"));   
//
//	//������ָ���ʱ�� ֻ�´��һ�� Ч����������ʱ��refһ��
//	boost::bind(&tracer::print,&t,_1)(std::string("I'm called directly on t\n"));
//
//	std::system("pause");
//}

////���麯�� 
//class base
//{
//public:
//	virtual void print()
//	{
//		std::cout<< "call base print."<< std::endl;
//	}
//};
//
//class drive : public base
//{
//public:
//	virtual void print()
//	{
//		std::cout<< "call drive print."<< std::endl;
//	}
//};
//
//void main()
//{
//	base b;
//	drive d;
//
//	base* p_b = new base();
//	drive* p_d = new drive();
//
//	(boost::bind(&base::print,_1)(b));
//	(boost::bind(&base::print,_1)(d));
//
//	(boost::bind(&base::print,_1)(p_b));
//	(boost::bind(&base::print,_1)(p_d));
//
//	std::system("pause");
//}

//#include <map>
//#include <algorithm>
////�󶨳�Ա����////
//void print_string(const std::string& str) 
//{
//	std::cout<< str<< std::endl;
//}
//
////typedef std::map<unsigned int, std::string> map_type;
//typedef std::map<char, std::string> map_type;
//
//void main()
//{
//	//std::map<unsigned int, std::string> my_map;
//	std::map<char, std::string> my_map;
//	my_map['a'] = "boost";
//	my_map['b'] = "bind";
//
//  //bindʱ ��һ����������ֵ���ڶ������������
//	std::for_each(
//		my_map.begin(),
//		my_map.end(),
//		boost::bind(&print_string,
//			boost::bind(&map_type::value_type::second,_1)
//		)
//	);
//
//	std::system("pause");
//}

////��� bindǶ��////
//#include <map>
//#include <vector>
//#include <algorithm>
//
//void print(std::ostream* os,int i) {
//  (*os) << i << '\n';
//}
////��װ print���� ���ü�////
////print_size ����״̬�� ״̬������std::ostream& os_�� 
//class print_size
//{
//	std::ostream& os_;
//	typedef map<string,vector<int>> map_type;
//public:
//	print_size(std::ostream& os) : os_(os) 
//	{
//		cout<< "calling print_size construct."<< endl;
//	}
//
//	void operator() (const map_type::value_type& value) const
//	{
//		os_<< value.second.size()<< endl;
//	}
//};
////for_each(m.begin(),m.end(),print_size(std::cout));  //ʹ�ü� 
//
////ȥ����״̬������resulttype �û���ʹ�� bind ʱ�Ͳ���Ҫ��ʽ������������ķ�������////
//class print_size1 {
//	typedef std::map<std::string, std::vector<int> > map_type;
//public:
//	typedef void result_type;
//	result_type operator()(std::ostream& os, const map_type::value_type& x) const 
//	{
//		os << x.second.size() << '\n';
//	}
//};
////std::for_each(m.begin(),m.end(), boost::bind(print_size(),boost::ref(std::cout),_1));
//
//typedef map<string,vector<int>> my_map_strint;
//void main()
//{
//	vector<int> vint;
//	vint.push_back(1);
//	vint.push_back(3);
//	vint.push_back(5);
//
//	std::map<std::string,std::vector<int> > m;
//    //����� 3 2
//	//m["Strange"].push_back(1);
//	//m["Strange"].push_back(2);
//	//m["Strange"].push_back(3);
//	//m["Weird"].push_back(4);
//	//m["Weird"].push_back(5);
//
//	m["Strange"] = vint;
//	vint.push_back(7);
//	m["Strange"] = vint;
//
//	m["Weird"] = vint;
//
//	cout<< "bind print method."<< endl;
//	std::for_each(
//		m.begin(),
//		m.end(),
//		boost::bind(&print,
//					&std::cout,
//					boost::bind(&std::vector<int>::size,
//								boost::bind(&my_map_strint::value_type::second,_1))));
//
//	cout<< "bind print_size method."<< endl;
//	std::for_each(m.begin(),m.end(),print_size(std::cout));
//
//	cout<< "bind print_size1 method."<< endl;
//	std::for_each(m.begin(),m.end(), boost::bind(print_size1(),boost::ref(std::cout),_1));
//
//	std::system("pause");
//}

////bind ԭ�����////
//class test_fun
//{
//public:
//	test_fun()  //bindʱ ������������1��
//	{
//		cout<< "calling test_fun() construct."<< endl;
//	}
//
//	test_fun(const test_fun& tf) //bindʱ ���������������
//	{
//		cout<< "calling test_fun(const test_fun& tf) construct."<< endl;
//	}
//
//	~test_fun()  //����6�� Ϊʲô
//	{
//		cout<< "calling ~test_fun() destroy."<< endl;
//	}
//
//	void fun(int i) 
//	{
//		std::cout<< "calling fun method."<< endl;
//		std::cout<< "input is "<< i<< endl;
//	}
//};
//
//void main()
//{
//	test_fun t; 
//	
//	//��boost::ref()(����boost::cref())���Ա��⿽����κ��������
//	(boost::bind(&test_fun::fun,boost::ref(t),_1)(11));
//
//	system("pause");
//}

#include <algorithm>  

template<typename _Arg1, typename _Arg2, typename _Result>  
struct binary_function2 
{  
    typedef _Arg1 first_argument_type;  
    typedef _Arg2 second_argument_type;  
    typedef _Result result_type;  
};  
  
template<typename _Tp>  
struct equal_to2: public binary_function2<_Tp, _Tp, bool> 
{  
    bool operator()(const _Tp& __x, const _Tp& __y) const 
	{  
        return __x == __y;  
    }  
};  
  
template<typename _Arg, typename _Result>  
struct unary_function2 
{  
    typedef _Arg argument_type;  
    typedef _Result result_type;  
};  
  
template<typename _Operation>  
class binder1st2: public unary_function2<typename _Operation::second_argument_type, typename _Operation::result_type> 
{  
protected:  
    _Operation op;  
    typename _Operation::first_argument_type value;  
  
public:  
    binder1st2(const _Operation& __x, const typename _Operation::first_argument_type& __y) : op(__x), value(__y) {}  
  
    typename _Operation::result_type operator()(const typename _Operation::second_argument_type& __x) const 
	{  
        return op(value, __x);  
    }  
  
    typename _Operation::result_type operator()(typename _Operation::second_argument_type& __x) const 
	{  
        return op(value, __x);  
    }  
};  
  
template<typename _Operation, typename _Tp>  
inline binder1st2<_Operation> bind1st2(const _Operation& __fn, const _Tp& __x) 
{  
    typedef typename _Operation::first_argument_type _Arg1_type;  
    return binder1st2<_Operation>(__fn, _Arg1_type(__x));  
}  

//int main() {  
//    binder1st2<equal_to2<int>> equal_to_10(equal_to2<int>(), 30);  
//
//    int numbers[] = { 10, 20, 30, 40, 50, 30 , 10 , 30 };  
//    int cx;  
//    cx = std::count_if(numbers, numbers + 8, bind1st2(equal_to2<int>(), 30));  
//    cout << "There are " << cx << " elements that are equal to 30.\n";  
//
//	std::system("pause");
//}

